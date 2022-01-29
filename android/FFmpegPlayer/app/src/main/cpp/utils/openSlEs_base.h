#ifndef OPENGLES_UTILS_AUDIO_OPENSLES_BASE_H
#define OPENGLES_UTILS_AUDIO_OPENSLES_BASE_H

#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>

class SLBase {
public:
    SLBase();
    virtual ~SLBase();

    void Start();
    void Stop();
    void SetQueueCallBack(slAndroidSimpleBufferQueueCallback callback);
    bool SendQueueLoop(const void *pBuffer, SLuint32 size);
    bool IsQueueLooping() const { return isQueueLooping; };
    bool IsQueueSelf(SLAndroidSimpleBufferQueueItf queue);

protected:
    // 引擎
    bool CreateEngine();                    // 创建引擎
    void ReleaseEngine();                   // 释放引擎

    // 输入输出
    SLDataSource dataSource;
    SLDataSink dataSink;

    virtual bool SetDataSource() = 0;         // 设置输入源
    virtual bool SetDataSink() = 0;           // 设置输出源

    // 播放器或者录音器
    virtual bool CreateFeature() = 0;         // 创建XXX器
    virtual void ReleaseFeature() = 0;        // 释放XXX器
    virtual bool SetStateRuning() = 0;        // 设置XXX器运行状态
    virtual bool SetStateStoping() = 0;       // 设置XXX器停止状态

    // 队列
    void SetQueueState(bool isLoop);

    bool isQueueLooping = false;
    SLAndroidSimpleBufferQueueItf queueItf;
    slAndroidSimpleBufferQueueCallback queueCallBack;

    SLObjectItf engineObject = nullptr;
    SLEngineItf engineEngine = nullptr;

};

#endif // OPENGLES_UTILS_AUDIO_OPENSLES_BASE_H
