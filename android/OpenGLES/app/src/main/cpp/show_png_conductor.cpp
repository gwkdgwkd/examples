#include "show_png_conductor.h"

#include <chrono>
#include "log.h"

const char *ShowPngConductor::TAG_ = "ShowPngConductor";

ShowPngConductor::ShowPngConductor() :
        msg_(MSG_NONE), egl_core_ptr_(0) {
    TRACE_FUNC();
    wapped_shader_program_ptr_ = new WappedShaderProgram();
    screen_left_ = 0;
    screen_right_ = 0;
    screen_width_ = 720;
    screen_height_ = 720;
    png_pic_decoder_ptr_ = nullptr;
    window_ = nullptr;
}

bool ShowPngConductor::Start(char *spriteFilePath) {
    TRACE_FUNC();
    png_pic_decoder_ptr_ = new PngPicDecoder();
    png_pic_decoder_ptr_->OpenFile(spriteFilePath);
    thread_ = std::thread(&ShowPngConductor::RenderLoop, this);

    return true;
}

void ShowPngConductor::Stop() {
    TRACE_FUNC();
    std::unique_lock <std::mutex> lck(mutex_);
    msg_ = MSG_RENDER_LOOP_EXIT;
    condition_variable_.notify_all();
    thread_.join();
}

bool ShowPngConductor::SetWindow(ANativeWindow *window) {
    TRACE_FUNC();
    std::unique_lock <std::mutex> lck(mutex_);
    msg_ = MSG_WINDOW_SET;
    window_ = window;
    condition_variable_.notify_all();

    return true;
}

void ShowPngConductor::ResetSize(int width, int height) {
    TRACE_FUNC();
    LOGI("ShowPngConductor::resetSize width:%d; height:%d", width, height);
    std::unique_lock <std::mutex> lck(mutex_);
    screen_width_ = width;
    screen_height_ = height;

    return;
}

bool ShowPngConductor::Initialize() {
    TRACE_FUNC();
    bool ret = false;
    egl_core_ptr_ = new EGLCore();
    if ((ret = egl_core_ptr_->Init(window_)) == false) {
        LOGE("egl core init failed!");
        return false;
    }

    wapped_texture_ptr_ = new WappedTexture();
    if ((ret = wapped_texture_ptr_->CreateTexture()) == false) {
        LOGE("create texture failed!");
        Destroy();
        return false;
    }

    UpdateTexImage();

    if (!wapped_shader_program_ptr_->Init()) {
        LOGE("shader program init failed!");
        return false;
    }

    return true;
}

void ShowPngConductor::RenderLoop() {
    TRACE_FUNC();
    bool renderingEnabled = true;
    while (renderingEnabled) {
        std::unique_lock <std::mutex> lck(mutex_);
        switch (msg_) {
            case MSG_WINDOW_SET:
                Initialize();
                break;
            case MSG_RENDER_LOOP_EXIT:
                renderingEnabled = false;
                Destroy();
                break;
            default:
                break;
        }
        msg_ = MSG_NONE;

        if (egl_core_ptr_) {
            egl_core_ptr_->MakeCurrent();
            DrawFrame();
            condition_variable_.wait(lck);
            std::this_thread::sleep_for(std::chrono::milliseconds(100));

        }
        lck.unlock();
    }

    return;
}

void ShowPngConductor::Destroy() {
    TRACE_FUNC();
    if (NULL != wapped_shader_program_ptr_) {
        delete wapped_shader_program_ptr_;
        wapped_shader_program_ptr_ = NULL;
    }
    if (egl_core_ptr_) {
        egl_core_ptr_->release();
        egl_core_ptr_ = NULL;
    }
    return;
}

void ShowPngConductor::UpdateTexImage() {
    TRACE_FUNC();
    if (png_pic_decoder_ptr_) {
        const RawImageData raw_image_data = png_pic_decoder_ptr_->GetRawImageData();
        LOGI("raw_image_data Meta: width is %d height is %d size is %d colorFormat is %d",
             raw_image_data.width, raw_image_data.height, raw_image_data.size,
             raw_image_data.gl_color_format);
        LOGI("colorFormat is %d", GL_RGBA);
        wapped_texture_ptr_->UpdateTexImage((byte *) raw_image_data.data, raw_image_data.width,
                                            raw_image_data.height);
        png_pic_decoder_ptr_->ReleaseRawImageData(&raw_image_data);
    }
}

void ShowPngConductor::DrawFrame() {
    TRACE_FUNC();
    Render();
    if (!egl_core_ptr_->SwapBuffers()) {
        LOGE("eglSwapBuffers() returned error %d", eglGetError());
    }
}

void ShowPngConductor::Render() {
    TRACE_FUNC();
    LOGI("left %d, right %d, width %d, height %d", screen_left_, screen_right_, screen_width_,
         screen_height_);
    glViewport(screen_left_, screen_right_, screen_width_, screen_height_);
    glClearColor(0.0f, 0.0f, 1.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glUseProgram(wapped_shader_program_ptr_->GetProgram());
    static const GLfloat _vertices[] = {-1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f, -1.0f};
    glVertexAttribPointer(WappedShaderProgram::ATTRIBUTE_VERTEX, 2, GL_FLOAT, 0, 0, _vertices);
    glEnableVertexAttribArray(WappedShaderProgram::ATTRIBUTE_VERTEX);
    static const GLfloat texCoords[] = {0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f};
    glVertexAttribPointer(WappedShaderProgram::ATTRIBUTE_TEXCOORD, 2, GL_FLOAT, 0, 0, texCoords);
    glEnableVertexAttribArray(WappedShaderProgram::ATTRIBUTE_TEXCOORD);
    wapped_texture_ptr_->BindTexture(wapped_shader_program_ptr_->GetUniformSampler());
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

