#ifndef FFMPEG_PLAYER_FFMPEG_AUDIO_DECODER_H
#define FFMPEG_PLAYER_FFMPEG_AUDIO_DECODER_H

#include "thread_base.h"
#include "thread_safe_queue.h"
#include "ffmpeg_demuxer.h"
#include "ffmpeg_codec_base.h"

extern "C" {
#include <libavutil/opt.h>
#include <libavutil/channel_layout.h>
#include <libavutil/samplefmt.h>
#include <libavutil/timestamp.h>
#include <libswresample/swresample.h>
};

class AudioFrame {
public:
    AudioFrame(int dataSize) {
        data_size_ = dataSize;
        data_ = static_cast<uint8_t *>(malloc(data_size_));
    }

    ~AudioFrame() {
        if (data_)
            free(data_);
        data_ = nullptr;
    }

    uint8_t *data_ = nullptr;
    int data_size_ = 0;
    double clock_ = 0.0;
};

class FFmpegAudioDecoder : public ThreadBase, CodecBase{
public:
    FFmpegAudioDecoder(FFmpegDemuxer *ffmpeg_demuxer);
    ~FFmpegAudioDecoder();

    bool Init();
    AudioFrame *GetAudioFrame();

    double GetClock() {return clock_;}
private:
    virtual void Process() override;
    virtual int OutputFrame(AVFrame *frame) override;

    enum AVMediaType type_;
    SwrContext *swr_ctx_;
    int dst_nb_samples_;
    int dst_frame_data_size_;
    ThreadSafeQueue<AudioFrame *> audio_frame_queue_;
    FFmpegDemuxer *ffmpeg_demuxer_;

    int audio_frame_count_;
    int audio_packet_count_;
    double clock_;
};

#endif // FFMPEG_PLAYER_FFMPEG_AUDIO_DECODER_H
