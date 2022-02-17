#include "ffmpeg_codec_base.h"
#include "log.h"

CodecBase::CodecBase() {
    TRACE_FUNC();
    frame_ = av_frame_alloc();
    if (!frame_) {
        LOGE("Could not allocate frame");
    }
    need_flush_ = false;
}

CodecBase::~CodecBase() {
    TRACE_FUNC();
    av_frame_free(&frame_);
}

int CodecBase::DecodePacket(AVCodecContext *dec, AVPacket *pkt) {
//    TRACE_FUNC();
    int ret = 0;

    if(need_flush_) {
        LOGE("%s flush codec", dec->codec_descriptor->name);
        avcodec_flush_buffers(dec);
        FlushFrameQueue();
        need_flush_ = false;
    }

    // submit the packet to the decoder
    ret = avcodec_send_packet(dec, pkt);
    if (ret < 0) {
        LOGE("Error submitting a packet for decoding (%s)", av_err2str(ret));
        return ret;
    }

    // get all the available frames from the decoder
    while (ret >= 0) {
        ret = avcodec_receive_frame(dec, frame_);
        if (ret < 0) {
            // those two return values are special and mean there is no output
            // frame available, but there were no errors during decoding
            if (ret == AVERROR_EOF || ret == AVERROR(EAGAIN)) {
//                LOGE("AVERROR_EOF(%d) or AVERROR(EAGAIN)(%d) : %d", AVERROR_EOF, AVERROR(EAGAIN), ret);
                return 0;
            }

            LOGE("Error during decoding (%s)", av_err2str(ret));
            return ret;
        }
        ret = OutputFrame(frame_);
        av_frame_unref(frame_);
        if (ret < 0)
            return ret;
    }
//    av_frame_free(&frame_);

    return 0;
}


