#ifndef FFMPEG_PLAYER_FFMPEG_PIPELINE_H
#define FFMPEG_PLAYER_FFMPEG_PIPELINE_H

#include <string>
#include <thread>
#include <mutex>
#include <condition_variable>

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>

#include <libavcodec/version.h>
#include <libavformat/version.h>
#include <libavutil/version.h>
#include <libavfilter/version.h>
#include <libswresample/version.h>
#include <libswscale/version.h>
};

class FFmpegPipeline {
public:
    FFmpegPipeline(const char *url);

    ~FFmpegPipeline();

    std::string GetFFmpegVersion();

    bool Init();

private:
    bool OpenCodecContext(int *stream_idx,
                          AVCodecContext **dec_ctx, AVFormatContext *fmt_ctx,
                          enum AVMediaType type);
    int DecodePacket(AVCodecContext *dec, const AVPacket *pkt);
    void DecodeThread();

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

    // thread
    std::mutex mutex_;
    std::condition_variable condition_variable_;
    std::thread *thread_;
};

#endif //FFMPEG_PLAYER_FFMPEG_PIPELINE_H
