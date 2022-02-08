#include "ffmpeg_pipeline.h"
//#include "render/audio/openSlEs_pcm_render.h"
//#include "render/video/video_render_interface.h"
#include "render/video/native_window_render.h"
#include "render/video/opengles_render.h"
//#include "ffmpeg/ffmpeg_demuxer.h"
#include "player.h"
#include "utils/log.h"

#ifdef __cplusplus
extern "C" {
#endif
FFmpegPipeline *ffmpeg_pipeline_ptr;
//SLBase *audio_render;

Player *ffmpeg_player;
char *url;
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
        (JNIEnv *env, jobject obj, jstring jurl, jint, jint type) {
    TRACE_FUNC();
    const char *url1 = env->GetStringUTFChars(jurl, nullptr);
    url = (char *) url1;
    ffmpeg_player = new Player(type);
}

JNIEXPORT void JNICALL Java_com_lwl_ffmpegplayer_NativeFFmpegPlayer_Play
        (JNIEnv *env, jobject obj) {
    TRACE_FUNC();
    ffmpeg_player->Start();
}

JNIEXPORT void JNICALL Java_com_lwl_ffmpegplayer_NativeFFmpegPlayer_SeekToPosition
        (JNIEnv *env, jobject obj, jfloat) {}

JNIEXPORT void JNICALL Java_com_lwl_ffmpegplayer_NativeFFmpegPlayer_Pause
        (JNIEnv *env, jobject obj) {}

JNIEXPORT void JNICALL Java_com_lwl_ffmpegplayer_NativeFFmpegPlayer_Stop
        (JNIEnv *env, jobject obj) {}

JNIEXPORT void JNICALL Java_com_lwl_ffmpegplayer_NativeFFmpegPlayer_UnInit
        (JNIEnv *env, jobject obj) {}

JNIEXPORT void JNICALL Java_com_lwl_ffmpegplayer_NativeFFmpegPlayer_OnSurfaceCreated
        (JNIEnv *env, jobject obj, jobject surface) {
    TRACE_FUNC();
    ffmpeg_player->Init(env, obj, surface, url);
}

JNIEXPORT void JNICALL Java_com_lwl_ffmpegplayer_NativeFFmpegPlayer_OnSurfaceChanged
        (JNIEnv *env, jobject obj, jint width, jint height) {
    TRACE_FUNC();
    LOGI("width %d, height %d", width, height);
    if(ffmpeg_player->GetVideoRenderType() > 0) {
        ffmpeg_player->GetOpenglesRender()->OnSurfaceChanged(width, height);
    }
}

JNIEXPORT void JNICALL Java_com_lwl_ffmpegplayer_NativeFFmpegPlayer_OnDrawFrame
        (JNIEnv *env, jobject obj) {
    TRACE_FUNC();
    if(ffmpeg_player->GetVideoRenderType() > 0) {
        ffmpeg_player->GetOpenglesRender()->OnDrawFrame();
    }
}

JNIEXPORT void JNICALL
Java_com_lwl_ffmpegplayer_NativeFFmpegPlayer_SetGesture(JNIEnv *env, jobject thiz,
                                                        jfloat x_rotate_angle,
                                                        jfloat y_rotate_angle, jfloat scale) {
    if(ffmpeg_player->GetVideoRenderType() > 0) {
        LOGE("SetGesture: %f %f %f", x_rotate_angle, y_rotate_angle, scale);
        ffmpeg_player->GetOpenglesRender()->UpdateMVPMatrix(x_rotate_angle, y_rotate_angle, scale,
                                                            scale);
    }
}
JNIEXPORT void JNICALL
Java_com_lwl_ffmpegplayer_NativeFFmpegPlayer_SetTouchLoc(JNIEnv *env, jobject thiz,
                                                         jfloat touch_x, jfloat touch_y) {
    if(ffmpeg_player->GetVideoRenderType() > 0) {
        LOGE("SetTouchLoc: %f %f", touch_x, touch_y);
        ffmpeg_player->GetOpenglesRender()->SetTouchLoc(touch_x, touch_y);
    }
}

#ifdef __cplusplus
}
#endif
