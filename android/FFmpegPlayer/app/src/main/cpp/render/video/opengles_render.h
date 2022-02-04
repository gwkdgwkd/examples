#ifndef RENDER_VIDEO_OPENGL_RENDER_H
#define RENDER_VIDEO_OPENGL_RENDER_H

#include <android/native_window.h>
#include <android/native_window_jni.h>
#include <jni.h>

#include "video_render_interface.h"
#include "ffmpeg_audio_decoder.h"
#include "egl_core.h"
#include "wapped_shader_program.h"
#include "wapped_texture.h"

class OpenGLESRender : public VideoRenderInterface {

public:
    OpenGLESRender(JNIEnv *env, jobject surface);
    virtual ~OpenGLESRender();

    virtual void Init(int videoWidth, int videoHeight, int *dstSize, FFmpegVideoDecoder *video_decoder);
    virtual void RenderVideoFrame(NativeImage *pImage);
    virtual void UnInit();

    void SetAudioDecoder(FFmpegAudioDecoder *audio_decoder) { audio_decoder_ = audio_decoder; }

private:
    virtual void Process() override;
    bool OpenglesInit();
    void DrawFrame();
    void Render();

    ANativeWindow *native_window_ = nullptr;

    FFmpegAudioDecoder *audio_decoder_;

    EGLCore *egl_core_ptr_;
    WappedShaderProgram *wapped_shader_program_ptr_;
    WappedTexture *wapped_texture_ptr_;
    std::atomic_bool is_opengles_init_;

    int real_width_;
    int real_height_;
};

#endif // RENDER_VIDEO_OPENGL_RENDER_H
