#ifndef FFMPEG_PLAYER_FFMPEG_PIPELINE_H
#define FFMPEG_PLAYER_FFMPEG_PIPELINE_H

#include <string>
#include <thread>
#include <mutex>
#include <condition_variable>

#include "thread_safe_queue.h"

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/opt.h>
#include <libavutil/channel_layout.h>
#include <libavutil/samplefmt.h>
#include <libswresample/swresample.h>

#include <libavcodec/version.h>
#include <libavformat/version.h>
#include <libavutil/version.h>
#include <libavfilter/version.h>
#include <libswresample/version.h>
#include <libswscale/version.h>
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
};


class FFmpegPipeline {
public:
    FFmpegPipeline(const char *url);
    ~FFmpegPipeline();

    std::string GetFFmpegVersion();
    bool Init();

    AudioFrame* GetAudioFrame();

private:
    bool OpenCodecContext(int *stream_idx,
                          AVCodecContext **dec_ctx, AVFormatContext *fmt_ctx,
                          enum AVMediaType type);
    int DecodePacket(AVCodecContext *dec, const AVPacket *pkt);
    void DecodeThread();
    int OutputAudioFrame(AVFrame *frame);
    int OutputVideoFrame(AVFrame *frame);

    bool InitSwscale(int64_t src_ch_layout, int src_rate, enum AVSampleFormat src_sample_fmt,
                     int src_nb_samples, int64_t dst_ch_layout, int dst_rate,
                     enum AVSampleFormat dst_sample_fmt);

    // ffmpeg
    std::string url_;
    AVFormatContext *fmt_ctx_;
    int video_stream_idx_;
    int audio_stream_idx_;
    AVCodecContext *video_dec_ctx_;
    AVCodecContext *audio_dec_ctx_;
    AVStream *video_stream_;
    AVStream *audio_stream_;
    AVFrame *frame_;
    AVPacket *packet_;

    SwrContext *swr_ctx_;
    int dst_nb_samples_;
    int dst_frame_data_size_;
    ThreadSafeQueue<AudioFrame *> audio_frame_queue_;

    // thread
    std::mutex mutex_;
    std::condition_variable condition_variable_;
    std::thread *thread_;
};

#endif //FFMPEG_PLAYER_FFMPEG_PIPELINE_H
