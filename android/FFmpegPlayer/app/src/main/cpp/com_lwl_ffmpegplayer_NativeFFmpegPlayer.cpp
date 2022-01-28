#include "com_lwl_ffmpegplayer_NativeFFmpegPlayer.h"

#include "ffmpeg_pipeline.h"
#include "openSlEs_pcm_render.h"
#include "utils/log.h"

#ifdef __cplusplus
extern "C" {
#endif
FFmpegPipeline *ffmpeg_pipeline_ptr;
SLBase *audio_render;
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
    TRACE_FUNC();
    if (audio_render == nullptr) return;
    if (!audio_render->IsQueueSelf(bq)) return;
    if (audio_render->IsQueueLooping()) {
        AudioFrame *frame = ffmpeg_pipeline_ptr->GetAudioFrame();
        if (frame) {
            audio_render->SendQueueLoop(frame->data_, frame->data_size_);
        }
    }
}

JNIEXPORT void JNICALL Java_com_lwl_ffmpegplayer_NativeFFmpegPlayer_Init
        (JNIEnv *env, jobject obj, jstring jurl, jint, jint) {
    const char *url = env->GetStringUTFChars(jurl, nullptr);
    ffmpeg_pipeline_ptr = new FFmpegPipeline(url);
    ffmpeg_pipeline_ptr->Init();

    audio_render = new SLRender();
    audio_render->SetQueueCallBack(PlayerCallback);
    audio_render->Start();
    audio_render->SendQueueLoop("", 1);    // 开启轮询

}

JNIEXPORT void JNICALL Java_com_lwl_ffmpegplayer_NativeFFmpegPlayer_Play
        (JNIEnv *env, jobject obj) {}

JNIEXPORT void JNICALL Java_com_lwl_ffmpegplayer_NativeFFmpegPlayer_SeekToPosition
        (JNIEnv *env, jobject obj, jfloat) {}

JNIEXPORT void JNICALL Java_com_lwl_ffmpegplayer_NativeFFmpegPlayer_Pause
        (JNIEnv *env, jobject obj) {}

JNIEXPORT void JNICALL Java_com_lwl_ffmpegplayer_NativeFFmpegPlayer_Stop
        (JNIEnv *env, jobject obj) {}

JNIEXPORT void JNICALL Java_com_lwl_ffmpegplayer_NativeFFmpegPlayer_UnInit
        (JNIEnv *env, jobject obj) {}

JNIEXPORT void JNICALL Java_com_lwl_ffmpegplayer_NativeFFmpegPlayer_OnSurfaceCreated
        (JNIEnv *env, jobject obj, jobject surface) {}

JNIEXPORT void JNICALL Java_com_lwl_ffmpegplayer_NativeFFmpegPlayer_OnSurfaceChanged
        (JNIEnv *env, jobject obj, jint width, jint height) {}

JNIEXPORT void JNICALL Java_com_lwl_ffmpegplayer_NativeFFmpegPlayer_OnDrawFrame
        (JNIEnv *env, jobject obj) {}

#ifdef __cplusplus
}
#endif