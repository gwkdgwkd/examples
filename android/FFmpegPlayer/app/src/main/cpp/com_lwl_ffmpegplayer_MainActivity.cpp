#include <jni.h>
#include <string>

#include "wapped_ffmpeg.h"
#include "log.h"

WappedFFmpeg *wappedFFmpegPtr;

extern "C" JNIEXPORT jstring JNICALL
Java_com_lwl_ffmpegplayer_MainActivity_GetFFmpegVersion(
        JNIEnv* env,
        jobject /* this */) {
    TRACE_FUNC();
    wappedFFmpegPtr = new WappedFFmpeg();
    std::string ret = wappedFFmpegPtr->GetFFmpegVersion();

    return env->NewStringUTF(ret.c_str());
}