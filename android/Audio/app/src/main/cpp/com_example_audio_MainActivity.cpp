#include "com_example_audio_MainActivity.h"

#include <string>
#include "openSlEs_base.h"
#include "openSlEs_pcm_player.h"
#include "openSlEs_pcm_recorder.h"
#include "log.h"
#define BUF_SIZE (1024*4)
#define PLAY_FILE_PATH "/storage/emulated/0/Download/WeiXin/audio.pcm"
#define RECORDER_FILE_PATH "/storage/emulated/0/Download/WeiXin/audio1.pcm"
FILE *readFp = nullptr;     // 播放器用到的读取文件
FILE *writeFp = nullptr;    // 录音器用到的写入文件
char *readBuf = nullptr;    // 读取缓存
char *writeBuf = nullptr;   // 写入缓存
SLBase *player;             // 播放器
SLBase *recorder;           // 录音器

void initPlayerResource(){
    TRACE_FUNC();
    if(readFp == nullptr) {
        if((readFp = fopen(PLAY_FILE_PATH, "rb")) == nullptr) {
            LOGE("open file failed : %d ,%s",errno,strerror(errno));
        }
    }

    if(readBuf == nullptr) readBuf = new char[BUF_SIZE];
}

void initRecorderResource(){
    TRACE_FUNC();
    if(!writeFp) {
        if((writeFp = fopen(RECORDER_FILE_PATH, "wb+")) == nullptr) {
            LOGE("open file failed : %d ,%s",errno,strerror(errno));
        }
    }

    if(writeBuf == nullptr) writeBuf = new char[BUF_SIZE];
}

void freePlayerResource(){
    TRACE_FUNC();
    if(readFp != nullptr) {
        fclose(readFp);
        readFp = nullptr;
    }
    if(readBuf != nullptr){
        delete readBuf;
        readBuf = nullptr;
    }
}

void freeRecorderResource(){
    TRACE_FUNC();
    if(writeFp != nullptr) {
        fclose(writeFp);
        writeFp = nullptr;
    }
    if(writeBuf != nullptr){
        delete writeBuf;
        writeBuf = nullptr;
    }
}

void PlayerCallback(SLAndroidSimpleBufferQueueItf bq, void *context) {
    if(readFp == nullptr) return;
    if (player == nullptr) return;
    if(!player->IsQueueSelf(bq)) return;
    if(player->IsQueueLooping()){
        int len = fread(readBuf,1,BUF_SIZE,readFp);
        LOGD("PlayerCallback:len=%d",len);
        if (len > 0) player->SendQueueLoop(readBuf,len);
    }
}
void RecorderCallback(SLAndroidSimpleBufferQueueItf bq, void *context) {
    if(!writeFp){
        LOGE("RecorderCallback:文件打开失败");
        return;
    }
    if (recorder == nullptr) {
        LOGE("RecorderCallback:recorder是空");
        return;
    }
    if(!recorder->IsQueueSelf(bq)) {    // 检查是否是自己
        LOGE("RecorderCallback:队列不是自己");
        return;
    }
    if(recorder->IsQueueLooping()){
        fwrite(writeBuf,1,BUF_SIZE,writeFp);
        fflush(writeFp);
        recorder->SendQueueLoop(writeBuf,BUF_SIZE);
    }
}

extern "C" JNIEXPORT void JNICALL Java_com_example_audio_MainActivity_startPlayPcm
(JNIEnv *, jobject) {
    TRACE_FUNC();
    initPlayerResource();
    player = new SLPlayer();
    player->SetQueueCallBack(PlayerCallback);
    player->Start();
    player->SendQueueLoop("",1);    // 开启轮询
}

extern "C" JNIEXPORT void JNICALL Java_com_example_audio_MainActivity_stopPlayPcm
(JNIEnv *, jobject) {
    TRACE_FUNC();
    player->Stop();
    freePlayerResource();
    delete player;
}

extern "C" JNIEXPORT void JNICALL Java_com_example_audio_MainActivity_startRecorderPcm
(JNIEnv *, jobject) {
    TRACE_FUNC();
    initRecorderResource();
    recorder = new SLRecorder();
    recorder->SetQueueCallBack(RecorderCallback);
    recorder->Start();
    recorder->SendQueueLoop(writeBuf,BUF_SIZE); // 因为开启轮询的方式依赖外部缓存，因此提供了一个函数让外部调用
}

extern "C" JNIEXPORT void JNICALL Java_com_example_audio_MainActivity_stopRecorderPcm
(JNIEnv *, jobject) {
    TRACE_FUNC();
    recorder->Stop();
    freeRecorderResource();
    delete recorder;
}