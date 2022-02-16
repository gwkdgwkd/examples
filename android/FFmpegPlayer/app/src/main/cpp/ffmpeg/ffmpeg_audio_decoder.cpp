#include "ffmpeg_audio_decoder.h"

static const int AUDIO_DST_SAMPLE_RATE = 44100; // 音频编码采样率
static const int AUDIO_DST_CHANNEL_COUNTS = 2; // 音频编码通道数
static const uint64_t AUDIO_DST_CHANNEL_LAYOUT = AV_CH_LAYOUT_STEREO; // 音频编码声道格式
static const AVSampleFormat DST_SAMPLT_FORMAT = AV_SAMPLE_FMT_S16;
static const int ACC_NB_SAMPLES = 1024; // ACC音频一帧采样数

FFmpegAudioDecoder::FFmpegAudioDecoder(FFmpegDemuxer *ffmpeg_demuxer) : ffmpeg_demuxer_(
        ffmpeg_demuxer) {
    TRACE_FUNC();
    swr_ctx_ = nullptr;
    type_ = AVMEDIA_TYPE_AUDIO;
    audio_frame_count_ = 0;
    audio_packet_count_ = 0;
}

FFmpegAudioDecoder::~FFmpegAudioDecoder() {
    TRACE_FUNC();
    if (swr_ctx_) {
        swr_free(&swr_ctx_);
        swr_ctx_ = nullptr;
    }
}

bool FFmpegAudioDecoder::Init() {
    TRACE_FUNC();
    AVCodecContext *codec_ctx = ffmpeg_demuxer_->GetCodecContext(type_);
    int64_t src_ch_layout = codec_ctx->channel_layout;
    int src_rate = codec_ctx->sample_rate;
    enum AVSampleFormat src_sample_fmt = codec_ctx->sample_fmt;
    int src_nb_samples = ACC_NB_SAMPLES;
    int64_t dst_ch_layout = AUDIO_DST_CHANNEL_LAYOUT;
    int dst_rate = AUDIO_DST_SAMPLE_RATE;
    enum AVSampleFormat dst_sample_fmt = DST_SAMPLT_FORMAT;

//    LOGI("src: sample rate: %d, format: %d, layout: %lld", src_rate,
//         src_sample_fmt, src_ch_layout);
//    LOGI("dst: sample rate: %d, format: %d, layout: %lld", dst_rate,
//         dst_sample_fmt, dst_ch_layout);

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
    if (swr_init(swr_ctx_) < 0) {
        LOGE("Failed to initialize the resampling context");
        return ret;
    }

    dst_nb_samples_ = (int) av_rescale_rnd(src_nb_samples, dst_rate, src_rate, AV_ROUND_UP);
    dst_frame_data_size_ = av_samples_get_buffer_size(NULL, av_get_channel_layout_nb_channels(
            dst_ch_layout), dst_nb_samples_, dst_sample_fmt, 1);

    return true;
}

void FFmpegAudioDecoder::OnControlEvent(ControlType type) {
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

void FFmpegAudioDecoder::OnSeekTo(float position) {
    TRACE_FUNC();
    LOGE("FFmpegAudioDecoder OnSeekTo %f", position);

    LOGE("audio_frame_queue_ before flush count %d", audio_frame_queue_.Size());
    avcodec_flush_buffers(ffmpeg_demuxer_->GetCodecContext(type_));
    audio_frame_queue_.flush([](AudioFrame *frame) { delete frame; });
    LOGE("audio_frame_queue_ after flush count %d", audio_frame_queue_.Size());
    clock_ = position;
}

void FFmpegAudioDecoder::Process() {
    // TRACE_FUNC();

    AVPacket *pkt = av_packet_alloc();
    pkt = ffmpeg_demuxer_->GetPacket(type_);
    if (pkt == nullptr) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        return;
    }
    LOGE("audio packet n:%d size:%d pts:%s\n",
         audio_packet_count_++, pkt->size,
         av_ts2timestr(pkt->pts, &ffmpeg_demuxer_->GetCodecContext(type_)->time_base));
    if (!DecodePacket(ffmpeg_demuxer_->GetCodecContext(type_), pkt) == 0) {
        LOGE("decode packet failed!");
    }

    return;
}

int FFmpegAudioDecoder::OutputFrame(AVFrame *frame) {
//    LOGE("audio frame n:%d nb_samples:%d pts:%s\n",
//         audio_frame_count_++, frame->nb_samples,
//         av_ts2timestr(frame->pts, &ffmpeg_demuxer_->GetCodecContext(type_)->time_base));

    AudioFrame *audio_frame = new AudioFrame(dst_frame_data_size_);
    if (swr_convert(swr_ctx_, &(audio_frame->data_), dst_frame_data_size_ / 2,
                    (const uint8_t **) frame->data, frame->nb_samples) > 0) {
        audio_frame->clock_ =
                frame->pts * av_q2d(ffmpeg_demuxer_->GetCodecContext(type_)->time_base);
        audio_frame_queue_.Push(audio_frame);
        av_frame_unref(frame_);
    } else {
        LOGE("swr_convert failed!");
    }

    return 0;
}

AudioFrame *FFmpegAudioDecoder::GetAudioFrame() {
    while (audio_frame_queue_.Empty()) {
        std::this_thread::sleep_for(std::chrono::microseconds(10));
    }

    AudioFrame *ret = audio_frame_queue_.Pop();
    clock_ = ret->clock_;

    return ret;
}

