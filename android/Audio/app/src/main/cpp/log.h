//
// Created by vpn on 2022/1/17.
//

#ifndef AUDIO_LOG_H
#define AUDIO_LOG_H

#include <android/log.h>
#include <string>

#ifndef LOG_TAG
#define LOG_TAG "AUDIO_JNI"
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG ,__VA_ARGS__) // 定义LOGD类型
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,LOG_TAG ,__VA_ARGS__) // 定义LOGI类型
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN,LOG_TAG ,__VA_ARGS__) // 定义LOGW类型
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,LOG_TAG ,__VA_ARGS__) // 定义LOGE类型
#define LOGF(...) __android_log_print(ANDROID_LOG_FATAL,LOG_TAG ,__VA_ARGS__) // 定义LOGF类型
#endif

class TraceFunc {
public:
    explicit TraceFunc(std::string function, std::string file, int line)
            : function_(std::move(function)), file_(std::move(file)), line_(line) {
        LOGI("%s %d [Trace] %s : E <<<",file_.c_str(),line_,function_.c_str());
    }
    virtual ~TraceFunc() {
        LOGI("%s %d [Trace] %s : X >>>",file_.c_str(),line_,function_.c_str());
    }

private:
    std::string function_;
    std::string file_;
    int line_;
};  // TraceFunc


#define TRACE_FUNC() TraceFunc trace_func(__FUNCTION__, __FILE__, __LINE__)

#endif //AUDIO_LOG_H
