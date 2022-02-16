#include "opengles_render.h"

#include <gtc/matrix_transform.hpp>
#include "log.h"

#define AV_SYNC_THRESHOLD_MIN 0.04
#define AV_SYNC_THRESHOLD_MAX 0.1

#define MATH_PI 3.1415926535897932384626433832802
#define ANGLE_SPAN  9
#define UNIT_SIZE   0.5
#define BALL_RADIUS 6.0
#define RADIAN(angle) ((angle) / 180 * MATH_PI)

static const char *kVertexShader = R"(
#version 300 es
layout(location = 0) in vec4 a_position;
layout(location = 1) in vec2 a_texcoord;
uniform mat4 MVPMatrix;
out vec2 v_texcorrd;
void main(void) {
    gl_Position = MVPMatrix * a_position;
    v_texcorrd = a_texcoord;
})";

static const char *kFragmentShader = R"(
#version 300 es
precision mediump float;
in vec2 v_texcorrd;
layout(location = 0) out vec4 out_color;
uniform sampler2D s_texturemap;
uniform sampler2D texture0;
uniform sampler2D texture1;
uniform sampler2D texture2;
uniform int img_type; // 1:RGBA, 2:NV21, 3:NV12, 4:I420
uniform int effect_type; // 0:no, 1:DynimicMesh, 2:GrayImage
uniform float offset;
uniform vec2 tex_size;

vec4 sampleImage(vec2 texcoord) {
    vec4 ret;
    if(img_type == 1) { // RGBA
        ret = texture(texture0, texcoord);
    } else if (img_type == 2) { // NV21
        vec3 yuv;
        yuv.x = texture(texture0, texcoord).r;
        yuv.y = texture(texture1, texcoord).a - 0.5;
        yuv.z = texture(texture1, texcoord).r - 0.5;
        highp vec3 rgb = mat3(1.0,  1.0,     1.0,
                              0.0,  -0.344,  1.770,
                              1.403,-0.714, 0.0) * yuv;
        ret = vec4(rgb, 1.0);
    } else if(img_type == 3) { // NV12
        vec3 yuv;
        yuv.x = texture(texture0, texcoord).r;
        yuv.y = texture(texture1, texcoord).r - 0.5;
        yuv.z = texture(texture1, texcoord).a - 0.5;
        highp vec3 rgb = mat3(1.0,  1.0,     1.0,
                              0.0,  -0.344,  1.770,
                              1.403,-0.714, 0.0) * yuv;
        ret = vec4(rgb, 1.0);
    } else if(img_type == 4) { // I420
        vec3 yuv;
        yuv.x = texture(texture0, texcoord).r;
        yuv.y = texture(texture1, texcoord).r - 0.5;
        yuv.z = texture(texture2, texcoord).r - 0.5;
        highp vec3 rgb = mat3(1.0,  1.0,     1.0,
                              0.0,  -0.344,  1.770,
                              1.403,-0.714, 0.0) * yuv;
       ret = vec4(rgb, 1.0);
    } else {
       ret = vec4(1.0);
    }
    return ret;
}

void main() {
    if(effect_type == 1) {
        vec2 img_texcoord = v_texcorrd * tex_size;
        float side_length = tex_size.y / 6.0;
        float max_offset = 0.08 * side_length;
        float x = mod(img_texcoord.x, floor(side_length));
        float y = mod(img_texcoord.y, floor(side_length));

        float final_offset = offset * max_offset;

        if(final_offset <= x && x <= side_length - final_offset && final_offset <= y && y <= side_length - final_offset) {
            out_color = sampleImage(v_texcorrd);
        }else{
            out_color = vec4(1.0, 1.0, 1.0, 1.0);
        }
    } else if(effect_type == 2) {
        out_color = sampleImage(v_texcorrd);
        if(v_texcorrd.x > 0.5) {
            out_color = vec4(vec3(out_color.r*0.299 + out_color.g*0.587 + out_color.b*0.114), out_color.a);
        }
    } else {
        out_color = sampleImage(v_texcorrd);
    }
})";

