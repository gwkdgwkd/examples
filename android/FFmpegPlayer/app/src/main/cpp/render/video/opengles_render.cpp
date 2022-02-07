#include "opengles_render.h"
#include "log.h"

#define AV_SYNC_THRESHOLD_MIN 0.04
#define AV_SYNC_THRESHOLD_MAX 0.1

#define MATH_PI 3.1415926535897932384626433832802

static const char *kVertexShader = R"(
#version 300 es
layout(location = 0) in vec4 a_position;
layout(location = 1) in vec2 a_texcoord;
out vec2 v_texcorrd;
void main(void) {
    gl_Position = a_position;
    v_texcorrd = a_texcoord;
})";

static const char *kFragmentShader = R"(
#version 300 es
precision mediump float;
in vec2 v_texcorrd;
layout(location = 0) out vec4 out_color;
uniform sampler2D s_texturemap;
uniform int effect_type;// 0:no,1:DynimicMesh,2:GrayImage
uniform float offset;
uniform vec2 tex_size;
void main() {
    if(effect_type == 1) {
        vec2 img_texcoord = v_texcorrd * tex_size;
        float side_length = tex_size.y / 6.0;
        float max_offset = 0.08 * side_length;
        float x = mod(img_texcoord.x, floor(side_length));
        float y = mod(img_texcoord.y, floor(side_length));

        float final_offset = offset * max_offset;

        if(final_offset <= x && x <= side_length - final_offset && final_offset <= y && y <= side_length - final_offset) {
            out_color = texture(s_texturemap, v_texcorrd);
        }else{
            out_color = vec4(1.0, 1.0, 1.0, 1.0);
        }
    } else if(effect_type == 2) {
        out_color = texture(s_texturemap, v_texcorrd);
        if(v_texcorrd.x > 0.5) {
            out_color = vec4(vec3(out_color.r*0.299 + out_color.g*0.587 + out_color.b*0.114), out_color.a);
        }
    } else {
        out_color = texture(s_texturemap, v_texcorrd);
    }
})";

static const char *kUniformName = "s_texturemap";

/* 渲染坐标系或OpenGLES坐标系:
    (-1.0, 1.0)             ( 1.0, 1.0)
                ( 0.0, 0.0)
    (-1.0,-1.0)             ( 1.0,-1.0)
*/
static GLfloat kVerticesCoords[] = {
        -1.0f,  1.0f, 0.0f,  // Position 0 : left top
        -1.0f, -1.0f, 0.0f,  // Position 1 : left bottom
        1.0f, -1.0f, 0.0f,   // Position 2 : right bottom
        1.0f,  1.0f, 0.0f,   // Position 3 : right top
};

/* 纹理坐标：
    (0,0)       (1,0)

    (0,1)       (1,1)
*/
static GLfloat kTextureCoords[] = {
        0.0f,  0.0f,        // TexCoord 0 : left top
        0.0f,  1.0f,        // TexCoord 1 : left bottom
        1.0f,  1.0f,        // TexCoord 2 : right bottom
        1.0f,  0.0f         // TexCoord 3 : right top

// 旋转180度：
//        1.0f,  1.0f,        // TexCoord 2 : right bottom
//        1.0f,  0.0f,        // TexCoord 3 : right top
//        0.0f,  0.0f,        // TexCoord 0 : left top
//        0.0f,  1.0f         // TexCoord 1 : left bottom
};

GLushort kIndices[] = { 0, 1, 2, 0, 2, 3 };

OpenGLESRender::OpenGLESRender(JNIEnv *env, jobject surface) : VideoRenderInterface(
        VIDEO_RENDER_ANWINDOW) {
    TRACE_FUNC();
    is_opengles_init_ = false;
    effect_type_ = EffectType::kNoEffect;
    frame_index_ = 0;
    native_window_ = ANativeWindow_fromSurface(env, surface);
}

OpenGLESRender::~OpenGLESRender() {
    TRACE_FUNC();
    if (native_window_)
        ANativeWindow_release(native_window_);
}

