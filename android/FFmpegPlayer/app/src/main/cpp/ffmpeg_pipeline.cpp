#include "ffmpeg_pipeline.h"

#include <cstdio>
#include <cstring>
#include <stdarg.h>
#include "utils/log.h"

static const int AUDIO_DST_SAMPLE_RATE = 44100; // 音频编码采样率
static const int AUDIO_DST_CHANNEL_COUNTS = 2; // 音频编码通道数
static const uint64_t AUDIO_DST_CHANNEL_LAYOUT = AV_CH_LAYOUT_STEREO; // 音频编码声道格式
static const AVSampleFormat DST_SAMPLT_FORMAT = AV_SAMPLE_FMT_S16;
static const int ACC_NB_SAMPLES = 1024; // ACC音频一帧采样数

static void log_callback(void *ptr, int level, const char *fmt, va_list vl) {
    va_list vl2;
    char *line = (char *) malloc(128 * sizeof(char));
    static int print_prefix = 1;
    va_copy(vl2, vl);
    av_log_format_line(ptr, level, fmt, vl2, line, 128, &print_prefix);
    va_end(vl2);
    line[127] = '\0';
    LOGE("%s", line);
    free(line);
}

FFmpegPipeline::FFmpegPipeline(const char *url) : url_(url) {
    TRACE_FUNC();
    fmt_ctx_ = nullptr;
    video_dec_ctx_ = nullptr;
    audio_dec_ctx_ = nullptr;
    video_stream_ = nullptr;
    audio_stream_ = nullptr;
    video_stream_idx_ = -1;
    audio_stream_idx_ = -1;
    frame_ = nullptr;
    packet_ = nullptr;
    swr_ctx_ = nullptr;
    sws_ctx_ = nullptr;
    thread_ = nullptr;

//    av_log_set_callback(log_callback);
}

FFmpegPipeline::~FFmpegPipeline() {
    avcodec_free_context(&video_dec_ctx_);
    avcodec_free_context(&audio_dec_ctx_);
    avformat_close_input(&fmt_ctx_);
    av_frame_free(&frame_);
    av_packet_free(&packet_);
}

bool FFmpegPipeline::Init() {
    TRACE_FUNC();
    bool ret = false;

    // open input file, and allocate format context
    if (avformat_open_input(&fmt_ctx_, url_.c_str(), NULL, NULL) < 0) {
        LOGE("Could not open source file %s", url_.c_str());
        return ret;
    }

    // retrieve stream information
    if (avformat_find_stream_info(fmt_ctx_, NULL) < 0) {
        LOGE("Could not find stream information");
        return ret;
    }

    if (OpenCodecContext(&video_stream_idx_, &video_dec_ctx_, fmt_ctx_, AVMEDIA_TYPE_VIDEO) >= 0) {
        video_stream_ = fmt_ctx_->streams[video_stream_idx_];
    }

    if (OpenCodecContext(&audio_stream_idx_, &audio_dec_ctx_, fmt_ctx_, AVMEDIA_TYPE_AUDIO) >= 0) {
        audio_stream_ = fmt_ctx_->streams[audio_stream_idx_];
    }

    if (!video_stream_ && !audio_stream_) {
        LOGE("Could not find audio or video stream in the input, aborting");
        return ret;
    }

    if (!InitSwresample(audio_dec_ctx_->channel_layout, audio_dec_ctx_->sample_rate,
                        audio_dec_ctx_->sample_fmt, ACC_NB_SAMPLES, AUDIO_DST_CHANNEL_LAYOUT,
                        AUDIO_DST_SAMPLE_RATE, DST_SAMPLT_FORMAT)) {
        LOGE("InitSwscale failed!");
        return ret;
    }

    // dump input information to stderr
    av_dump_format(fmt_ctx_, 0, url_.c_str(), 0);

    frame_ = av_frame_alloc();
    if (!frame_) {
        LOGE("Could not allocate frame");
        return ret;
    }

    packet_ = av_packet_alloc();
    if (!packet_) {
        LOGE("Could not allocate packet");
    }

    return true;
}

void FFmpegPipeline::Start() {
    thread_ = new std::thread(&FFmpegPipeline::DecodeThread, this);
}

