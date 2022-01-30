#ifndef FFMPEG_PLAYER_FFMPEG_DEMUXER_H
#define FFMPEG_PLAYER_FFMPEG_DEMUXER_H

#include <string>

#include "thread_base.h"
#include "thread_safe_queue.h"

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
};


class FFmpegDemuxer : public ThreadBase{
public:
    FFmpegDemuxer(const char *url);
    ~FFmpegDemuxer();

    bool Init();

    AVCodecContext *GetCodecContext(enum AVMediaType type);
    AVPacket *GetPacket(enum AVMediaType type);

private:
    bool OpenCodecContext(int *stream_idx,
                          AVCodecContext **dec_ctx, AVFormatContext *fmt_ctx,
                          enum AVMediaType type);
    virtual void Process() override;

    std::string url_;
    AVFormatContext *fmt_ctx_;
    int video_stream_idx_;
    int audio_stream_idx_;
    AVCodecContext *video_dec_ctx_;
    AVCodecContext *audio_dec_ctx_;
    AVStream *video_stream_;
    AVStream *audio_stream_;

    ThreadSafeQueue<AVPacket *> video_packet_queue_;
    ThreadSafeQueue<AVPacket *> audio_packet_queue_;
    AVPacketList list;
};

#endif //FFMPEG_PLAYER_FFMPEG_DEMUXER_H
