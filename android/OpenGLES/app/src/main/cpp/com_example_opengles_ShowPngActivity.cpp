#include "com_example_opengles_ShowPngActivity.h"

#include <string>
#include "show_png_conductor.h"
#include "log.h"

extern "C" JNIEXPORT void JNICALL Java_com_example_opengles_ShowPngActivity_init
  (JNIEnv *, jobject, jstring) {
    TRACE_FUNC();

}

extern "C" JNIEXPORT void JNICALL Java_com_example_opengles_ShowPngActivity_setSurface
  (JNIEnv *, jobject, jobject){
    TRACE_FUNC();
}

extern "C" JNIEXPORT void JNICALL Java_com_example_opengles_ShowPngActivity_resetSize
  (JNIEnv *, jobject, jint, jint) {
    TRACE_FUNC();
}

extern "C" JNIEXPORT void JNICALL Java_com_example_opengles_ShowPngActivity_stop
  (JNIEnv *, jobject) {
    TRACE_FUNC();
}
