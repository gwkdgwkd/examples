#include "com_lwl_ffmpegplayer_NativeFFmpegPlayer.h"

#include "ffmpeg_pipeline.h"
#include "log.h"

#ifdef __cplusplus
extern "C" {
#endif
FFmpegPipeline *ffmpeg_pipeline_ptr;
JNIEXPORT jstring JNICALL Java_com_lwl_ffmpegplayer_NativeFFmpegPlayer_GetFFmpegVersion
  (JNIEnv *env, jobject obj) {
//  TRACE_FUNC();
//  ffmpeg_pipeline_ptr = new FFmpegPipeline();
//  std::string ret = ffmpeg_pipeline_ptr->GetFFmpegVersion();
//
//  return env->NewStringUTF(ret.c_str());
return env->NewStringUTF("");
}

JNIEXPORT void JNICALL Java_com_lwl_ffmpegplayer_NativeFFmpegPlayer_Init
  (JNIEnv *env, jobject obj, jstring, jint, jint) {}

JNIEXPORT void JNICALL Java_com_lwl_ffmpegplayer_NativeFFmpegPlayer_Play
  (JNIEnv *env, jobject obj) {}

JNIEXPORT void JNICALL Java_com_lwl_ffmpegplayer_NativeFFmpegPlayer_SeekToPosition
  (JNIEnv *env, jobject obj, jfloat)  {}

JNIEXPORT void JNICALL Java_com_lwl_ffmpegplayer_NativeFFmpegPlayer_Pause
  (JNIEnv *env, jobject obj)  {}

JNIEXPORT void JNICALL Java_com_lwl_ffmpegplayer_NativeFFmpegPlayer_Stop
  (JNIEnv *env, jobject obj)  {}

JNIEXPORT void JNICALL Java_com_lwl_ffmpegplayer_NativeFFmpegPlayer_UnInit
  (JNIEnv *env, jobject obj)  {}

JNIEXPORT void JNICALL Java_com_lwl_ffmpegplayer_NativeFFmpegPlayer_OnSurfaceCreated
  (JNIEnv *env, jobject obj, jobject surface)  {}

JNIEXPORT void JNICALL Java_com_lwl_ffmpegplayer_NativeFFmpegPlayer_OnSurfaceChanged
  (JNIEnv *env, jobject obj, jint width, jint height)  {}

JNIEXPORT void JNICALL Java_com_lwl_ffmpegplayer_NativeFFmpegPlayer_OnDrawFrame
  (JNIEnv *env, jobject obj)  {}

#ifdef __cplusplus
}
#endif