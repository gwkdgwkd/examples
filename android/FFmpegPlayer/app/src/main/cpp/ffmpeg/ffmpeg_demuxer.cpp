#include "ffmpeg_demuxer.h"
#include "log.h"

extern int packet_queue_put(PacketQueue *q, AVPacket *pkt);

extern int packet_queue_get(PacketQueue *q, AVPacket *pkt, int block, int *serial);

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
    video_packet_count_ = 0;
    audio_packet_count_ = 0;
    is_demuxer_finish_ = false;
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

    LOGE("audio_stream_idx_ : %d", audio_stream_idx_);
    LOGE("video_stream_idx_ : %d", video_stream_idx_);

    if (!video_stream_ && !audio_stream_) {
        LOGE("Could not find audio or video stream in the input, aborting");
        return ret;
    }

    // dump input information to stderr
    av_dump_format(fmt_ctx_, 0, url_.c_str(), 0);

//    if (packet_queue_init(&video_packet_queue_)) {
//        LOGE("video_packet_queue init failed!");
//        return ret;
//    }
//    if (packet_queue_init(&audio_packet_queue_)) {
//        LOGE("audio_packet_queue init failed!");
//        return ret;
//    }

    return true;
}

AVCodecContext *FFmpegDemuxer::GetCodecContext(enum AVMediaType type) {
    if (type == AVMEDIA_TYPE_VIDEO) {
        return video_dec_ctx_;
    } else {
        return audio_dec_ctx_;
    }
}

AVStream *FFmpegDemuxer::GetAVStream(enum AVMediaType type){
    if (type == AVMEDIA_TYPE_VIDEO) {
        return video_stream_;
    } else {
        return audio_stream_;
    }
}

AVPacket *FFmpegDemuxer::GetPacket(enum AVMediaType type) {
//int FFmpegDemuxer::GetPacket(enum AVMediaType type, AVPacket *pkt) {
    if (type == AVMEDIA_TYPE_VIDEO) {
        return video_packet_queue_.Pop();
//return 0;
    } else {
//        return packet_queue_get(&audio_packet_queue_, pkt, 1, nullptr);
//        LOGE("==================== size %d", audio_packet_queue_.Size());
        return audio_packet_queue_.Pop();
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
//TRACE_FUNC();
    AVPacket *packet = av_packet_alloc();
    if (!packet) {
        LOGE("Could not allocate packet");
        return;
    }

    int ret = av_read_frame(fmt_ctx_, packet);
    // read frames from the file
    if (ret >= 0) {
        // check if the packet belongs to a stream we are interested in, otherwise skip it
        if (packet->stream_index == video_stream_idx_) {
//            packet_queue_put(&video_packet_queue_, packet);
            video_packet_queue_.Push(packet);
        } else if (packet->stream_index == audio_stream_idx_) {
//            PrintPackInfo(packet);
            audio_packet_queue_.Push(packet);
//            LOGE("read audio packet n : %d, size %d, pts %s", audio_packet_count_++, packet->size,
//                 av_ts2timestr(packet->pts, &audio_dec_ctx_->time_base));
//            packet_queue_put(&audio_packet_queue_, packet);
//            PrintQueueInfo();
        } else {
            av_packet_free(&packet);
        }
    } else if (AVERROR_EOF == ret) {
        LOGE("read audio packet finish, get AVERROR_EOF, Pause thread");
        is_demuxer_finish_ = true;
        Pause();
    }

    return;
}

void FFmpegDemuxer::PrintPackInfo(AVPacket *pkt) {
    TRACE_FUNC();
    LOGE("size: %d", pkt->size);
    LOGE("duration: %ld", pkt->duration);
    LOGE("pts: %ld", pkt->pts);
    LOGE("dts: %ld", pkt->dts);
    LOGE("stream_index: %d", pkt->stream_index);
    LOGE("pos: %ld", pkt->pos);
}

void FFmpegDemuxer::PrintQueueInfo() {
    TRACE_FUNC();
//    LOGE("size: %d", audio_packet_queue_.size);
//    LOGE("duration: %ld", audio_packet_queue_.duration);
//    LOGE("nb_packets: %d", audio_packet_queue_.nb_packets);
}
