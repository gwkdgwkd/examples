#include "egl_core.h"
#include "log.h"

const char *EGLCore::TAG_ = "EGLCore";

EGLCore::EGLCore() {
    TRACE_FUNC();
    pfneglPresentationTimeANDROID = 0;
    display_ = EGL_NO_DISPLAY;
    context_ = EGL_NO_CONTEXT;
    surface_ = EGL_NO_SURFACE;
}

void EGLCore::Release() {
    TRACE_FUNC();
    eglDestroySurface(display_, surface_);
    surface_ = EGL_NO_SURFACE;
    eglMakeCurrent(display_, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    eglDestroyContext(display_, context_);
    display_ = EGL_NO_DISPLAY;
    context_ = EGL_NO_CONTEXT;
}

bool EGLCore::CreateWindowSurface(ANativeWindow *window) {
    TRACE_FUNC();
    EGLint format;
    if (!eglGetConfigAttrib(display_, config_, EGL_NATIVE_VISUAL_ID, &format)) {
        LOGE("eglGetConfigAttrib() returned error %d", eglGetError());
        Release();
        return false;
    }

    ANativeWindow_setBuffersGeometry(window, 0, 0, format);
    if (!(surface_ = eglCreateWindowSurface(display_, config_, window, 0))) {
        LOGE("eglCreateWindowSurface() returned error %d", eglGetError());
        return false;
    }

    return true;
}

bool EGLCore::MakeCurrent() {
    if(!eglMakeCurrent(display_, surface_, surface_, context_)) {
        LOGE("eglMakeCurrent failed!");
        return false;
    }
    return true;
}

bool EGLCore::SwapBuffers() {
    TRACE_FUNC();
    return eglSwapBuffers(display_, surface_);
}

bool EGLCore::Init(ANativeWindow *window) {
    TRACE_FUNC();
    if (!InitEGL()) {
        LOGE("InitEGL failed!");
        return false;
    }
    LOGI("window %p", window);
    if (!CreateWindowSurface(window)) {
        LOGE("CreateWindowSurface failed!");
        return false;
    }
    return MakeCurrent();
}

bool EGLCore::InitEGL() {
    TRACE_FUNC();
    return this->InitEGL(NULL);
}

bool EGLCore::InitEGL(EGLContext shared_context) {
    TRACE_FUNC();
    EGLint numConfigs;

    const EGLint attribs[] = {EGL_BUFFER_SIZE, 32, EGL_ALPHA_SIZE, 8, EGL_BLUE_SIZE, 8,
                              EGL_GREEN_SIZE, 8, EGL_RED_SIZE, 8, EGL_RENDERABLE_TYPE,
                              EGL_OPENGL_ES2_BIT,
                              EGL_SURFACE_TYPE, EGL_WINDOW_BIT, EGL_NONE};

    if ((display_ = eglGetDisplay(EGL_DEFAULT_DISPLAY)) == EGL_NO_DISPLAY) {
        LOGE("eglGetDisplay() returned error %d", eglGetError());
        return false;
    }
    if (!eglInitialize(display_, 0, 0)) {
        LOGE("eglInitialize() returned error %d", eglGetError());
        return false;
    }

    if (!eglChooseConfig(display_, attribs, &config_, 1, &numConfigs)) {
        LOGE("eglChooseConfig() returned error %d", eglGetError());
        Release();
        return false;
    }

    EGLint eglContextAttributes[] = {EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE};
    if (!(context_ = eglCreateContext(display_, config_,
                                      NULL == shared_context ? EGL_NO_CONTEXT : shared_context,
                                      eglContextAttributes))) {
        LOGE("eglCreateContext() returned error %d", eglGetError());
        Release();
        return false;
    }

    pfneglPresentationTimeANDROID = (PFNEGLPRESENTATIONTIMEANDROIDPROC) eglGetProcAddress(
            "eglPresentationTimeANDROID");
    if (!pfneglPresentationTimeANDROID) {
        LOGE("eglPresentationTimeANDROID is not available!");
        return false;
    }

    return true;
}