/* 渲染坐标系或OpenGLES坐标系:
    (-1.0, 1.0)             ( 1.0, 1.0)
                ( 0.0, 0.0)
    (-1.0,-1.0)             ( 1.0,-1.0)
*/
static GLfloat kVerticesCoords[] = {
        -1.0f, 1.0f, 0.0f,  // Position 0 : left top
        -1.0f, -1.0f, 0.0f,  // Position 1 : left bottom
        1.0f, -1.0f, 0.0f,   // Position 2 : right bottom
        1.0f, 1.0f, 0.0f,   // Position 3 : right top
};

/* 纹理坐标：
    (0,0)       (1,0)

    (0,1)       (1,1)
*/
static GLfloat kTextureCoords[] = {
        0.0f, 0.0f,        // TexCoord 0 : left top
        0.0f, 1.0f,        // TexCoord 1 : left bottom
        1.0f, 1.0f,        // TexCoord 2 : right bottom
        1.0f, 0.0f         // TexCoord 3 : right top

// 旋转180度：
//        1.0f,  1.0f,        // TexCoord 2 : right bottom
//        1.0f,  0.0f,        // TexCoord 3 : right top
//        0.0f,  0.0f,        // TexCoord 0 : left top
//        0.0f,  1.0f         // TexCoord 1 : left bottom
};

GLushort kIndices[] = {0, 1, 2, 0, 2, 3};

OpenGLESRender::OpenGLESRender(JNIEnv *env, jobject surface, enum ViewType view_type,
                               enum VideoRenderType video_render_type, enum EffectType effect_type)
        : VideoRenderInterface(view_type, video_render_type, effect_type) {
    TRACE_FUNC();
    is_opengles_init_ = false;
    frame_index_ = 0;
    last_process_ = 0;
    if (IsSurface()) {
        native_window_ = ANativeWindow_fromSurface(env, surface);
    }
}

OpenGLESRender::~OpenGLESRender() {
    TRACE_FUNC();
    if (IsSurface()) {
        if (native_window_)
            ANativeWindow_release(native_window_);
    }

    vertex_coords_.clear();
    texture_coords_.clear();
}

bool OpenGLESRender::IsSurface() {
    return view_type_ == ViewType::kSurfaceView;
}

void OpenGLESRender::Init(int videoWidth, int videoHeight, int *render_size) {
    TRACE_FUNC();
    if (IsSurface()) {
        LOGI("OpenGLESRender::Init OpenGLESRender=%p, video[w,h]=[%d, %d]", native_window_,
             videoWidth, videoHeight);
        if (native_window_ == nullptr) return;

        int windowWidth = ANativeWindow_getWidth(native_window_);
        int windowHeight = ANativeWindow_getHeight(native_window_);

        if (windowWidth < windowHeight * videoWidth / videoHeight) {
            real_width_ = windowWidth;
            real_height_ = windowWidth * videoHeight / videoWidth;
        } else {
            real_width_ = windowHeight * videoWidth / videoHeight;
            real_height_ = windowHeight;
        }
        LOGI("OpenGLESRender::Init window[w,h]=[%d, %d],DstSize[w, h]=[%d, %d]", windowWidth,
             windowHeight, real_width_, real_height_);
    } else {
        real_width_ = 1080;
        real_height_ = 606;
    }
    render_size[0] = real_width_;
    render_size[1] = real_height_;

    if (IsSurface()) {
        Start();
    } else {
        OpenglesInit();
    }
}

void OpenGLESRender::UnInit() {

}