bool FFmpegPipeline::OpenCodecContext(int *stream_idx,
                                      AVCodecContext **dec_ctx, AVFormatContext *fmt_ctx,
                                      enum AVMediaType type) {
    TRACE_FUNC();
    bool ret = false;

    int stream_index = av_find_best_stream(fmt_ctx, type, -1, -1, NULL, 0);
    if (stream_index < 0) {
        LOGE("Could not find %s stream in input file '%s'\n",
             av_get_media_type_string(type), url_.c_str());
        return ret;
    }

    AVStream *st = fmt_ctx->streams[stream_index];

    // find decoder for the stream
    const AVCodec *dec = NULL;
    dec = avcodec_find_decoder(st->codecpar->codec_id);
    if (!dec) {
        LOGE("Failed to find %s codec", av_get_media_type_string(type));
        return ret;
    }

    // Allocate a codec context for the decoder
    *dec_ctx = avcodec_alloc_context3(dec);
    if (!*dec_ctx) {
        LOGE("Failed to allocate the %s codec context", av_get_media_type_string(type));
        return ret;
    }

    // Copy codec parameters from input stream to output codec context
    if (avcodec_parameters_to_context(*dec_ctx, st->codecpar) < 0) {
        LOGE("Failed to copy %s codec parameters to decoder context",
             av_get_media_type_string(type));
        return ret;
    }

    // Init the decoders
    if (avcodec_open2(*dec_ctx, dec, NULL) < 0) {
        LOGE("Failed to open %s codec\n", av_get_media_type_string(type));
        return ret;
    }

    *stream_idx = stream_index;

    return true;
}

AudioFrame *FFmpegPipeline::GetAudioFrame() {
    while (audio_frame_queue_.Empty()) {
        std::this_thread::sleep_for(std::chrono::microseconds(10));
    }
    return audio_frame_queue_.Pop();
}

int FFmpegPipeline::OutputAudioFrame(AVFrame *frame) {
    AudioFrame *audio_frame = new AudioFrame(dst_frame_data_size_);
    int ret = swr_convert(swr_ctx_, &(audio_frame->data_), dst_frame_data_size_ / 2,
                          (const uint8_t **) frame->data, frame->nb_samples);
    if (ret <= 0) {
        LOGE("swr_convert failed!");
        return ret;
    }

    audio_frame_queue_.Push(audio_frame);
    return ret;
}

void FFmpegPipeline::SetVideoRander(VideoRenderInterface *video_render) {
    TRACE_FUNC();
    video_render_ = video_render;
}

int FFmpegPipeline::OutputVideoFrame(AVFrame *frame) {
    sws_scale(sws_ctx_, frame->data, frame->linesize, 0,
              video_dec_ctx_->height, rgb_frame_->data, rgb_frame_->linesize);
    NativeImage image;
    image.format = IMAGE_FORMAT_RGBA;
    image.width = render_width_;
    image.height = render_height_;
    image.ppPlane[0] = rgb_frame_->data[0];
    image.pLineSize[0] = image.width * 4;
    video_render_->RenderVideoFrame(&image);

    return 0;
}

int FFmpegPipeline::DecodePacket(AVCodecContext *dec, const AVPacket *pkt) {
    int ret = 0;

    // submit the packet to the decoder
    ret = avcodec_send_packet(dec, pkt);
    if (ret < 0) {
        LOGE("Error submitting a packet for decoding (%s)", av_err2str(ret));
        return ret;
    }

    // get all the available frames from the decoder
    while (ret >= 0) {
        ret = avcodec_receive_frame(dec, frame_);
        if (ret < 0) {
            // those two return values are special and mean there is no output
            // frame available, but there were no errors during decoding
            if (ret == AVERROR_EOF || ret == AVERROR(EAGAIN))
                return 0;

            LOGE("Error during decoding (%s)", av_err2str(ret));
            return ret;
        }

        // render the frame
        if (dec->codec->type == AVMEDIA_TYPE_VIDEO)
            ret = OutputVideoFrame(frame_);
        else
            ret = OutputAudioFrame(frame_);

        av_frame_unref(frame_);
        if (ret < 0) {
            return ret;
        }
    }

    return 0;
}

bool FFmpegPipeline::InitSwresample(int64_t src_ch_layout, int src_rate,
                                    enum AVSampleFormat src_sample_fmt, int src_nb_samples,
                                    int64_t dst_ch_layout, int dst_rate,
                                    enum AVSampleFormat dst_sample_fmt) {
    TRACE_FUNC();
    LOGI("src: sample rate: %d, format: %d, layout: %lld", src_rate,
         src_sample_fmt, src_ch_layout);
    LOGI("dst: sample rate: %d, format: %d, layout: %lld", dst_rate,
         dst_sample_fmt, dst_ch_layout);

    bool ret = false;

    swr_ctx_ = swr_alloc();
    if (!swr_ctx_) {
        LOGE("Could not allocate resampler context");
        return ret;
    }

    /* set options */
    av_opt_set_int(swr_ctx_, "in_channel_layout", src_ch_layout, 0);
    av_opt_set_int(swr_ctx_, "in_sample_rate", src_rate, 0);
    av_opt_set_sample_fmt(swr_ctx_, "in_sample_fmt", src_sample_fmt, 0);

    av_opt_set_int(swr_ctx_, "out_channel_layout", dst_ch_layout, 0);
    av_opt_set_int(swr_ctx_, "out_sample_rate", dst_rate, 0);
    av_opt_set_sample_fmt(swr_ctx_, "out_sample_fmt", dst_sample_fmt, 0);

    /* initialize the resampling context */
    if ((ret = swr_init(swr_ctx_)) < 0) {
        LOGE("Failed to initialize the resampling context");
        return ret;
    }

    dst_nb_samples_ = (int) av_rescale_rnd(src_nb_samples, dst_rate, src_rate, AV_ROUND_UP);
    dst_frame_data_size_ = av_samples_get_buffer_size(NULL, av_get_channel_layout_nb_channels(
            dst_ch_layout), dst_nb_samples_, dst_sample_fmt, 1);

    return true;
}

