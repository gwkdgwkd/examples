#include "audio_track_render.h"

AudioTrackRender::AudioTrackRender(FFmpegAudioDecoder *audio_decoder)
        : audio_decoder_(audio_decoder) {}

void AudioTrackRender::SetPcmCallback(void *context, PcmCallback callback) {
    context_ = context;
    callback_ = callback;
}

void AudioTrackRender::Process() {
    AudioFrame *frame = audio_decoder_->GetAudioFrame();
    if (frame && context_ && callback_) {
        callback_(context_, frame->data_, frame->data_size_);
    }
    if (visual_audio_render_) {
        visual_audio_render_->UpdateAudioFrame(frame);
    }

    std::this_thread::sleep_for(std::chrono::nanoseconds(100));
}

