#include "ffmpeg_demuxer.h"
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

FFmpegDemuxer::FFmpegDemuxer(const char *url) : url_(url) {
    TRACE_FUNC();
    fmt_ctx_ = nullptr;
    video_dec_ctx_ = nullptr;
    audio_dec_ctx_ = nullptr;
    video_stream_ = nullptr;
    audio_stream_ = nullptr;
    video_stream_idx_ = -1;
    audio_stream_idx_ = -1;
    //  av_log_set_callback(log_callback);
}

FFmpegDemuxer::~FFmpegDemuxer() {
    avcodec_free_context(&video_dec_ctx_);
    avcodec_free_context(&audio_dec_ctx_);
    avformat_close_input(&fmt_ctx_);
}

bool FFmpegDemuxer::Init() {
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

    LOGE("audio_stream_idx_ : %d",audio_stream_idx_);
    LOGE("video_stream_idx_ : %d",video_stream_idx_);

    if (!video_stream_ && !audio_stream_) {
        LOGE("Could not find audio or video stream in the input, aborting");
        return ret;
    }

    // dump input information to stderr
    av_dump_format(fmt_ctx_, 0, url_.c_str(), 0);

    return true;
}

AVCodecContext *FFmpegDemuxer::GetCodecContext(enum AVMediaType type) {
    if (type == AVMEDIA_TYPE_VIDEO) {
        return video_dec_ctx_;
    } else {
        return audio_dec_ctx_;
    }
}

AVPacket *FFmpegDemuxer::GetPacket(enum AVMediaType type) {
    if (type == AVMEDIA_TYPE_VIDEO) {
        return video_packet_queue_.Pop();
    } else {
        return audio_packet_queue_.Pop();;
    }
}

bool FFmpegDemuxer::OpenCodecContext(int *stream_idx,
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

void FFmpegDemuxer::Process() {
    TRACE_FUNC();
    AVPacket *packet = av_packet_alloc();
    if (!packet) {
        LOGE("Could not allocate packet");
        return;
    }

    if(packet->data == nullptr) {
        LOGE("packet->data===========1========== is null, size %d",packet->size);
    } else {
        LOGE("packet->data============1========= is not null %d,size %d",packet->data,packet->size);
        LOGE("pkt->data===========1====pkt->stream_index : %d",packet->stream_index);
    }
    if(packet->buf == nullptr) {
        LOGE("packet->buf===========1========== is null");
    } else {
        LOGE("packet->buf===========1========== is not null");
    }

    // read frames from the file
    if (av_read_frame(fmt_ctx_, packet) >= 0) {
        // check if the packet belongs to a stream we are interested in, otherwise skip it
        if (packet->stream_index == video_stream_idx_) {
            video_packet_queue_.Push(packet);
        } else if (packet->stream_index == audio_stream_idx_) {
            if(packet->data == nullptr) {
                LOGE("packet->data===========2========== is null");
            } else {
                LOGE("packet->data===========2========== is not null %d, %d",packet->data,packet->size);
            }
            if(packet->buf == nullptr) {
                LOGE("packet->buf===========2========== is null");
            } else {
                LOGE("packet->buf===========2========== is not null");
            }

            audio_packet_queue_.Push(packet);
            LOGI("==========%d : %ld %ld",audio_packet_queue_.Size(),packet->pts,packet->dts);
        } else {
            av_packet_free(&packet);
        }
    }

    return;
}
