#include "visual_audio_render.h"

#include <gtc/matrix_transform.hpp>
#include "log.h"

#define MAX_AUDIO_LEVEL 5000
#define RESAMPLE_LEVEL  40

static const char *kVertexShader = R"(
#version 300 es
layout(location = 0) in vec4 a_position;
layout(location = 1) in vec2 a_texCoord;
uniform mat4 u_MVPMatrix;
out vec2 v_texCoord;
void main(void) {
    gl_Position = u_MVPMatrix * a_position;
    v_texCoord = a_texCoord;
    gl_PointSize = 4.0f;
})";

static const char *kFragmentShader = R"(
#version 300 es
precision mediump float;
in vec2 v_texCoord;
layout(location = 0) out vec4 outColor;
uniform float drawType;
void main() {
  if(drawType == 1.0) {
      outColor = vec4(1.5 - v_texCoord.y, 0.3, 0.3, 1.0);
  } else if(drawType == 2.0) {
      outColor = vec4(1.0, 1.0, 1.0, 1.0);
  } else if(drawType == 3.0) {
      outColor = vec4(0.3, 0.3, 0.3, 1.0);
  }
})";

VisualAudioRender::VisualAudioRender() {
    TRACE_FUNC();

    m_VaoId = GL_NONE;

    m_pTextureCoords = nullptr;
    m_pVerticesCoords = nullptr;

    memset(m_VboIds, 0, sizeof(GLuint) * 2);
}

VisualAudioRender::~VisualAudioRender() {
    TRACE_FUNC();
}

void VisualAudioRender::OnSurfaceCreated() {
    TRACE_FUNC();
    wapped_shader_program_ptr_ = new WappedShaderProgram();
    if (!wapped_shader_program_ptr_->Init(kVertexShader, kFragmentShader)) {
        LOGE("shader program init failed!");
        return;
    }
    m_ProgramObj = wapped_shader_program_ptr_->GetProgram();

    //设置 MVP Matrix
    // Projection matrix
    glm::mat4 Projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, 0.1f, 100.0f);
    //glm::mat4 Projection = glm::frustum(-ratio, ratio, -1.0f, 1.0f, 4.0f, 100.0f);
    //glm::mat4 Projection = glm::perspective(45.0f, ratio, 0.1f, 100.f);

    // View matrix
    glm::mat4 View = glm::lookAt(
            glm::vec3(0, 0, 4), // Camera is at (0,0,1), in World Space
            glm::vec3(0, 0, 0), // and looks at the origin
            glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
    );

    // Model matrix
    glm::mat4 Model = glm::mat4(1.0f);
    Model = glm::scale(Model, glm::vec3(1.0f, 1.0f, 1.0f));
    Model = glm::rotate(Model, 0.0f, glm::vec3(1.0f, 0.0f, 0.0f));
    Model = glm::rotate(Model, 0.0f, glm::vec3(0.0f, 1.0f, 0.0f));
    Model = glm::translate(Model, glm::vec3(0.0f, 0.0f, 0.0f));

    m_MVPMatrix = Projection * View * Model;
}

void VisualAudioRender::OnSurfaceChanged(int w, int h) {
    TRACE_FUNC();
    glViewport(0, 0, w, h);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
}

void VisualAudioRender::OnDrawFrame() {
    // TRACE_FUNC();

    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    std::unique_lock <std::mutex> lock(m_Mutex);
    if (m_ProgramObj == GL_NONE || m_pAudioBuffer == nullptr) return;
    UpdateMesh();
    lock.unlock();

    // Generate VBO Ids and load the VBOs with data
    if (m_VboIds[0] == 0) {
        glGenBuffers(2, m_VboIds);

        glBindBuffer(GL_ARRAY_BUFFER, m_VboIds[0]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * m_RenderDataSize * 6 * 3, m_pVerticesCoords,
                     GL_DYNAMIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, m_VboIds[1]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * m_RenderDataSize * 6 * 2, m_pTextureCoords,
                     GL_DYNAMIC_DRAW);
    } else {
        glBindBuffer(GL_ARRAY_BUFFER, m_VboIds[0]);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GLfloat) * m_RenderDataSize * 6 * 3,
                        m_pVerticesCoords);

        glBindBuffer(GL_ARRAY_BUFFER, m_VboIds[1]);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GLfloat) * m_RenderDataSize * 6 * 2,
                        m_pTextureCoords);
    }

    if (m_VaoId == GL_NONE) {
        glGenVertexArrays(1, &m_VaoId);
        glBindVertexArray(m_VaoId);

        glBindBuffer(GL_ARRAY_BUFFER, m_VboIds[0]);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (const void *) 0);
        glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);

        glBindBuffer(GL_ARRAY_BUFFER, m_VboIds[1]);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (const void *) 0);
        glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);

        glBindVertexArray(GL_NONE);
    }

    // Use the program object
    glUseProgram(m_ProgramObj);
    glBindVertexArray(m_VaoId);
    wapped_shader_program_ptr_->SetMat4("u_MVPMatrix", m_MVPMatrix);
    wapped_shader_program_ptr_->SetFloat("drawType", 1.0f);
    glDrawArrays(GL_TRIANGLES, 0, m_RenderDataSize * 6);
    wapped_shader_program_ptr_->SetFloat("drawType", 2.0f);
    glDrawArrays(GL_LINES, 0, m_RenderDataSize * 6);
}