bool OpenGLESRender::VaoInit() {
    TRACE_FUNC();

    // Generate VBO Ids and load the VBOs with data
    glGenBuffers(vbo_nums_, vbo_ids_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_ids_[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * vertex_coords_.size(), &vertex_coords_[0],
                 GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, vbo_ids_[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * texture_coords_.size(), &texture_coords_[0],
                 GL_STATIC_DRAW);

    if (vbo_nums_ > 2) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_ids_[2]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(kIndices), kIndices, GL_STATIC_DRAW);
    }

    // Generate VAO Id
    glGenVertexArrays(1, &vao_id_);
    glBindVertexArray(vao_id_);

    glBindBuffer(GL_ARRAY_BUFFER, vbo_ids_[0]);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (const void *) 0);
    glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);

    glBindBuffer(GL_ARRAY_BUFFER, vbo_ids_[1]);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (const void *) 0);
    glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);

    if (vbo_nums_ > 2) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_ids_[2]);
    }

    glBindVertexArray(GL_NONE);

    return true;
}

bool OpenGLESRender::OpenglesInit() {
    TRACE_FUNC();
    bool ret = false;

    if (IsSurface()) {
        egl_core_ptr_ = new EGLCore();
        if ((ret = egl_core_ptr_->Init(native_window_)) == false) {
            LOGE("egl core init failed!");
            return ret;
        }
    }

    wapped_shader_program_ptr_ = new WappedShaderProgram();
    if (!wapped_shader_program_ptr_->Init(kVertexShader, kFragmentShader)) {
        LOGE("shader program init failed!");
        return ret;
    }

    wapped_texture_ptr_ = new WappedTexture(3, wapped_shader_program_ptr_);
    if ((ret = wapped_texture_ptr_->CreateTexture()) == false) {
        LOGE("create texture failed!");
        return ret;
    }

    GenerateMesh();

    if (effect_type_ != EffectType::k3DVR) {
        vbo_nums_ = 3;
    } else {
        vbo_nums_ = 2;
    }
    LOGE("vbo_nums_ is %d", vbo_nums_);
    VaoInit();

    if (effect_type_ != EffectType::k3DVR) {
        UpdateMVPMatrix(0, 0, 1.0f, 1.0f);
    }

    is_opengles_init_ = true;

    if (IsSurface()) {
        Pause();
    }

    return true;
}

void OpenGLESRender::RenderVideoFrame(NativeImage *pImage) {
    TRACE_FUNC();
}

void OpenGLESRender::OnControlEvent(ControlType type) {
    LOGE("play control type %d", type);
    switch(type) {
        case ControlType::kPlay:
            Start();
            break;
        case ControlType::kStop:
            break;
        case ControlType::kPause:
            Pause();
            break;
        case ControlType::kResume:
            Resume();
            break;
        default:
            LOGE("unknown control type");
            break;
    }
}

void OpenGLESRender::OnSeekTo(float position) {
    TRACE_FUNC();
    last_process_ = -1;
}

void OpenGLESRender::Process() {
    TRACE_FUNC();

    if (IsSurface()) {
        if (!is_opengles_init_) {
            OpenglesInit();
            return;
        }
        OnDrawFrame();
    } else {
        if (m_MsgContext && m_MsgCallback)
            m_MsgCallback(m_MsgContext, 0, 0);
        Pause();
    }
}

void OpenGLESRender::OnSurfaceCreated() {
}

void OpenGLESRender::OnSurfaceChanged(int w, int h) {
    TRACE_FUNC();

    // for surface view. maybe before init
    while (!is_opengles_init_) {
        std::this_thread::sleep_for(std::chrono::microseconds(10));
    }
    real_width_ = w;
    real_height_ = h;
    glViewport(0, 0, w, h);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    if (effect_type_ == EffectType::k3DVR) {
        UpdateMVPMatrix(0, 0, 1.0f, 1.0f);
    }
}

