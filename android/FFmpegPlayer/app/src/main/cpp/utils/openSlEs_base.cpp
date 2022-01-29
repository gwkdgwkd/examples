#include "openSlEs_base.h"
#include "log.h"

SLBase::SLBase() {
    TRACE_FUNC();
}
SLBase::~SLBase() {
    TRACE_FUNC();
    ReleaseEngine();
}

// 设置回调方法
void SLBase::SetQueueCallBack(slAndroidSimpleBufferQueueCallback callback) {
    queueCallBack = callback;
}

/**
 * 设置队列状态
 * @param isLoop true正在轮询状态
 */
void SLBase::SetQueueState(bool isLoop) {
    TRACE_FUNC();
    isQueueLooping = isLoop;
}

// 像队列发送数据并轮询
bool SLBase::SendQueueLoop(const void *pBuffer, SLuint32 size) {
    if(!isQueueLooping) return false;
    SLresult result = (*queueItf)->Enqueue(queueItf, pBuffer,size);
    if(result!=SL_RESULT_SUCCESS) return false;

    return true;
}

/**
 * 检查回调函数回来的队列是否是自己
 * @param queue 因为SLAndroidSimpleBufferQueueItf已经实现了==运算符重载，因此不需要来比较是否为同一个对象(即不需要比较他们的地址是否相等，已经验证他们的地址不相等)
 * @return
 */
bool SLBase::IsQueueSelf(SLAndroidSimpleBufferQueueItf queue) {
    if(queue == queueItf)
        return true;
    else
        return false;
}

bool SLBase::CreateEngine() {
    TRACE_FUNC();
    SLresult result;
    result = slCreateEngine(&engineObject, 0, nullptr, 0, nullptr, nullptr);
    if(result != SL_RESULT_SUCCESS) return false;
    result = (*engineObject)->Realize(engineObject, SL_BOOLEAN_FALSE);
    if(result != SL_RESULT_SUCCESS) return false;
    result = (*engineObject)->GetInterface(engineObject, SL_IID_ENGINE, &engineEngine);
    if(result != SL_RESULT_SUCCESS) return false;
    return true;
}

void SLBase::ReleaseEngine() {
    TRACE_FUNC();
    engineEngine = nullptr;
    if (engineObject != nullptr) {
        (*engineObject)->Destroy(engineObject);
        engineObject = nullptr;
    }
}

void SLBase::Start() {
    TRACE_FUNC();
    // 创建引擎
    bool isSucced = CreateEngine();
    if(!isSucced) return;
    // 设置输入源
    isSucced = SetDataSource();
    if(!isSucced) return;
    // 设置输出源
    isSucced = SetDataSink();
    if(!isSucced) return;
    // 创建播放器或者录音器
    isSucced = CreateFeature();
    if(!isSucced) return;
    // 设置运行状态
    isSucced = SetStateRuning();
    if(!isSucced) return;
    // 开启轮询 注意：最后需要手动调用SendQueueLoop方法来开启轮询
    SetQueueState(true);
}

void SLBase::Stop() {
    TRACE_FUNC();
    LOGE("SLBase-Stop");
    // 停止轮询
    SetQueueState(false);
    LOGE("SLBase-停止轮询");
    SetStateStoping();
    LOGE("SLBase-SetStateStoping");
    ReleaseFeature();
    LOGE("SLBase-ReleaseFeature");
    ReleaseEngine();
    LOGE("SLBase-ReleaseEngine");
}
