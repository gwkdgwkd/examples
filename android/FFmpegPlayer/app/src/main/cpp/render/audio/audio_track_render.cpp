#include "audio_track_render.h"

AudioTrackRender::AudioTrackRender(FFmpegAudioDecoder *audio_decoder)
        : audio_decoder_(audio_decoder) {}

void AudioTrackRender::SetPcmCallback(void *context, PcmCallback callback) {
    context_ = context;
    callback_ = callback;
}

void AudioTrackRender::OnPlay() {
    TRACE_FUNC();
    Start();
}

void AudioTrackRender::OnPause() {
    TRACE_FUNC();
    Pause();
}

void AudioTrackRender::OnResume() {
    TRACE_FUNC();
    Resume();
}

void AudioTrackRender::OnStop() {}

void AudioTrackRender::OnSeekTo(float position) {
    TRACE_FUNC();
    LOGE("AudioTrackRender OnSeekTo %f", position);
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