void OpenGLESRender::OnDrawFrame() {
    TRACE_FUNC();
    // LOGI("left %d, right %d, width %d, height %d", 0, 0, real_width_, real_height_);

    NativeImage *pImage = video_decoder_->GetVideoImage();
    if (pImage == nullptr) return;
    double delay = pImage->delay;
    // 如果有音频的话
    double audio_clock = audio_decoder_->GetClock();
    double diff = pImage->clock - audio_clock;
    // delay < 0.04, 同步阀值为0.04
    // delay > 0.1, 同步阀值为0.1
    // 0.04 < delay < 0.1, 同步阀值是delay
    double sync = FFMAX(AV_SYNC_THRESHOLD_MIN, FFMIN(AV_SYNC_THRESHOLD_MAX, pImage->delay));
    if (diff <= -sync) { // 视频落后来，让delay时间减小
        delay = FFMAX(0, delay + diff);
    } else if (diff > sync) { // 视频快了，让delay久一些，等待音频赶上来
        delay = delay + diff;
    }
    long real_delay = delay * 1000000;
    LOGE("Video:%lf Audio:%lf delay:%lf(%ld) A-V=%lf", pImage->clock, audio_clock, delay,
         real_delay, -diff);

    int process = pImage->clock + delay;
    std::this_thread::sleep_for(std::chrono::microseconds(real_delay));

    int img_type = pImage->format;
    switch (pImage->format) {
        case IMAGE_FORMAT_RGBA:
            wapped_texture_ptr_->UpdateTexImage(0, WappedTexture::kTypeRGBA,
                                                (unsigned char *) pImage->ppPlane[0], pImage->width,
                                                pImage->height);
            break;
        case IMAGE_FORMAT_NV21:
        case IMAGE_FORMAT_NV12:
            wapped_texture_ptr_->UpdateTexImage(0, WappedTexture::kTypeLUMINANCE,
                                                (unsigned char *) pImage->ppPlane[0], pImage->width,
                                                pImage->height);
            wapped_texture_ptr_->UpdateTexImage(1, WappedTexture::kTypeLUMINANCEALPHA,
                                                (unsigned char *) pImage->ppPlane[1],
                                                pImage->width >> 1,
                                                pImage->height >> 1);
            break;
        case IMAGE_FORMAT_I420:
            wapped_texture_ptr_->UpdateTexImage(0, WappedTexture::kTypeLUMINANCE,
                                                (unsigned char *) pImage->ppPlane[0], pImage->width,
                                                pImage->height);
            wapped_texture_ptr_->UpdateTexImage(1, WappedTexture::kTypeLUMINANCE,
                                                (unsigned char *) pImage->ppPlane[1],
                                                pImage->width >> 1,
                                                pImage->height >> 1);
            wapped_texture_ptr_->UpdateTexImage(2, WappedTexture::kTypeLUMINANCE,
                                                (unsigned char *) pImage->ppPlane[2],
                                                pImage->width >> 1,
                                                pImage->height >> 1);
            break;
        default:
            break;
    }

    NativeImageUtil::FreeNativeImage(pImage);
    delete pImage;

    frame_index_++;

    glClear(GL_STENCIL_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(1.0, 1.0, 1.0, 1.0);

    if (effect_type_ == EffectType::k3DVR) {
        glEnable(GL_DEPTH_TEST);
    }

    glUseProgram(wapped_shader_program_ptr_->GetProgram());

    glBindVertexArray(vao_id_);
    wapped_shader_program_ptr_->SetMat4("MVPMatrix", MVPMatrix_);

    std::vector<std::string> names = {"texture0", "texture1", "texture2"};
    wapped_texture_ptr_->BindTexture(names);

    wapped_shader_program_ptr_->SetInt("img_type", img_type);
    wapped_shader_program_ptr_->SetInt("effect_type", effect_type_);
    if (effect_type_ == kDynimicMesh || effect_type_ == k3DVR) {
        float offset;
        if (effect_type_ == kDynimicMesh) {
            offset = (sin(frame_index_ * MATH_PI / 40) + 1.0f) / 2.0f;
        } else {
            offset = (sin(frame_index_ * MATH_PI / 25) + 1.0f) / 2.0f;
        }
        wapped_shader_program_ptr_->SetFloat("offset", offset);
        wapped_shader_program_ptr_->SetVec2("tex_size", glm::vec2(real_width_, real_height_));
    }

    if (effect_type_ == k3DVR) {
        glDrawArrays(GL_TRIANGLES, 0, vertex_coords_.size());
    } else {
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, (const void *) 0);
    }

    if (IsSurface()) {
        if (!egl_core_ptr_->SwapBuffers()) {
            LOGE("eglSwapBuffers() returned error %d", eglGetError());
        }
    } else {
        Resume();
    }

    if (m_MsgContext && m_MsgCallback && last_process_ < process) {
        if(last_process_ < 0) { // for seek
            last_process_++;
        } else {
            m_MsgCallback(m_MsgContext, 1, process);
            last_process_ = process;
        }
    }
}