void OpenGLESRender::Init(int videoWidth, int videoHeight, int *dstSize,
                          FFmpegVideoDecoder *video_decoder) {
    TRACE_FUNC();
    LOGI("NativeRender::Init OpenGLESRender=%p, video[w,h]=[%d, %d]", native_window_, videoWidth,
         videoHeight);
    if (native_window_ == nullptr) return;

    video_decoder_ = video_decoder;

    int windowWidth = ANativeWindow_getWidth(native_window_);
    int windowHeight = ANativeWindow_getHeight(native_window_);

    if (windowWidth < windowHeight * videoWidth / videoHeight) {
        real_width_ = windowWidth;
        real_height_ = windowWidth * videoHeight / videoWidth;
    } else {
        real_width_ = windowHeight * videoWidth / videoHeight;
        real_height_ = windowHeight;
    }
    LOGI("NativeRender::Init window[w,h]=[%d, %d],DstSize[w, h]=[%d, %d]", windowWidth,
         windowHeight, real_width_, real_height_);

    dstSize[0] = real_width_;
    dstSize[1] = real_height_;

    Start();
}

void OpenGLESRender::UnInit() {

}

bool OpenGLESRender::VaoInit() {
    TRACE_FUNC();

    // Generate VBO Ids and load the VBOs with data
    glGenBuffers(3, vbo_ids_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_ids_[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(kVerticesCoords), kVerticesCoords, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, vbo_ids_[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(kTextureCoords), kTextureCoords, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_ids_[2]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(kIndices), kIndices, GL_STATIC_DRAW);

    // Generate VAO Id
    glGenVertexArrays(1, &vao_id_);
    glBindVertexArray(vao_id_);

    glBindBuffer(GL_ARRAY_BUFFER, vbo_ids_[0]);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (const void *)0);
    glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);

    glBindBuffer(GL_ARRAY_BUFFER, vbo_ids_[1]);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (const void *)0);
    glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_ids_[2]);

    glBindVertexArray(GL_NONE);

    return true;
}

bool OpenGLESRender::OpenglesInit() {
    TRACE_FUNC();
    bool ret = false;

    egl_core_ptr_ = new EGLCore();
    if ((ret = egl_core_ptr_->Init(native_window_)) == false) {
        LOGE("egl core init failed!");
        return ret;
    }

    wapped_texture_ptr_ = new WappedTexture();
    if ((ret = wapped_texture_ptr_->CreateTexture()) == false) {
        LOGE("create texture failed!");
        return ret;
    }

    wapped_shader_program_ptr_ = new WappedShaderProgram();
    if (!wapped_shader_program_ptr_->Init(kVertexShader, kFragmentShader, kUniformName)) {
        LOGE("shader program init failed!");
        return ret;
    }

    is_opengles_init_ = true;

    VaoInit();

    Pause();

    return true;
}

void OpenGLESRender::RenderVideoFrame(NativeImage *pImage) {
    TRACE_FUNC();
}

void OpenGLESRender::Process() {
    TRACE_FUNC();

    if (!is_opengles_init_) {
        OpenglesInit();
        return;
    }

    NativeImage *pImage = video_decoder_->GetVideoImage();
    if (native_window_ == nullptr || pImage == nullptr) return;

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

    std::this_thread::sleep_for(std::chrono::microseconds(real_delay));

    wapped_texture_ptr_->UpdateTexImage((unsigned char *) pImage->ppPlane[0], pImage->width,
                                        pImage->height);
    DrawFrame();

    NativeImageUtil::FreeNativeImage(pImage);
    delete pImage;

    frame_index_++;
}

void OpenGLESRender::DrawFrame() {
    TRACE_FUNC();
    LOGI("left %d, right %d, width %d, height %d", 0, 0, real_width_, real_height_);

    glClear(GL_STENCIL_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(1.0, 1.0, 1.0, 1.0);

    glUseProgram(wapped_shader_program_ptr_->GetProgram());

    glBindVertexArray(vao_id_);

    wapped_texture_ptr_->BindTexture(wapped_shader_program_ptr_->GetUniformSampler());

    wapped_shader_program_ptr_->SetInt("effect_type", effect_type_);
    if(effect_type_ == kDynimicMesh) {
        float offset = (sin(frame_index_ * MATH_PI / 40) + 1.0f) / 2.0f;
        wapped_shader_program_ptr_->SetFloat("offset", offset);
        wapped_shader_program_ptr_->SetVec2("tex_size", glm::vec2(real_width_, real_height_));
    }

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, (const void *)0);

    if (!egl_core_ptr_->SwapBuffers()) {
        LOGE("eglSwapBuffers() returned error %d", eglGetError());
    }
}


