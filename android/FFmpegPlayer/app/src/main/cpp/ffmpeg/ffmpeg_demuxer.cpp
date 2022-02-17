#include "ffmpeg_demuxer.h"

#include <functional>

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


    return true;
}

AVCodecContext *FFmpegDemuxer::GetCodecContext(enum AVMediaType type) {
    if (type == AVMEDIA_TYPE_VIDEO) {
        return video_dec_ctx_;
    } else {
        return audio_dec_ctx_;
    }
}

AVStream *FFmpegDemuxer::GetAVStream(enum AVMediaType type) {
    if (type == AVMEDIA_TYPE_VIDEO) {
        return video_stream_;
    } else {
        return audio_stream_;
    }
}

AVPacket *FFmpegDemuxer::GetPacket(enum AVMediaType type) {
    if (type == AVMEDIA_TYPE_VIDEO) {
        while (video_packet_queue_.Empty()) {
            std::this_thread::sleep_for(std::chrono::microseconds(10));
        }
        return video_packet_queue_.Pop();
    } else {
        while (audio_packet_queue_.Empty()) {
            std::this_thread::sleep_for(std::chrono::microseconds(10));
        }
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

void FFmpegDemuxer::OnControlEvent(ControlType type) {
    LOGE("play control type %d", type);
    switch (type) {
        case ControlType::kPlay:
            Start();
            break;
        case ControlType::kStop:
            break;
        case ControlType::kPause:
            Pause();
            break;
        case ControlType::kResume:
            Resume();
            break;
        default:
            LOGE("unknown control type");
            break;
    }
}

void FFmpegDemuxer::OnSeekTo(float position) {
    TRACE_FUNC();
    LOGE("FFmpegDemuxer OnSeekTo %f", position);

    std::function<void(AVPacket *)> func = [](AVPacket *pkt) {
        av_packet_free(&pkt);
    };
    int64_t position_b = position * AV_TIME_BASE;

    if (video_stream_idx_ >= 0) {
        LOGE("video_stream_ time_base %lf, {%d,%d}", av_q2d(video_stream_->time_base),
             video_stream_->time_base.num, video_stream_->time_base.den);
        // position_b * AV_TIME_BASE_Q / video_stream_->time_base
        int64_t seek_target = av_rescale_q(position_b, AV_TIME_BASE_Q, video_stream_->time_base);
        LOGE("video seek target is %ld", seek_target);
        if (av_seek_frame(fmt_ctx_, video_stream_idx_, seek_target, AVSEEK_FLAG_BACKWARD) >= 0) {
            LOGE("video_packet_queue_ before flush count %d", video_packet_queue_.Size());
            video_packet_queue_.flush(func);
            LOGE("video_packet_queue_ after flush count %d", video_packet_queue_.Size());
        } else {
            LOGE("error while video seeking");
        }
    }

    if (audio_stream_idx_ >= 0) {
        LOGE("audio_stream_ time_base %lf, {%d,%d}", av_q2d(audio_stream_->time_base),
             audio_stream_->time_base.num, audio_stream_->time_base.den);
        int64_t seek_target = av_rescale_q(position_b, AV_TIME_BASE_Q, audio_stream_->time_base);
        LOGE("audio seek target is %ld", seek_target);
        if (av_seek_frame(fmt_ctx_, audio_stream_idx_, seek_target, AVSEEK_FLAG_BACKWARD) >= 0) {
            LOGE("audio_packet_queue_ before flush count %d", audio_packet_queue_.Size());
            audio_packet_queue_.flush(func);
            LOGE("audio_packet_queue_ after flush count %d", audio_packet_queue_.Size());
        } else {
            LOGE("error while audio seeking");
        }
    }
}

void FFmpegDemuxer::Process() {
    // TRACE_FUNC();

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
            video_packet_queue_.Push(packet);
            LOGE("read video packet n : %d, size %d, pts %d time %s", video_packet_count_++, packet->size,
                 packet->pts, av_ts2timestr(packet->pts, &video_dec_ctx_->time_base));
        } else if (packet->stream_index == audio_stream_idx_) {
            audio_packet_queue_.Push(packet);
            LOGE("read audio packet n : %d, size %d, pts %d time %s", audio_packet_count_++, packet->size,
                 packet->pts, av_ts2timestr(packet->pts, &audio_dec_ctx_->time_base));
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

void FFmpegDemuxer::GetMediaInfo(MediaInfo *info) const {
    info->sample_fmt = audio_dec_ctx_->sample_fmt;
    info->channels = audio_dec_ctx_->channels;
    info->sample_rate = audio_dec_ctx_->sample_rate;
    info->width = video_dec_ctx_->width;
    info->height = video_dec_ctx_->height;
    info->duration = (fmt_ctx_->duration / AV_TIME_BASE * 1000) * 1.0f / 1000;

    return;
}
