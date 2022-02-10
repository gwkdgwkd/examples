#ifndef FFMPEG_PLAYER_FFMPEG_DEMUXER_H
#define FFMPEG_PLAYER_FFMPEG_DEMUXER_H

#include <string>

#include "thread_base.h"
#include "thread_safe_queue.h"

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/timestamp.h>
};

typedef void (*MessageCallback)(void*, int, float);

class FFmpegDemuxer : public ThreadBase {
public:
    FFmpegDemuxer(const char *url);
    ~FFmpegDemuxer();

    bool Init();
    AVCodecContext *GetCodecContext(enum AVMediaType type);
    AVStream *GetAVStream(enum AVMediaType type);
//    int GetPacket(enum AVMediaType type, AVPacket *pkt);
    AVPacket * GetPacket(enum AVMediaType type);
    bool GetDemuxerState() { return is_demuxer_finish_; }

    void SetMessageCallback(void* context, MessageCallback callback) {
        m_MsgContext = context;
        m_MsgCallback = callback;
    }
    void * m_MsgContext = nullptr;
    MessageCallback m_MsgCallback = nullptr;

private:
    bool OpenCodecContext(int *stream_idx,
                          AVCodecContext **dec_ctx, AVFormatContext *fmt_ctx,
                          enum AVMediaType type);
    virtual void Process() override;
    void PrintPackInfo(AVPacket *pkt);
    void PrintQueueInfo();



    std::string url_;
    AVFormatContext *fmt_ctx_;
    int video_stream_idx_;
    int audio_stream_idx_;
    AVCodecContext *video_dec_ctx_;
    AVCodecContext *audio_dec_ctx_;
    AVStream *video_stream_;
    AVStream *audio_stream_;

//    PacketQueue video_packet_queue_;
//    PacketQueue audio_packet_queue_;
    int video_packet_count_;
    int audio_packet_count_;
    ThreadSafeQueue<AVPacket *> video_packet_queue_;
    ThreadSafeQueue<AVPacket *> audio_packet_queue_;

    std::atomic_bool is_demuxer_finish_;
};

#endif //FFMPEG_PLAYER_FFMPEG_DEMUXER_H
