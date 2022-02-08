
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
    enum VideoRenderType {
        kAnWindow,
        kOpenglesSurface,
        kOpenglesGlSurface,
        k3dVr
    };
    VideoRenderInterface(enum VideoRenderType type){
        render_type_ = type;
    }
    virtual ~VideoRenderInterface() {}
    virtual void Init(int video_width, int video_height, int *real_size, FFmpegVideoDecoder *video_decoder) = 0;
    virtual void RenderVideoFrame(NativeImage *imageptr) = 0;
    virtual void UnInit() = 0;
    virtual void SetAudioDecoder(FFmpegAudioDecoder *audio_decoder) { audio_decoder_ = audio_decoder; }
    int GetRenderType() {
        return render_type_;
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
    VideoRenderType render_type_;

};

class VideoRenderFactoryInterface {
public:
    virtual VideoRenderInterface* CreateVideoRender() = 0;
    virtual ~VideoRenderFactoryInterface() {}
};

#endif //LEARNFFMPEG_VIDEORENDER_H
