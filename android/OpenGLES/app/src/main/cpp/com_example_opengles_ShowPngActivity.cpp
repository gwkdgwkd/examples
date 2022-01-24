#include "com_example_opengles_ShowPngActivity.h"

#include <string>
#include <android/native_window.h>
#include <android/native_window_jni.h>
#include "show_png_conductor.h"
#include "log.h"

static ANativeWindow *window = 0;
static ShowPngConductor *conductor = 0;

extern "C" JNIEXPORT void JNICALL Java_com_example_opengles_ShowPngActivity_init
  (JNIEnv *env, jobject obj, jstring png_path) {
    TRACE_FUNC();
    conductor = new ShowPngConductor();
    char* path = (char*) env->GetStringUTFChars(png_path, NULL);
    conductor->Start(path);
    env->ReleaseStringUTFChars(png_path, path);
}

extern "C" JNIEXPORT void JNICALL Java_com_example_opengles_ShowPngActivity_setSurface
  (JNIEnv *env, jobject obj, jobject surface){
    TRACE_FUNC();
    if (surface != 0 && NULL != conductor) {
        window = ANativeWindow_fromSurface(env, surface);
        LOGI("Got window %p", window);
        conductor->SetWindow(window);
    } else if (window != 0) {
        LOGI("Releasing window");
        ANativeWindow_release(window);
        window = 0;
    }
}

extern "C" JNIEXPORT void JNICALL Java_com_example_opengles_ShowPngActivity_resetSize
  (JNIEnv *env, jobject obj, jint width, jint height) {
    TRACE_FUNC();
    if (NULL != conductor) {
        conductor->ResetSize(width, height);
    }
}

extern "C" JNIEXPORT void JNICALL Java_com_example_opengles_ShowPngActivity_stop
  (JNIEnv *env, jobject obj) {
    TRACE_FUNC();
    if (NULL != conductor) {
        conductor->Stop();
        delete conductor;
        conductor = NULL;
    }
}
