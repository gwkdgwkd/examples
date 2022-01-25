#ifndef OPENGLES_TRIANGLE_RENDER_H
#define OPENGLES_TRIANGLE_RENDER_H
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <android/native_window.h>
#include <android/native_window_jni.h>
#include "egl_core.h"

class TriangleRender{
public:
    void init(int width, int height, ANativeWindow *window);
    void render();
    void destroy();
private:
    int loadShader(GLenum shaderType,const char* pSource);
    void createProgram();
    bool checkGlError(const char* op);

    int width_;
    int height_;
    ANativeWindow * window_;
    EGLCore *eglCore_;
};
#endif //OPENGLES_TRIANGLE_RENDER_H
