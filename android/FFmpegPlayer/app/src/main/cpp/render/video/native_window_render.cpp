#include "native_window_render.h"

NativeWindowRender::NativeWindowRender(JNIEnv *env, jobject surface) : VideoRenderInterface(
        VIDEO_RENDER_ANWINDOW) {
    native_window_ = ANativeWindow_fromSurface(env, surface);
}

NativeWindowRender::~NativeWindowRender() {
    if (native_window_)
        ANativeWindow_release(native_window_);

}

void NativeWindowRender::Init(int videoWidth, int videoHeight, int *dstSize) {
    LOGI("NativeRender::Init m_NativeWindow=%p, video[w,h]=[%d, %d]", native_window_, videoWidth,
         videoHeight);
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
    LOGI("NativeRender::Init window[w,h]=[%d, %d],DstSize[w, h]=[%d, %d]", windowWidth,
         windowHeight, real_width_, real_height_);

    ANativeWindow_setBuffersGeometry(native_window_, real_width_,
                                     real_height_, WINDOW_FORMAT_RGBA_8888);

    dstSize[0] = real_width_;
    dstSize[1] = real_height_;
}

void NativeWindowRender::RenderVideoFrame(NativeImage *pImage) {
    if (native_window_ == nullptr || pImage == nullptr) return;
    ANativeWindow_lock(native_window_, &native_window_buffer_, nullptr);
    uint8_t *dstBuffer = static_cast<uint8_t *>(native_window_buffer_.bits);
    int srcLineSize = pImage->width * 4; // RGBA
    int dstLineSize = native_window_buffer_.stride * 4;
    for (int i = 0; i < real_height_; ++i) {
        memcpy(dstBuffer + i * dstLineSize, pImage->ppPlane[0] + i * srcLineSize, srcLineSize);
    }
    ANativeWindow_unlockAndPost(native_window_);
}

void NativeWindowRender::UnInit() {

}

