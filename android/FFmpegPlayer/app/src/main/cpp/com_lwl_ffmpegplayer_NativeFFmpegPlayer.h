/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class com_lwl_ffmpegplayer_NativeFFmpegPlayer */

#ifndef _Included_com_lwl_ffmpegplayer_NativeFFmpegPlayer
#define _Included_com_lwl_ffmpegplayer_NativeFFmpegPlayer
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     com_lwl_ffmpegplayer_NativeFFmpegPlayer
 * Method:    GetFFmpegVersion
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_lwl_ffmpegplayer_NativeFFmpegPlayer_GetFFmpegVersion
  (JNIEnv *, jobject);

/*
 * Class:     com_lwl_ffmpegplayer_NativeFFmpegPlayer
 * Method:    Init
 * Signature: (Ljava/lang/String;II)V
 */
JNIEXPORT void JNICALL Java_com_lwl_ffmpegplayer_NativeFFmpegPlayer_Init
  (JNIEnv *, jobject, jstring, jint, jint);

/*
 * Class:     com_lwl_ffmpegplayer_NativeFFmpegPlayer
 * Method:    Play
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_lwl_ffmpegplayer_NativeFFmpegPlayer_Play
  (JNIEnv *, jobject);

/*
 * Class:     com_lwl_ffmpegplayer_NativeFFmpegPlayer
 * Method:    SeekToPosition
 * Signature: (F)V
 */
JNIEXPORT void JNICALL Java_com_lwl_ffmpegplayer_NativeFFmpegPlayer_SeekToPosition
  (JNIEnv *, jobject, jfloat);

/*
 * Class:     com_lwl_ffmpegplayer_NativeFFmpegPlayer
 * Method:    Pause
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_lwl_ffmpegplayer_NativeFFmpegPlayer_Pause
  (JNIEnv *, jobject);

/*
 * Class:     com_lwl_ffmpegplayer_NativeFFmpegPlayer
 * Method:    Stop
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_lwl_ffmpegplayer_NativeFFmpegPlayer_Stop
  (JNIEnv *, jobject);

/*
 * Class:     com_lwl_ffmpegplayer_NativeFFmpegPlayer
 * Method:    UnInit
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_lwl_ffmpegplayer_NativeFFmpegPlayer_UnInit
  (JNIEnv *, jobject);

#ifdef __cplusplus
}
#endif
#endif