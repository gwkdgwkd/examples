#ifndef FFMPEG_PLAYER_PLAYER_H
#define FFMPEG_PLAYER_PLAYER_H

#include "ffmpeg/ffmpeg_demuxer.h"
#include "ffmpeg/ffmpeg_audio_decoder.h"
#include "ffmpeg/ffmpeg_video_decoder.h"
#include "render/audio/openSlEs_pcm_render.h"
#include "render/video/video_render_interface.h"

class Player {
public:
    Player();
    ~Player();

    bool Init(const char *url, VideoRenderInterface *video_render);
    void Start();

    FFmpegAudioDecoder *GetAudioDecoder() const;
    SLBase *GetAudioRender() const;

    FFmpegAudioDecoder *GetAudioDecoder() {return audio_decoder_;}

private:
    FFmpegDemuxer *demuxer_;
    FFmpegAudioDecoder *audio_decoder_;
    FFmpegVideoDecoder *video_decoder_;
    SLBase *audio_render_;
    VideoRenderInterface *video_render_;
};

#endif // FFMPEG_PLAYER_PLAYER_H
