#ifndef FFMPEG_PLAYER_PLAYER_H
#define FFMPEG_PLAYER_PLAYER_H

#include <jni.h>
#include <atomic>
#include <list>
#include <iterator>

#include "play_control_observer_interface.h"

#include "ffmpeg/ffmpeg_demuxer.h"
#include "ffmpeg/ffmpeg_audio_decoder.h"
#include "ffmpeg/ffmpeg_video_decoder.h"

#include "render/audio/openSlEs_pcm_render.h"
#include "render/audio/audio_track_render.h"
#include "render/video/video_render_interface.h"

#include "render/video/native_window_render.h"
#include "render/video/visual_audio_render.h"
#include "render/video/opengles_render.h"

#include "scale/ffmpeg_scale.h"
#include "scale/libyuv_scale.h"
#include "scale/opengl_scale.h"

class Player : public PlayControlObserverInterface {
public:
    enum AudioRenderType {
        kOpensles,
        kAudioTrack1,
        kAudioTrack2
    };
    enum ScaleType {
        kFFmpeg,
        kLibyuv,
        kOpengles
    };

    Player(int view_type, int audio_render_type, int video_render_type ,int effect_type, int scale_type);
    ~Player();

    bool Init(const char *url);
    void Init2(JNIEnv *env, jobject obj, jobject surface);
    void Uninit();

    virtual void OnPlay() override;
    virtual void OnPause() override;
    virtual void OnResume() override;
    virtual void OnStop() override;
    virtual void OnSeekTo(float position) override;

    FFmpegAudioDecoder *GetAudioDecoder() const;
    SLBase *GetAudioRender() const;
    FFmpegAudioDecoder *GetAudioDecoder() { return audio_decoder_; }
    OpenglesRenderInterface *GetOpenglesRender() { return dynamic_cast<OpenglesRenderInterface*>(video_render_); }
    VisualAudioRender *GetVisualAudioRender() { return visual_audio_render_; }
    AudioTrackRender *GetAudioTrackRender() { return audio_track_render_; }
    FFmpegDemuxer *GetDemuxer() { return demuxer_;}
    void SetVisualAudioRender(VisualAudioRender *render);
    bool GetInitStatus() { return is_inited_; }
    int GetVideoRenderType() { return video_render_type_; }
    int GetEffectType() { return effect_type_; }
    static void WritePcm(void *context, uint8_t *pcm, int len);
    static void PostMessage(void *context, int msgType, float msgCode);
    MediaInfo media_info_;
private:
    JNIEnv *GetJNIEnv(bool *is_attach);
    jobject GetJavaObj();
    JavaVM *GetJavaVM();

    FFmpegDemuxer *demuxer_;
    FFmpegAudioDecoder *audio_decoder_;
    FFmpegVideoDecoder *video_decoder_;
    SLBase *audio_render_;
    AudioTrackRender* audio_track_render_;
    VideoRenderInterface *video_render_;
    VisualAudioRender *visual_audio_render_;

    VideoRenderInterface::ViewType view_type_;
    AudioRenderType audio_render_type_;
    VideoRenderInterface::VideoRenderType video_render_type_;
    VideoRenderInterface::EffectType effect_type_;
    ScaleType scale_type_;

    std::list<PlayControlObserverInterface*> list_observer_;

    std::atomic<bool> is_inited_;
    std::atomic<bool> is_playing_;
};

#endif // FFMPEG_PLAYER_PLAYER_H
