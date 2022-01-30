#include "ffmpeg_codec_base.h"
#include "log.h"

CodecBase::CodecBase() {
    TRACE_FUNC();
    frame_ = av_frame_alloc();
    if (!frame_) {
        LOGE("Could not allocate frame");
    }
}

CodecBase::~CodecBase() {
    TRACE_FUNC();
    av_frame_free(&frame_);
}

int CodecBase::DecodePacket(AVCodecContext *dec, AVPacket *pkt) {
    TRACE_FUNC();
    int ret = 0;

    if(!pkt) {
        LOGE("get null packet!");
        return -1;
    }

    if(pkt->data == nullptr) {
        LOGE("pkt->data===========3========== is null");
    } else {
        LOGE("pkt->data===========3========== is not null %d %d",pkt->data,pkt->size);
        LOGE("pkt->data===========3====pkt->stream_index : %d",pkt->stream_index);
    }
    if(pkt->buf == nullptr) {
        LOGE("packet->buf===========3========== is null");
    } else {
        LOGE("packet->buf===========3========== is not null");
    }

    // submit the packet to the decoder
    ret = avcodec_send_packet(dec, pkt);
    if (ret < 0) {
        LOGE("Error submitting a packet for decoding (%s)", av_err2str(ret));
        return ret;
    }
    av_packet_free(&pkt);

    LOGE("avcodec_send_packet chenggggg 11");


    AVFrame *frame = av_frame_alloc();;

    // get all the available frames from the decoder
    while (ret >= 0) {
        ret = avcodec_receive_frame(dec, frame);
        LOGE("================((((((((((=========1");
        LOGI("frame->linesize %d",frame->linesize);
        LOGI("frame->channel_layout %lu",frame->channel_layout);
        LOGI("frame->nb_samples %d",frame->nb_samples);
        LOGI("frame->channels %d",frame->channels);
        LOGI("frame->pts %ld",frame->pts);
        LOGI("frame->pkt_dts %ld",frame->pkt_dts);
        LOGI("frame->sample_rate 111 %d",frame->sample_rate);
        LOGE("==================)))))))))))))=======2");

        LOGE("avcodec_send_packet chenggggg 22   %d ", ret);
        if (ret < 0) {
            // those two return values are special and mean there is no output
            // frame available, but there were no errors during decoding
            if (ret == AVERROR_EOF || ret == AVERROR(EAGAIN))
                return 0;

            LOGE("Error during decoding (%s)", av_err2str(ret));
            return ret;
        }
    }
    av_frame_free(&frame);

    return 0;
}


