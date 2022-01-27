#include "ffmpeg_pipeline.h"

#include <cstdio>
#include <cstring>
#include <stdarg.h>
#include "log.h"

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
    thread_ = nullptr;

    av_log_set_callback(log_callback);
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

    thread_ = new std::thread(&FFmpegPipeline::DecodeThread, this);

    return true;
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
//        if (dec->codec->type == AVMEDIA_TYPE_VIDEO)
//            ret = output_video_frame(frame_);
//        else
//            ret = output_audio_frame(frame_);

        av_frame_unref(frame_);
        if (ret < 0) {
            return ret;
        }
    }

    return 0;
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
