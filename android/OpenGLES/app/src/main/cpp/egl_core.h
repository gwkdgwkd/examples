#ifndef OPENGLES_EGL_CORE_H
#define OPENGLES_EGL_CORE_H


#include <android/native_window.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>

typedef EGLBoolean (EGLAPIENTRYP PFNEGLPRESENTATIONTIMEANDROIDPROC)(EGLDisplay display, EGLSurface surface, khronos_stime_nanoseconds_t time);

class EGLCore {
public:
	EGLCore();
    virtual ~EGLCore() {}

    bool Init(ANativeWindow*);
	bool SwapBuffers();

    void release();

private:
	bool InitEGL();
	bool InitEGL(EGLContext sharedContext);
	bool CreateWindowSurface(ANativeWindow* _window);

	EGLDisplay display_;
	EGLConfig config_;
	EGLContext context_;
    EGLSurface surface_;
	PFNEGLPRESENTATIONTIMEANDROIDPROC pfneglPresentationTimeANDROID;
	static const char* TAG_;
};
#endif // OPENGLES_EGL_CORE_H
