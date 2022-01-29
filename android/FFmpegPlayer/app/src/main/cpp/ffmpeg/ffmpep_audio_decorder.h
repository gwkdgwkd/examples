#ifndef FFMPEG_PLAYER_FFMPEP_AUDIO_DECORDER_H
#define FFMPEG_PLAYER_FFMPEP_AUDIO_DECORDER_H

#include "thread_base.h"
#include "thread_safe_queue.h"

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavutil/opt.h>
#include <libavutil/channel_layout.h>
#include <libavutil/samplefmt.h>
#include <libswresample/swresample.h>
};

class AudioFrame1 {
public:
    AudioFrame1(int dataSize) {
        data_size_ = dataSize;
        data_ = static_cast<uint8_t *>(malloc(data_size_));
    }

    ~AudioFrame1() {
        if (data_)
            free(data_);
        data_ = nullptr;
    }

    uint8_t *data_ = nullptr;
    int data_size_ = 0;
};

class FFmpegAudioDecorder : public ThreadBase{
public:
    FFmpegAudioDecorder(AVCodecContext *audio_dec_ctx_);
    ~FFmpegAudioDecorder();

    bool Init();
    AVPacket * GetAudioPacket();
private:
    virtual void Process() override;


    AVCodecContext *audio_dec_ctx_;
    SwrContext *swr_ctx_;
    int dst_nb_samples_;
    int dst_frame_data_size_;
    ThreadSafeQueue<AudioFrame1 *> audio_frame_queue_;
};


#endif //FFMPEG_PLAYER_FFMPEP_AUDIO_DECORDER_H
