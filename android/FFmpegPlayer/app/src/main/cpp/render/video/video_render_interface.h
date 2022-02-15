
#ifndef LEARNFFMPEG_VIDEORENDER_H
#define LEARNFFMPEG_VIDEORENDER_H

#include "native_image.h"
#include "thread_base.h"
#include "ffmpeg_video_decoder.h"
#include "ffmpeg_audio_decoder.h"
#include "log.h"

typedef void (*MessageCallback)(void*, int, float);

class FFmpegVideoDecoder;
class VideoRenderInterface : public ThreadBase{
public:
    enum ViewType {
        kSurfaceView,
        kGLSurfaceView
    };
    enum VideoRenderType {
        kAnWindow,
        kOpengles
    };
    enum EffectType {
        kNoEffect,
        kDynimicMesh,
        kGrayImage,
        k3DVR,
        kVisualAudio
    };

    VideoRenderInterface(enum ViewType view_type, enum VideoRenderType video_render_type, enum EffectType effect_type){
        view_type_ = view_type;
        video_render_type_ = video_render_type;
        effect_type_ = effect_type;
    }
    virtual ~VideoRenderInterface() {}
    virtual void Init(int video_width, int video_height, int *render_size) = 0;
    virtual void RenderVideoFrame(NativeImage *imageptr) = 0;
    virtual void UnInit() = 0;
    virtual void SetAudioDecoder(FFmpegAudioDecoder *audio_decoder) { audio_decoder_ = audio_decoder; }
    virtual void SetVideoDecoder(FFmpegVideoDecoder *video_decoder) { video_decoder_ = video_decoder; }
    int GetViedoRenderType() {
        return video_render_type_;
    }
    int GetViewType() {
        return view_type_;
    }
    int GetEffectType() {
        return effect_type_;
    }

    void SetMessageCallback(void* context, MessageCallback callback) {
        m_MsgContext = context;
        m_MsgCallback = callback;
    }
    void * m_MsgContext = nullptr;
    MessageCallback m_MsgCallback = nullptr;

protected:
    FFmpegVideoDecoder *video_decoder_;
    FFmpegAudioDecoder *audio_decoder_;
    long frame_index_;

    ViewType view_type_;
    VideoRenderType video_render_type_;
    EffectType effect_type_;
};

class VideoRenderFactoryInterface {
public:
    virtual VideoRenderInterface* CreateVideoRender() = 0;
    virtual ~VideoRenderFactoryInterface() {}
};

#endif //LEARNFFMPEG_VIDEORENDER_H
