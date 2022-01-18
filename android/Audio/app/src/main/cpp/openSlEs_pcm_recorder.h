#ifndef AUDIO_OPENSLES_PCM_RECORDER_H
#define AUDIO_OPENSLES_PCM_RECORDER_H
#include "openSlEs_base.h"

class SLRecorder : public SLBase{
public:
    SLRecorder();
    virtual ~SLRecorder();

protected:
    SLObjectItf recorderObject;
    SLRecordItf recorderItf;

    virtual bool SetDataSource();           // 设置输入源
    virtual bool SetDataSink();             // 设置输出源
    virtual bool CreateFeature();           // 创建XXX器
    virtual void ReleaseFeature();          // 释放XXX器
    virtual bool SetStateRuning();          // 设置XXX器运行状态
    virtual bool SetStateStoping();         // 设置XXX器停止状态
};

#endif //AUDIO_OPENSLES_PCM_RECORDER_H