bool FFmpegPipeline::GetVideoWidthAndHeight(int *w, int *h) {
    *w = video_dec_ctx_->width;
    *h = video_dec_ctx_->height;
    return true;
}

bool FFmpegPipeline::InitSwscale(int src_width, int src_height, enum AVPixelFormat src_format,
                                 int dst_width, int dst_height, enum AVPixelFormat dst_format) {
    TRACE_FUNC();
    render_width_ = dst_width;
    render_height_ = dst_height;
    video_width_ = video_dec_ctx_->width;
    video_height_ = video_dec_ctx_->height;
    src_width = video_dec_ctx_->width;
    src_height = video_dec_ctx_->height;
    src_format = video_dec_ctx_->pix_fmt;
    LOGI("src: src_width: %d, src_height: %d, src_format: %d", src_width, src_height, src_format);
    LOGI("dst: dst_width: %d, dst_height: %d, dst_format: %d", dst_width, dst_height, dst_format);

    rgb_frame_ = av_frame_alloc();
    int buffer_size = av_image_get_buffer_size(dst_format, dst_width, dst_height, 1);
    frame_buffer_ = (uint8_t *) av_malloc(buffer_size * sizeof(uint8_t));
    av_image_fill_arrays(rgb_frame_->data, rgb_frame_->linesize,
                         frame_buffer_, dst_format, dst_width, dst_height, 1);

    sws_ctx_ = sws_getContext(src_width, src_height, src_format, dst_width, dst_height, dst_format,
                              SWS_FAST_BILINEAR, NULL, NULL, NULL);

    return true;
}

void FFmpegPipeline::DecodeThread() {
    TRACE_FUNC();
    int ret;

    // read frames from the file
    while (av_read_frame(fmt_ctx_, packet_) >= 0) {
        // check if the packet belongs to a stream we are interested in, otherwise skip it
        if (packet_->stream_index == video_stream_idx_) {
            ret = DecodePacket(video_dec_ctx_, packet_);
        } else if (packet_->stream_index == audio_stream_idx_) {
            ret = DecodePacket(audio_dec_ctx_, packet_);
        }
        av_packet_unref(packet_);
        if (ret < 0) {
            break;
        }
    }

    // flush the decoders
    if (video_dec_ctx_)
        DecodePacket(video_dec_ctx_, NULL);
    if (audio_dec_ctx_)
        DecodePacket(audio_dec_ctx_, NULL);
}

std::string FFmpegPipeline::GetFFmpegVersion() {
    TRACE_FUNC();
    char strBuffer[1024 * 4] = {0};
    strcat(strBuffer, "libavcodec : ");
    strcat(strBuffer, AV_STRINGIFY(LIBAVCODEC_VERSION));
    strcat(strBuffer, "\nlibavformat : ");
    strcat(strBuffer, AV_STRINGIFY(LIBAVFORMAT_VERSION));
    strcat(strBuffer, "\nlibavutil : ");
    strcat(strBuffer, AV_STRINGIFY(LIBAVUTIL_VERSION));
    strcat(strBuffer, "\nlibavfilter : ");
    strcat(strBuffer, AV_STRINGIFY(LIBAVFILTER_VERSION));
    strcat(strBuffer, "\nlibswresample : ");
    strcat(strBuffer, AV_STRINGIFY(LIBSWRESAMPLE_VERSION));
    strcat(strBuffer, "\nlibswscale : ");
    strcat(strBuffer, AV_STRINGIFY(LIBSWSCALE_VERSION));
    strcat(strBuffer, "\navcodec_configure : \n");
    strcat(strBuffer, avcodec_configuration());
    strcat(strBuffer, "\navcodec_license : ");
    strcat(strBuffer, avcodec_license());
    LOGE("GetFFmpegVersion:\n%s", strBuffer);

    return std::string(strBuffer);
}
