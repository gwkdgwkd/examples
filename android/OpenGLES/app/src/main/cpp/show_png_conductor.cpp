#include "show_png_conductor.h"

#include <chrono>
#include <thread>
#include "log.h"

const char *ShowPngConductor::TAG_ = "ShowPngConductor";

ShowPngConductor::ShowPngConductor() :
        msg_(MSG_NONE), egl_core_ptr_(0) {
    TRACE_FUNC();
    wapped_shader_program_ptr_ = new WappedShaderProgram();
    this->screen_width_ = 720;
    this->screen_height_ = 720;
    png_pic_decoder_ptr_ = NULL;
}

bool ShowPngConductor::Start(char *spriteFilePath) {
    LOGI("Creating VideoDutePlayerController thread");
    png_pic_decoder_ptr_ = new PngPicDecoder();
    png_pic_decoder_ptr_->openFile(spriteFilePath);
//    pthread_create(&_threadId, 0, threadStartCallback, this);
    return true;
}

void ShowPngConductor::Stop() {
    LOGI("Stopping VideoDutePlayerController Render thread");
    /*send message to render thread to stop rendering*/
//    pthread_mutex_lock(&mLock);
    msg_ = MSG_RENDER_LOOP_EXIT;
//    pthread_cond_signal(&mCondition);
//    pthread_mutex_unlock(&mLock);

    LOGI("we will join render thread stop");
//    pthread_join(_threadId, 0);
    LOGI("VideoDutePlayerController Render thread stopped");
}

bool ShowPngConductor::SetWindow(ANativeWindow *window) {
//  pthread_mutex_lock(&mLock);
    bool ret = false;
    egl_core_ptr_ = new EGLCore();
    if((ret = egl_core_ptr_->Init(window)) == false) {
        return false;
    }

    wapped_texture_ptr_ = new WappedTexture();
    if((ret = wapped_texture_ptr_->CreateTexture()) == false) {
        Destroy();
        return false;
    }

    UpdateTexImage();

    if (!wapped_shader_program_ptr_->Init()) {
        return false;
    }

    return true;
//    msg_ = MSG_WINDOW_SET;
//    window_ = window;
//    pthread_cond_signal(&mCondition);
//    pthread_mutex_unlock(&mLock);
}

void ShowPngConductor::ResetSize(int width, int height) {
    LOGI("VideoDutePlayerController::resetSize width:%d; height:%d", width, height);
//    pthread_mutex_lock(&mLock);
    screen_width_ = width;
    screen_height_ = height;
//    wapped_shader_program_ptr_->ResetRenderSize(0, 0, width, height);
//    pthread_cond_signal(&mCondition);
//    pthread_mutex_unlock(&mLock);
}

void *ShowPngConductor::ThreadStartCallback(void *myself) {
    ShowPngConductor *controller = (ShowPngConductor *) myself;
    controller->RenderLoop();
//    pthread_exit(0);
    return 0;
}

void ShowPngConductor::RenderLoop() {
    bool renderingEnabled = true;
    LOGI("renderLoop()");
    while (renderingEnabled) {
//        pthread_mutex_lock(&mLock);
        /*process incoming messages*/
        switch (msg_) {
            case MSG_WINDOW_SET:
//                Init();
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
//            egl_core_ptr_->MakeCurrent(previewSurface);
            DrawFrame();
//            pthread_cond_wait(&mCondition, &mLock);
            std::this_thread::sleep_for(std::chrono::milliseconds(100));

        }

//        pthread_mutex_unlock(&mLock);
    }
    LOGI("Render loop exits");

    return;
}


void ShowPngConductor::Destroy() {
    LOGI("dealloc renderer ...");
    if (NULL != wapped_shader_program_ptr_) {
//        wapped_shader_program_ptr_->Dealloc();
        delete wapped_shader_program_ptr_;
        wapped_shader_program_ptr_ = NULL;
    }
    if (egl_core_ptr_) {
//        egl_core_ptr_->releaseSurface(previewSurface);
        egl_core_ptr_->release();
        egl_core_ptr_ = NULL;
    }
    return;
}

void ShowPngConductor::UpdateTexImage() {
    if (png_pic_decoder_ptr_) {
        const RawImageData raw_image_data = png_pic_decoder_ptr_->getRawImageData();
        LOGI("raw_image_data Meta: width is %d height is %d size is %d colorFormat is %d",
             raw_image_data.width, raw_image_data.height, raw_image_data.size,
             raw_image_data.gl_color_format);
        LOGI("colorFormat is %d", GL_RGBA);
        wapped_texture_ptr_->UpdateTexImage((byte *) raw_image_data.data, raw_image_data.width,
                                            raw_image_data.height);
        png_pic_decoder_ptr_->releaseRawImageData(&raw_image_data);
    }
}

void ShowPngConductor::DrawFrame() {
//    wapped_shader_program_ptr_->render();
    if (!egl_core_ptr_->SwapBuffers()) {
        LOGE("eglSwapBuffers() returned error %d", eglGetError());
    }
}