void VisualAudioRender::UpdateMesh() {
    // TRACE_FUNC();
    float dy = 0.25f / MAX_AUDIO_LEVEL;
    float dx = 1.0f / m_RenderDataSize;
    for (int i = 0; i < m_RenderDataSize; ++i) {
        int index = i * RESAMPLE_LEVEL;
        short *pValue = (short *)(m_pAudioBuffer->data_ + index);
        float y = *pValue * dy;
        y = y < 0 ? y : -y;
        glm::vec2 p1(i * dx, 0 + 1.0f);
        glm::vec2 p2(i * dx, y + 1.0f);
        glm::vec2 p3((i + 1) * dx, y + 1.0f);
        glm::vec2 p4((i + 1) * dx, 0 + 1.0f);

        m_pTextureCoords[i * 6 + 0] = p1;
        m_pTextureCoords[i * 6 + 1] = p2;
        m_pTextureCoords[i * 6 + 2] = p4;
        m_pTextureCoords[i * 6 + 3] = p4;
        m_pTextureCoords[i * 6 + 4] = p2;
        m_pTextureCoords[i * 6 + 5] = p3;

        m_pVerticesCoords[i * 6 + 0] = wapped_shader_program_ptr_->TexCoordToVertexCoord(p1);
        m_pVerticesCoords[i * 6 + 1] = wapped_shader_program_ptr_->TexCoordToVertexCoord(p2);
        m_pVerticesCoords[i * 6 + 2] = wapped_shader_program_ptr_->TexCoordToVertexCoord(p4);
        m_pVerticesCoords[i * 6 + 3] = wapped_shader_program_ptr_->TexCoordToVertexCoord(p4);
        m_pVerticesCoords[i * 6 + 4] = wapped_shader_program_ptr_->TexCoordToVertexCoord(p2);
        m_pVerticesCoords[i * 6 + 5] = wapped_shader_program_ptr_->TexCoordToVertexCoord(p3);
    }
}

void VisualAudioRender::UpdateAudioFrame(AudioFrame *audioFrame) {
    TRACE_FUNC();
    if(audioFrame != nullptr) {
        // LOGE("VisualAudioRender::UpdateAudioFrame audioFrame->dataSize=%d", audioFrame->data_size_);
        std::unique_lock<std::mutex> lock(m_Mutex);
        if(m_pAudioBuffer != nullptr && m_pAudioBuffer->data_size_ != audioFrame->data_size_) {
            delete m_pAudioBuffer;
            m_pAudioBuffer = nullptr;

            delete [] m_pTextureCoords;
            m_pTextureCoords = nullptr;

            delete [] m_pVerticesCoords;
            m_pVerticesCoords = nullptr;
        }

        if(m_pAudioBuffer == nullptr) {

            m_pAudioBuffer = new AudioFrame(audioFrame->data_size_);
            m_RenderDataSize = m_pAudioBuffer->data_size_ / RESAMPLE_LEVEL;

            m_pVerticesCoords = new glm::vec3[m_RenderDataSize * 6]; //(x,y,z) * 6 points
            m_pTextureCoords = new glm::vec2[m_RenderDataSize * 6]; //(x,y) * 6 points
        } else {
            memcpy(m_pAudioBuffer->data_, audioFrame->data_, audioFrame->data_size_);
        }
        lock.unlock();
    }
}