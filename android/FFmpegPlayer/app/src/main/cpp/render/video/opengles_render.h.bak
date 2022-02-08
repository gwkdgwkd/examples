#ifndef RENDER_VIDEO_OPENGL_RENDER_H
#define RENDER_VIDEO_OPENGL_RENDER_H

#include <android/native_window.h>
#include <android/native_window_jni.h>
#include <jni.h>

#include "video_render_interface.h"
#include "egl_core.h"
#include "wapped_shader_program.h"
#include "wapped_texture.h"
#include "opengles_render_interface.h"

class OpenGLESRender : public VideoRenderInterface, public OpenglesRenderInterface {

public:
    enum EffectType {
        kNoEffect,
        kDynimicMesh,
        kGrayImage
    };
    OpenGLESRender(JNIEnv *env, jobject surface);
    virtual ~OpenGLESRender();

    virtual void Init(int videoWidth, int videoHeight, int *dstSize, FFmpegVideoDecoder *video_decoder);
    virtual void RenderVideoFrame(NativeImage *pImage);
    virtual void UnInit();

    virtual void OnSurfaceCreated() {}
    virtual void OnSurfaceChanged(int w, int h);
    virtual void OnDrawFrame() {}
    virtual void UpdateMVPMatrix(int angleX, int angleY, float scaleX, float scaleY);
    virtual void SetTouchLoc(float touchX, float touchY);

private:
    virtual void Process() override;
    bool OpenglesInit();
    bool VaoInit();
    void DrawFrame();

    ANativeWindow *native_window_ = nullptr;
    EGLCore *egl_core_ptr_;
    WappedShaderProgram *wapped_shader_program_ptr_;
    WappedTexture *wapped_texture_ptr_;
    std::atomic_bool is_opengles_init_;
    GLuint vao_id_;
    GLuint vbo_ids_[3];
    EffectType effect_type_;

    glm::mat4 MVPMatrix_;
    glm::vec2 touchxy_;

    int real_width_;
    int real_height_;
};

#endif // RENDER_VIDEO_OPENGL_RENDER_H
