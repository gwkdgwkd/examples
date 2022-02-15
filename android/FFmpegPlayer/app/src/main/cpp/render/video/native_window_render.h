#ifndef RENDER_VIDEO_NATIVE_WINDOW_RENDER_H
#define RENDER_VIDEO_NATIVE_WINDOW_RENDER_H

#include <android/native_window.h>
#include <android/native_window_jni.h>
#include <jni.h>

#include "play_control_observer_interface.h"
#include "video_render_interface.h"
#include "../ffmpeg/ffmpeg_audio_decoder.h"

class NativeWindowRender : public VideoRenderInterface, public PlayControlObserverInterface {

public:
    NativeWindowRender(JNIEnv *env, jobject surface, enum ViewType view_type,
                       enum VideoRenderType video_render_type, enum EffectType effect_type);

    virtual ~NativeWindowRender();
    virtual void Init(int videoWidth, int videoHeight, int *render_size);
    virtual void RenderVideoFrame(NativeImage *pImage);
    virtual void UnInit();

    virtual void OnPlay() override;
    virtual void OnPause() override;
    virtual void OnResume() override;
    virtual void OnStop() override;
    virtual void OnSeekTo(float position) override;
private:
    virtual void Process() override;

    ANativeWindow_Buffer native_window_buffer_;
    ANativeWindow *native_window_ = nullptr;

    int real_width_;
    int real_height_;
    int last_process_;
};

//class NativeWindowRenderFactory : public VideoRenderFactoryInterface {
//    virtual VideoRenderInterface* CreateVideoRender() { return new NativeWindowRender();}
//};

#endif // RENDER_VIDEO_NATIVE_WINDOW_RENDER_H
