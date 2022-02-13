#ifndef FFMPEG_PLAYER_AUDIO_TRACK_RENDER_H
#define FFMPEG_PLAYER_AUDIO_TRACK_RENDER_H

#include "thread_base.h"
#include "ffmpeg_audio_decoder.h"

typedef void (*PcmCallback)(void *context, uint8_t *pcm, int len);

class AudioTrackRender : public ThreadBase {
public:
    AudioTrackRender(FFmpegAudioDecoder *audio_decoder);
    void SetPcmCallback(void *context, PcmCallback callback);

private:
    virtual void Process() override;

    FFmpegAudioDecoder *audio_decoder_;

    void *context_ = nullptr;
    PcmCallback callback_ = nullptr;
};

#endif // FFMPEG_PLAYER_AUDIO_TRACK_RENDER_H
