#include "native_window_render.h"
#include "log.h"

#define AV_SYNC_THRESHOLD_MIN 0.04
#define AV_SYNC_THRESHOLD_MAX 0.1

NativeWindowRender::NativeWindowRender(JNIEnv *env, jobject surface, enum VideoRenderType type)
        : VideoRenderInterface(type) {
    native_window_ = ANativeWindow_fromSurface(env, surface);
}

NativeWindowRender::~NativeWindowRender() {
    if (native_window_)
        ANativeWindow_release(native_window_);
}

void NativeWindowRender::Init(int videoWidth, int videoHeight, int *dstSize,
                              FFmpegVideoDecoder *video_decoder) {
    LOGI("NativeRender::Init m_NativeWindow=%p, video[w,h]=[%d, %d]", native_window_, videoWidth,
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

    ANativeWindow_setBuffersGeometry(native_window_, real_width_,
                                     real_height_, WINDOW_FORMAT_RGBA_8888);

    dstSize[0] = real_width_;
    dstSize[1] = real_height_;
}

void NativeWindowRender::UnInit() {

}

void NativeWindowRender::RenderVideoFrame(NativeImage *pImage) {
    TRACE_FUNC();
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

void NativeWindowRender::Process() {
    TRACE_FUNC();

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


    ANativeWindow_lock(native_window_, &native_window_buffer_, nullptr);
    uint8_t *dstBuffer = static_cast<uint8_t *>(native_window_buffer_.bits);
    int srcLineSize = pImage->width * 4; // RGBA
    int dstLineSize = native_window_buffer_.stride * 4;
    for (int i = 0; i < real_height_; ++i) {
        memcpy(dstBuffer + i * dstLineSize, pImage->ppPlane[0] + i * srcLineSize, srcLineSize);
    }

    ANativeWindow_unlockAndPost(native_window_);
    NativeImageUtil::FreeNativeImage(pImage);
    delete pImage;
}


