#include "com_example_opengles_TriangleRenderActivity.h"

#include <string>
#include <android/native_window.h>
#include <android/native_window_jni.h>
#include "triangle_render.h"
#include "log.h"

static ANativeWindow *window = 0;
TriangleRender *triangle_render;

extern "C" JNIEXPORT void JNICALL Java_com_example_opengles_TriangleRenderActivity_createGraphical
  (JNIEnv *env, jobject obj, jint width, jint height, jobject surface) {
    TRACE_FUNC();
  window = ANativeWindow_fromSurface(env, surface);
  triangle_render = new TriangleRender();
  triangle_render->init(width, height, window);
  //渲染
  triangle_render->render();

}
