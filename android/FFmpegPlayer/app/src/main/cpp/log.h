#ifndef OPENGLES_LOG_H
#define OPENGLES_LOG_H

#include <android/log.h>
#include <string>

#ifndef LOG_TAG
#define LOG_TAG "FFMPGE_PLAYER_JNI"
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG ,__VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,LOG_TAG ,__VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN,LOG_TAG ,__VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,LOG_TAG ,__VA_ARGS__)
#define LOGF(...) __android_log_print(ANDROID_LOG_FATAL,LOG_TAG ,__VA_ARGS__)
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

#endif //OPENGLES_LOG_H