void OpenGLESRender::UpdateMVPMatrix(int angleX, int angleY, float scaleX, float scaleY) {
    angleX = angleX % 360;
    angleY = angleY % 360;

    //转化为弧度角
    float radiansX = static_cast<float>(MATH_PI / 180.0f * angleX);
    float radiansY = static_cast<float>(MATH_PI / 180.0f * angleY);

    // Projection matrix

    int camera_pos;
    glm::mat4 Projection;
    if (effect_type_ == EffectType::k3DVR) {
        float ratio = real_width_ / real_height_;
        Projection = glm::frustum(-ratio, ratio, -1.0f, 1.0f, 4.0f, 100.0f);
        camera_pos = 3;
    } else {
        Projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, 0.1f, 100.0f);
        camera_pos = 4;
    }
    //glm::mat4 Projection = glm::frustum(-ratio, ratio, -1.0f, 1.0f, 4.0f, 100.0f);
    //glm::mat4 Projection = glm::perspective(45.0f,ratio, 0.1f,100.f);

    // View matrix
    glm::mat4 View = glm::lookAt(
            glm::vec3(0, 0, camera_pos), // Camera is at (0,0,1), in World Space
            glm::vec3(0, 0, 0), // and looks at the origin
            glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
    );

    // Model matrix
    glm::mat4 Model = glm::mat4(1.0f);
    Model = glm::scale(Model, glm::vec3(scaleX, scaleY, 1.0f));
    Model = glm::rotate(Model, radiansX, glm::vec3(1.0f, 0.0f, 0.0f));
    Model = glm::rotate(Model, radiansY, glm::vec3(0.0f, 1.0f, 0.0f));
    Model = glm::translate(Model, glm::vec3(0.0f, 0.0f, 0.0f));

    MVPMatrix_ = Projection * View * Model;
}

void OpenGLESRender::SetTouchLoc(float touchX, float touchY) {
    touchxy_.x = touchX / real_width_;
    touchxy_.y = touchY / real_height_;
}

