#ifndef RENDER_VIDEO_NATIVE_WINDOW_RENDER_H
#define RENDER_VIDEO_NATIVE_WINDOW_RENDER_H

#include <android/native_window.h>
#include <android/native_window_jni.h>
#include <jni.h>

#include "video_render_interface.h"
#include "../ffmpeg/ffmpeg_audio_decoder.h"

class NativeWindowRender : public VideoRenderInterface {

public:
    NativeWindowRender(JNIEnv *env, jobject surface, enum VideoRenderType type);
    virtual ~NativeWindowRender();

    virtual void Init(int videoWidth, int videoHeight, int *dstSize, FFmpegVideoDecoder *video_decoder);
    virtual void RenderVideoFrame(NativeImage *pImage);
    virtual void UnInit();

private:
    virtual void Process() override;

    ANativeWindow_Buffer native_window_buffer_;
    ANativeWindow *native_window_ = nullptr;

    int real_width_;
    int real_height_;
};

//class NativeWindowRenderFactory : public VideoRenderFactoryInterface {
//    virtual VideoRenderInterface* CreateVideoRender() { return new NativeWindowRender();}
//};

#endif // RENDER_VIDEO_NATIVE_WINDOW_RENDER_H
