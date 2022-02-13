#include "ffmpeg_scale.h"

#include <chrono>

FFmpegScale::FFmpegScale() {
    frame_buffer_ = nullptr;
    sws_ctx_ = nullptr;
    dst_frame_ = av_frame_alloc();
}

FFmpegScale::~FFmpegScale() {
    av_frame_free(&dst_frame_);
    dst_frame_ = nullptr;
}

bool FFmpegScale::Init(int src_width, int src_height, enum AVPixelFormat src_format, int dst_width,
                       int dst_height, enum AVPixelFormat dst_format) {

    src_width_ = src_width;
    src_height_ = src_height;
    src_format_ = src_format;
    dst_width_ = dst_width;
    dst_height_ = dst_height;
    dst_format_ = dst_format;
    sws_ctx_ = sws_getContext(src_width_, src_height_, src_format_, dst_width_, dst_height_,
                              dst_format_, SWS_FAST_BILINEAR, NULL, NULL, NULL);

    int buffer_size = av_image_get_buffer_size(dst_format_, dst_width_, dst_height_, 1);
    frame_buffer_ = (uint8_t *) av_malloc(buffer_size * sizeof(uint8_t));
    av_image_fill_arrays(dst_frame_->data, dst_frame_->linesize,
                         frame_buffer_, dst_format_, dst_width_, dst_height_, 1);

    return sws_ctx_ ? true : false;
}

NativeImage *FFmpegScale::Scale(AVFrame *src_frame) {

    /* save to yuv file
    static int i = 0;
    if (i < 20) {
        FILE *fp;
        int j;
        fp = fopen("/storage/emulated/0/amedia/1.yuv", "ab");
        for (j = 0; j < src_height_; j++) {
            fwrite(src_frame->data[0] + j * src_frame->linesize[0], src_width_, 1,
                   fp);
        }
        for (j = 0; j < src_height_ / 2; j++) {
            fwrite(src_frame->data[1] + j * src_frame->linesize[1], src_width_ / 2,
                   1, fp);
        }
        for (j = 0; j < src_height_ / 2; j++) {
            fwrite(src_frame->data[2] + j * src_frame->linesize[2], src_width_ / 2,
                   1, fp);
        }
        fclose(fp);
    }
    //*/

    auto beforeTime = std::chrono::steady_clock::now();
    sws_scale(sws_ctx_, src_frame->data, src_frame->linesize, 0,
              src_height_, dst_frame_->data, dst_frame_->linesize);
    auto afterTime = std::chrono::steady_clock::now();
    double duration_microsecond = std::chrono::duration<double, std::micro>(
            afterTime - beforeTime).count();
    double duration_millsecond = std::chrono::duration<double, std::milli>(
            afterTime - beforeTime).count();
    LOGE("sws_scale used : %lf microsecond(%lf millsecond)", duration_microsecond,
         duration_millsecond);

    NativeImage *image = new NativeImage();
    image->format = IMAGE_FORMAT_RGBA;
    image->width = dst_width_;
    image->height = dst_height_;
    NativeImageUtil::AllocNativeImage(image);
    if (dst_frame_->linesize[0] != image->pLineSize[0]) {
        for (int i = 0; i < dst_height_; ++i) {
            memcpy(image->ppPlane[0] + i * image->pLineSize[0],
                   dst_frame_->data[0] + i * dst_frame_->linesize[0], image->width * 4);
        }
    } else {
        // memcpy(image->ppPlane[0], dst_frame_->data[0], image->width * image->height * 4);
        memcpy(image->ppPlane[0], dst_frame_->data[0],
               dst_frame_->linesize[0] * dst_height_);
    }

    /* save to rgba file
    static int i = 0;
    if (i++ < 20) {
        FILE *fp;
        fp = fopen("/storage/emulated/0/amedia/2.rgba", "ab");
        for (int j = 0; j < dst_height_; j++) {
            fwrite(image->ppPlane[0] + j * image->pLineSize[0], dst_width_ * 4, 1, fp);
        }
        fclose(fp);
    }
    // */

    return image;
}

void FFmpegScale::UnInit() {

}
