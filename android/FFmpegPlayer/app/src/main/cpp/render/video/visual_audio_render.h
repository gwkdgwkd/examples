#ifndef RENDER_VIDEO_VISUAL_AUDIO_RENDER_H
#define RENDER_VIDEO_VISUAL_AUDIO_RENDER_H

#include <jni.h>
#include <mutex>

#include "wapped_shader_program.h"
#include "opengles_render_interface.h"
#include "audio_frame.h"

class VisualAudioRender : public OpenglesRenderInterface {
public:
    VisualAudioRender();
    virtual ~VisualAudioRender();

    virtual void OnSurfaceCreated();
    virtual void OnSurfaceChanged(int w, int h);
    virtual void OnDrawFrame();
    virtual void UpdateMVPMatrix(int angleX, int angleY, float scaleX, float scaleY) {}
    virtual void SetTouchLoc(float touchX, float touchY) {}
    void UpdateAudioFrame(AudioFrame *audioFrame);

private:
    void UpdateMesh();

    WappedShaderProgram *wapped_shader_program_ptr_;
    std::mutex m_Mutex;
    AudioFrame *m_pAudioBuffer = nullptr;

    GLuint m_ProgramObj = 0;
    GLuint m_VaoId;
    GLuint m_VboIds[2];
    glm::mat4 m_MVPMatrix;
    glm::vec3 *m_pVerticesCoords = nullptr;
    glm::vec2 *m_pTextureCoords = nullptr;

    int m_RenderDataSize;
};

#endif // RENDER_VIDEO_VISUAL_AUDIO_RENDER_H
