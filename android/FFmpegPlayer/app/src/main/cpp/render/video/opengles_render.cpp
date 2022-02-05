#include "opengles_render.h"
#include "log.h"

#define AV_SYNC_THRESHOLD_MIN 0.04
#define AV_SYNC_THRESHOLD_MAX 0.1

// https://blog.csdn.net/u010302327/article/details/77031445

OpenGLESRender::OpenGLESRender(JNIEnv *env, jobject surface) : VideoRenderInterface(
        VIDEO_RENDER_ANWINDOW) {
    TRACE_FUNC();
    is_opengles_init_ = false;
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
    if (!wapped_shader_program_ptr_->Init()) {
        LOGE("shader program init failed!");
        return ret;
    }

    is_opengles_init_ = true;
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
}

void OpenGLESRender::DrawFrame() {
    TRACE_FUNC();
    Render();
    if (!egl_core_ptr_->SwapBuffers()) {
        LOGE("eglSwapBuffers() returned error %d", eglGetError());
    }
}

void OpenGLESRender::Render() {
    TRACE_FUNC();
    LOGI("left %d, right %d, width %d, height %d", 0, 0, real_width_,
         real_height_);
    glViewport(0, 0, real_width_, real_height_);
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


