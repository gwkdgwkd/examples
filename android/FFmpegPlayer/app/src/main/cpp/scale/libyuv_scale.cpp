#include "libyuv_scale.h"


extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/timestamp.h>
};
bool LibyuvScale::Init(int src_width, int src_height, enum AVPixelFormat src_format, int dst_width,
                       int dst_height, enum AVPixelFormat dst_format) {
    return true;
}
bool LibyuvScale::Scale() {

    AVFrame *frame;
    I420Scale(frame->data[0],frame->linesize[0],
              frame->data[1],frame->linesize[1],
              frame->data[2],frame->linesize[2],
              src_width_,src_height_,
              frame->data[0],frame->linesize[0],
              frame->data[1],frame->linesize[1],
              frame->data[2],frame->linesize[2],
              dst_width_,dst_height_,libyuv::kFilterNone);
    //why not I420ToRGBA？ ijkplayer uses I420ToABGR
//    I420ToABGR(is->scaleFrame->data[0],is->scaleFrame->linesize[0],
//               is->scaleFrame->data[1],is->scaleFrame->linesize[1],
//               is->scaleFrame->data[2],is->scaleFrame->linesize[2],
//               is->frameRGBA->data[0],is->frameRGBA->linesize[0],
//               width,height);

    return true;
}
void LibyuvScale::UnInit() {

}
