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
//FFmpegPipeline *ffmpeg_pipeline_ptr;
//SLBase *audio_render;

Player *ffmpeg_player = nullptr;
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
        (JNIEnv *env, jobject obj, jstring jurl, jint view_type,
         jint audio_render_type, jint video_render_type ,jint effect_type, jint scale_type) {
    TRACE_FUNC();
    const char *url = env->GetStringUTFChars(jurl, nullptr);
    ffmpeg_player = new Player(view_type, audio_render_type, video_render_type, effect_type, scale_type);
    ffmpeg_player->Init(url);
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

VisualAudioRender *visual_audio_render;
JNIEXPORT void JNICALL Java_com_lwl_ffmpegplayer_NativeFFmpegPlayer_OnSurfaceCreated
        (JNIEnv *env, jobject obj, jobject surface, jint type) {
    TRACE_FUNC();
    if (type == 0) {
        ffmpeg_player->Init2(env, obj, surface);
    } else {
        while (!(ffmpeg_player && ffmpeg_player->GetInitStatus())) {
            usleep(10);
        }
        visual_audio_render = new VisualAudioRender();
        visual_audio_render->OnSurfaceCreated();
        ffmpeg_player->SetVisualAudioRender(visual_audio_render);
    }
}

JNIEXPORT void JNICALL Java_com_lwl_ffmpegplayer_NativeFFmpegPlayer_OnSurfaceChanged
        (JNIEnv *env, jobject obj, jint width, jint height, jint type) {
    TRACE_FUNC();
    LOGI("width %d, height %d, type %d", width, height, type);
    if (ffmpeg_player && ffmpeg_player->GetVideoRenderType() > 0) {
        if (type == 1) {
            visual_audio_render->OnSurfaceChanged(width, height);
        } else {
            ffmpeg_player->GetOpenglesRender()->OnSurfaceChanged(width, height);
        }
    }
}

JNIEXPORT void JNICALL Java_com_lwl_ffmpegplayer_NativeFFmpegPlayer_OnDrawFrame
        (JNIEnv *env, jobject obj, jint type) {
    // TRACE_FUNC();
    if (ffmpeg_player && ffmpeg_player->GetVideoRenderType() > 0) {
        if (type == 1) {
            visual_audio_render->OnDrawFrame();
        } else {
            ffmpeg_player->GetOpenglesRender()->OnDrawFrame();
        }
    }
}

JNIEXPORT void JNICALL
Java_com_lwl_ffmpegplayer_NativeFFmpegPlayer_SetGesture(JNIEnv *env, jobject thiz,
                                                        jfloat x_rotate_angle,
                                                        jfloat y_rotate_angle, jfloat scale) {
    if (ffmpeg_player->GetVideoRenderType() > 0) {
        LOGE("SetGesture: %f %f %f", x_rotate_angle, y_rotate_angle, scale);
        ffmpeg_player->GetOpenglesRender()->UpdateMVPMatrix(x_rotate_angle, y_rotate_angle, scale,
                                                            scale);
    }
}
JNIEXPORT void JNICALL
Java_com_lwl_ffmpegplayer_NativeFFmpegPlayer_SetTouchLoc(JNIEnv *env, jobject thiz,
                                                         jfloat touch_x, jfloat touch_y) {
    if (ffmpeg_player->GetVideoRenderType() > 0) {
        LOGE("SetTouchLoc: %f %f", touch_x, touch_y);
        ffmpeg_player->GetOpenglesRender()->SetTouchLoc(touch_x, touch_y);
    }
}

JNIEXPORT void JNICALL
Java_com_lwl_ffmpegplayer_NativeFFmpegPlayer_GetMediaInfo(JNIEnv *env, jobject obj,
                                                          jobject mediainfo) {
    jclass clazz = env->GetObjectClass(mediainfo);
    jfieldID fid;

    MediaInfo info;
    ffmpeg_player->GetDemuxer()->GetMediaInfo(&info);
    ffmpeg_player->media_info_ = info;

    fid = env->GetFieldID(clazz, "sample_rate", "I");
    env->SetIntField(mediainfo, fid, info.sample_fmt);

    fid = env->GetFieldID(clazz, "channels", "I");
    env->SetIntField(mediainfo, fid, info.channels);

    fid = env->GetFieldID(clazz, "sample_rate", "I");
    env->SetIntField(mediainfo, fid, info.sample_rate);

    fid = env->GetFieldID(clazz, "width", "I");
    env->SetIntField(mediainfo, fid, info.width);

    fid = env->GetFieldID(clazz, "height", "I");
    env->SetIntField(mediainfo, fid, info.height);

    fid = env->GetFieldID(clazz, "duration", "D");
    env->SetDoubleField(mediainfo, fid, info.duration);


    return;
}

#ifdef __cplusplus
}
#endif
