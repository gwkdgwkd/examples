#ifndef FFMPEG_PLAYER_FFMPEG_CODEC_BASE_H
#define FFMPEG_PLAYER_FFMPEG_CODEC_BASE_H

#include <atomic>

extern "C" {
#include <libavcodec/avcodec.h>
};

class CodecBase {
public:
    CodecBase();
    virtual ~CodecBase();

protected:
    int DecodePacket(AVCodecContext *dec, AVPacket *pkt);
    virtual int OutputFrame(AVFrame *frame) = 0;
    virtual void FlushFrameQueue() = 0;

    AVFrame *frame_;
    std::atomic<bool> need_flush_;
};

#endif //FFMPEG_PLAYER_FFMPEG_CODEC_BASE_H
