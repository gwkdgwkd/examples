#ifndef FFMPEG_PLAYER_PLAYER_H
#define FFMPEG_PLAYER_PLAYER_H

#include <jni.h>

#include "ffmpeg/ffmpeg_demuxer.h"
#include "ffmpeg/ffmpeg_audio_decoder.h"
#include "ffmpeg/ffmpeg_video_decoder.h"
#include "render/audio/openSlEs_pcm_render.h"
#include "render/video/video_render_interface.h"
#include "render/video/native_window_render.h"
#include "render/video/opengles_render.h"

class Player {
public:
    Player();
    ~Player();

    bool
    Init(JNIEnv *env, jobject obj, jobject surface, const char *url);
    void Uninit();
    void Start();
    FFmpegAudioDecoder *GetAudioDecoder() const;
    SLBase *GetAudioRender() const;
    FFmpegAudioDecoder *GetAudioDecoder() { return audio_decoder_; }
    static void PostMessage(void *context, int msgType, float msgCode);

private:
    JNIEnv *GetJNIEnv(bool *is_attach);
    jobject GetJavaObj();
    JavaVM *GetJavaVM();

    FFmpegDemuxer *demuxer_;
    FFmpegAudioDecoder *audio_decoder_;
    FFmpegVideoDecoder *video_decoder_;
    SLBase *audio_render_;
    VideoRenderInterface *video_render_;

//    JavaVM *java_vm_ = nullptr;
//    jobject java_obj_ = nullptr;
};

#endif // FFMPEG_PLAYER_PLAYER_H
