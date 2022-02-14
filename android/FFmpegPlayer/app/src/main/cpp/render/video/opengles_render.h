#ifndef RENDER_VIDEO_OPENGL_RENDER_H
#define RENDER_VIDEO_OPENGL_RENDER_H

#include <vector>
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
    OpenGLESRender(JNIEnv *env, jobject surface, enum ViewType view_type,
                   enum VideoRenderType video_render_type, enum EffectType effect_type);

    virtual ~OpenGLESRender();

    virtual void Init(int videoWidth, int videoHeight, int *render_size) override;
    virtual void RenderVideoFrame(NativeImage *pImage) override;
    virtual void UnInit() override;
    virtual void OnSurfaceCreated() override;
    virtual void OnSurfaceChanged(int w, int h) override;
    virtual void OnDrawFrame() override;
    virtual void UpdateMVPMatrix(int angleX, int angleY, float scaleX, float scaleY) override;
    virtual void SetTouchLoc(float touchX, float touchY) override;

private:
    virtual void Process() override;
    bool OpenglesInit();
    bool VaoInit();
    void GenerateMesh();
    bool IsSurface();

    ANativeWindow *native_window_ = nullptr;
    EGLCore *egl_core_ptr_;
    WappedShaderProgram *wapped_shader_program_ptr_;
    WappedTexture *wapped_texture_ptr_;
    std::atomic_bool is_opengles_init_;

    std::vector<glm::vec3> vertex_coords_;
    std::vector<glm::vec2> texture_coords_;

    GLuint vao_id_;
    GLuint vbo_ids_[3];
    int vbo_nums_;

    glm::mat4 MVPMatrix_;
    glm::vec2 touchxy_;

    int real_width_;
    int real_height_;
    int last_process_;
};

#endif // RENDER_VIDEO_OPENGL_RENDER_H
