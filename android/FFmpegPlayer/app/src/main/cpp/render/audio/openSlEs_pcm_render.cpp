#include <cstdio>
#include "openSlEs_pcm_render.h"
#include "log.h"

SLRender::SLRender() : SLBase(){
    TRACE_FUNC();
}
SLRender::~SLRender() {
    TRACE_FUNC();
    ReleaseFeature();
}

bool SLRender::SetDataSource() {
    TRACE_FUNC();
    static SLDataLocator_AndroidSimpleBufferQueue loc_bufq = {SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE, 4};
    static SLDataFormat_PCM format_pcm = {SL_DATAFORMAT_PCM, // 播放pcm格式的数据
                                          2, // 2个声道（立体声）
                                          SL_SAMPLINGRATE_44_1, // 44100hz的频率
                                          SL_PCMSAMPLEFORMAT_FIXED_16, // 位数16位
                                          SL_PCMSAMPLEFORMAT_FIXED_16, // 和位数一致就行
                                          SL_SPEAKER_FRONT_LEFT|SL_SPEAKER_FRONT_RIGHT,// 声道,立体声（前左前右)
                                          SL_BYTEORDER_LITTLEENDIAN  // 结束标志,字节序,小端
    };
    // 注意loc_bufq和format_pcm不用static修饰在该函数结束就会被释放
    dataSource = {&loc_bufq, &format_pcm};
    return true;
}

bool SLRender::SetDataSink() {
    TRACE_FUNC();
    SLresult result;
    const SLInterfaceID ids[1] = {SL_IID_ENVIRONMENTALREVERB};
    const SLboolean req[1] = {SL_BOOLEAN_FALSE};
    result = (*engineEngine)->CreateOutputMix(engineEngine, &outMixObject, 1, ids, req);
    if(SL_RESULT_SUCCESS != result)
        return false;
    result = (*outMixObject)->Realize(outMixObject, SL_BOOLEAN_FALSE);
    if(SL_RESULT_SUCCESS != result)
        return false;

    static SLDataLocator_OutputMix loc_outmix = {SL_DATALOCATOR_OUTPUTMIX, outMixObject};
    // 注意loc_outmix不用static修饰在该函数结束就会被释放
    dataSink = {&loc_outmix, nullptr};
    return true;
}

bool SLRender::CreateFeature() {
    TRACE_FUNC();
    SLresult result;
    const int length = 2;
    const SLInterfaceID ids[length] = {SL_IID_BUFFERQUEUE, SL_IID_VOLUME};
    const SLboolean req[length] = {SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE};
    result = (*engineEngine)->CreateAudioPlayer(engineEngine, &playerObject, &dataSource, &dataSink,length, ids, req);
    if (SL_RESULT_SUCCESS != result)
        return false;
    result = (*playerObject)->Realize(playerObject, SL_BOOLEAN_FALSE);
    if (SL_RESULT_SUCCESS != result)
        return false;
    result = (*playerObject)->GetInterface(playerObject, SL_IID_PLAY, &playerItf);
    if (SL_RESULT_SUCCESS != result)
        return false;
    result = (*playerObject)->GetInterface(playerObject, SL_IID_BUFFERQUEUE, &queueItf);
    if (SL_RESULT_SUCCESS != result)
        return false;
    if (queueCallBack == nullptr)
        return true;
    result = (*queueItf)->RegisterCallback(queueItf, queueCallBack, nullptr);
    if (SL_RESULT_SUCCESS != result)
        return false;
    LOGE("SLPlayer：CreateFeature 回调函数注册成功");
    return true;
}

void SLRender::ReleaseFeature() {
    TRACE_FUNC();
    if(playerObject != nullptr) {
        (*playerObject)->Destroy(playerObject);
        playerObject = nullptr;
        playerItf = nullptr;
        queueItf = nullptr;
    }
    if(outMixObject != nullptr){
        (*outMixObject)->Destroy(outMixObject);
        outMixObject = nullptr;
    }
}

bool SLRender::SetStateRuning() {
    TRACE_FUNC();
    SLresult result = (*playerItf)->SetPlayState(playerItf,SL_PLAYSTATE_PLAYING);
    if(result!=SL_RESULT_SUCCESS)
        return false;
    return true;
}

bool SLRender::SetStateStoping() {
    TRACE_FUNC();
    SLresult result = (*playerItf)->SetPlayState(playerItf,SL_PLAYSTATE_STOPPED);
    if(result!=SL_RESULT_SUCCESS)
        return false;
    return true;
}

