#ifndef FFMPEG_PLAYER_PLAYER_H
#define FFMPEG_PLAYER_PLAYER_H

#include "ffmpeg/ffmpeg_demuxer.h"
#include "ffmpeg/ffmpep_audio_decorder.h"
#include "render/audio/openSlEs_pcm_render.h"

class Player {
public:
    Player();
    ~Player();

    bool Init(const char *url);
    void Start();

    FFmpegAudioDecorder *GetAudioDecorder() const;
    SLBase *GetAudioRender() const;

private:
    FFmpegDemuxer *demuxer_;
    FFmpegAudioDecorder *audio_decoder_;
    SLBase *audio_render_;
};

#endif // FFMPEG_PLAYER_PLAYER_H
