#include "com_lwl_ffmpegplayer_NativeFFmpegPlayer.h"

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

void PlayerCallback(SLAndroidSimpleBufferQueueItf bq, void *context) {
//    if (audio_render == nullptr) return;
//    if (!audio_render->IsQueueSelf(bq)) return;
//    if (audio_render->IsQueueLooping()) {
//        AudioFrame *frame = ffmpeg_pipeline_ptr->GetAudioFrame();
//        if (frame) {
//            audio_render->SendQueueLoop(frame->data_, frame->data_size_);
//        }
//    }
}

JNIEXPORT void JNICALL Java_com_lwl_ffmpegplayer_NativeFFmpegPlayer_Init
        (JNIEnv *env, jobject obj, jstring jurl, jint, jint) {
    TRACE_FUNC();
    const char *url1 = env->GetStringUTFChars(jurl, nullptr);
    url = (char *) url1;
//    ffmpeg_pipeline_ptr = new FFmpegPipeline(url);
//    ffmpeg_pipeline_ptr->Init();
//    ffmpeg_pipeline_ptr->Start();
//
//    audio_render = new SLRender();
//    audio_render->SetQueueCallBack(PlayerCallback);

//    ffmpeg_player = new Player();
//    ffmpeg_player->Init(url);
}

JNIEXPORT void JNICALL Java_com_lwl_ffmpegplayer_NativeFFmpegPlayer_Play
        (JNIEnv *env, jobject obj) {
    TRACE_FUNC();
//    ffmpeg_pipeline_ptr->Start();
//    audio_render->Start();
//    audio_render->SendQueueLoop("", 1);    // 开启轮询
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
//    native_window_render = new NativeWindowRender(env, surface);
//    int w, h;
//    ffmpeg_pipeline_ptr->GetVideoWidthAndHeight(&w, &h);
//    int dstSize[2] = {0};
//    native_window_render->Init(w, h, dstSize);
//    ffmpeg_pipeline_ptr->InitSwscale(0, 0, AV_PIX_FMT_RGBA, dstSize[0], dstSize[1],
//                                     AV_PIX_FMT_RGBA);
//    ffmpeg_pipeline_ptr->SetVideoRander(native_window_render);


//    ffmpeg_player->Init(url, native_window_render);
    ffmpeg_player = new Player();
    ffmpeg_player->Init(env, obj, surface, url);
}

JNIEXPORT void JNICALL Java_com_lwl_ffmpegplayer_NativeFFmpegPlayer_OnSurfaceChanged
        (JNIEnv *env, jobject obj, jint width, jint height) {
    TRACE_FUNC();
    LOGI("width %d, height %d", width, height);
    ffmpeg_player->GetOpenglesRender()->OnSurfaceChanged(width, height);
}

JNIEXPORT void JNICALL Java_com_lwl_ffmpegplayer_NativeFFmpegPlayer_OnDrawFrame
        (JNIEnv *env, jobject obj) {}

JNIEXPORT void JNICALL
Java_com_lwl_ffmpegplayer_NativeFFmpegPlayer_SetGesture(JNIEnv *env, jobject thiz,
                                                        jfloat x_rotate_angle,
                                                        jfloat y_rotate_angle, jfloat scale) {
    LOGE("SetGesture: %f %f %f", x_rotate_angle, y_rotate_angle, scale);
    ffmpeg_player->GetOpenglesRender()->UpdateMVPMatrix(x_rotate_angle, y_rotate_angle, scale, scale);
}
JNIEXPORT void JNICALL
Java_com_lwl_ffmpegplayer_NativeFFmpegPlayer_SetTouchLoc(JNIEnv *env, jobject thiz,
                                                         jfloat touch_x, jfloat touch_y) {
    LOGE("SetTouchLoc: %f %f", touch_x, touch_y);
    ffmpeg_player->GetOpenglesRender()->SetTouchLoc(touch_x, touch_y);
}

#ifdef __cplusplus
}
#endif
