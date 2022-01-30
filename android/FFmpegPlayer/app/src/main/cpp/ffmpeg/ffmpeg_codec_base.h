#ifndef FFMPEG_PLAYER_FFMPEG_CODEC_BASE_H
#define FFMPEG_PLAYER_FFMPEG_CODEC_BASE_H

extern "C" {
#include <libavcodec/avcodec.h>
};

class CodecBase {
public:
    CodecBase();
    virtual ~CodecBase();

protected:
    int DecodePacket(AVCodecContext *dec, AVPacket *pkt);

    AVFrame *frame_;
};

#endif //FFMPEG_PLAYER_FFMPEG_CODEC_BASE_H