void OpenGLESRender::GenerateMesh() {
    if (effect_type_ == EffectType::k3DVR) {
        // 构建顶点坐标
        for (float vAngle = 90; vAngle > -90; vAngle = vAngle - ANGLE_SPAN) {//垂直方向ANGLE_SPAN度一份
            for (float hAngle = 360; hAngle > 0; hAngle = hAngle - ANGLE_SPAN) {//水平方向ANGLE_SPAN度一份
                //纵向横向各到一个角度后计算对应的此点在球面上的坐标
                double xozLength = BALL_RADIUS * UNIT_SIZE * cos(RADIAN(vAngle));
                float x1 = (float) (xozLength * cos(RADIAN(hAngle)));
                float z1 = (float) (xozLength * sin(RADIAN(hAngle)));
                float y1 = (float) (BALL_RADIUS * UNIT_SIZE * sin(RADIAN(vAngle)));
                xozLength = BALL_RADIUS * UNIT_SIZE * cos(RADIAN(vAngle - ANGLE_SPAN));
                float x2 = (float) (xozLength * cos(RADIAN(hAngle)));
                float z2 = (float) (xozLength * sin(RADIAN(hAngle)));
                float y2 = (float) (BALL_RADIUS * UNIT_SIZE * sin(RADIAN(vAngle - ANGLE_SPAN)));
                xozLength = BALL_RADIUS * UNIT_SIZE * cos(RADIAN(vAngle - ANGLE_SPAN));
                float x3 = (float) (xozLength * cos(RADIAN(hAngle - ANGLE_SPAN)));
                float z3 = (float) (xozLength * sin(RADIAN(hAngle - ANGLE_SPAN)));
                float y3 = (float) (BALL_RADIUS * UNIT_SIZE * sin(RADIAN(vAngle - ANGLE_SPAN)));
                xozLength = BALL_RADIUS * UNIT_SIZE * cos(RADIAN(vAngle));
                float x4 = (float) (xozLength * cos(RADIAN(hAngle - ANGLE_SPAN)));
                float z4 = (float) (xozLength * sin(RADIAN(hAngle - ANGLE_SPAN)));
                float y4 = (float) (BALL_RADIUS * UNIT_SIZE * sin(RADIAN(vAngle)));

                glm::vec3 v1(x1, y1, z1);
                glm::vec3 v2(x2, y2, z2);
                glm::vec3 v3(x3, y3, z3);
                glm::vec3 v4(x4, y4, z4);

                //构建第一个三角形
                vertex_coords_.push_back(v1);
                vertex_coords_.push_back(v2);
                vertex_coords_.push_back(v4);
                //构建第二个三角形
                vertex_coords_.push_back(v4);
                vertex_coords_.push_back(v2);
                vertex_coords_.push_back(v3);
            }
        }

        // 构建纹理坐标，球面展开后的矩形
        int width = 360 / ANGLE_SPAN;//列数
        int height = 180 / ANGLE_SPAN;//行数
        float dw = 1.0f / width;
        float dh = 1.0f / height;
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                //每一个小矩形，由两个三角形构成，共六个点
                float s = j * dw;
                float t = i * dh;
                glm::vec2 v1(s, t);
                glm::vec2 v2(s, t + dh);
                glm::vec2 v3(s + dw, t + dh);
                glm::vec2 v4(s + dw, t);

                //构建第一个三角形
                texture_coords_.push_back(v1);
                texture_coords_.push_back(v2);
                texture_coords_.push_back(v4);
                //构建第二个三角形
                texture_coords_.push_back(v4);
                texture_coords_.push_back(v2);
                texture_coords_.push_back(v3);
            }
        }
    } else {
        glm::vec3 v1(-1.0f, 1.0f, 0.0f); // Position 0 : left top
        glm::vec3 v2(-1.0f, -1.0f, 0.0f); // Position 1 : left bottom
        glm::vec3 v3(1.0f, -1.0f, 0.0f); // Position 2 : right bottom
        glm::vec3 v4(1.0f, 1.0f, 0.0f); // Position 3 : right top
        vertex_coords_.emplace_back(v1);
        vertex_coords_.emplace_back(v2);
        vertex_coords_.emplace_back(v3);
        vertex_coords_.emplace_back(v4);

        glm::vec2 t1(0.0f, 0.0f); // TexCoord 0 : left top
        glm::vec2 t2(0.0f, 1.0f); // TexCoord 1 : left bottom
        glm::vec2 t3(1.0f, 1.0f); // TexCoord 2 : right bottom
        glm::vec2 t4(1.0f, 0.0f); // TexCoord 3 : right top
        texture_coords_.emplace_back(t1);
        texture_coords_.emplace_back(t2);
        texture_coords_.emplace_back(t3);
        texture_coords_.emplace_back(t4);
    }
}


