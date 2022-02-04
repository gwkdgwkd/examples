
#ifndef LEARNFFMPEG_VIDEORENDER_H
#define LEARNFFMPEG_VIDEORENDER_H

#include "native_image.h"
#include "thread_base.h"
#include "../ffmpeg/ffmpeg_video_decoder.h"
#include "log.h"

#define VIDEO_RENDER_OPENGL             0
#define VIDEO_RENDER_ANWINDOW           1
#define VIDEO_RENDER_3D_VR              2

class FFmpegVideoDecoder;
class VideoRenderInterface : public ThreadBase{
public:
    VideoRenderInterface(int type){
        m_RenderType = type;
    }
    virtual ~VideoRenderInterface(){}
    virtual void Init(int video_width, int video_height, int *real_size, FFmpegVideoDecoder *video_decoder) = 0;
    virtual void RenderVideoFrame(NativeImage *imageptr) = 0;
    virtual void UnInit() = 0;

    int GetRenderType() {
        return m_RenderType;
    }

    FFmpegVideoDecoder *video_decoder_;
private:
    int m_RenderType = VIDEO_RENDER_ANWINDOW;
};

class VideoRenderFactoryInterface {
public:
    virtual VideoRenderInterface* CreateVideoRender() = 0;
    virtual ~VideoRenderFactoryInterface() {}
};

#endif //LEARNFFMPEG_VIDEORENDER_H
