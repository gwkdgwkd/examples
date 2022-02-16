#include "audio_track_render.h"

AudioTrackRender::AudioTrackRender(FFmpegAudioDecoder *audio_decoder)
        : audio_decoder_(audio_decoder) {}

void AudioTrackRender::SetPcmCallback(void *context, PcmCallback callback) {
    context_ = context;
    callback_ = callback;
}

void AudioTrackRender::OnControlEvent(ControlType type) {
    LOGE("play control type %d", type);
    switch(type) {
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

void AudioTrackRender::Process() {
    TRACE_FUNC();
    AudioFrame *frame = audio_decoder_->GetAudioFrame();
    if (frame && context_ && callback_) {
        callback_(context_, frame->data_, frame->data_size_);
    }
    if (visual_audio_render_) {
        visual_audio_render_->UpdateAudioFrame(frame);
    }

    std::this_thread::sleep_for(std::chrono::nanoseconds(100));
}

