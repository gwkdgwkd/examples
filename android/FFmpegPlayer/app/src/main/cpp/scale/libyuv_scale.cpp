#include "libyuv_scale.h"

#include <chrono>

LibyuvScale::LibyuvScale() {
    frame_buffer_ = nullptr;
    dst_frame_ = av_frame_alloc();
}

LibyuvScale::~LibyuvScale() {
    av_frame_free(&dst_frame_);
    dst_frame_ = nullptr;
}

bool LibyuvScale::Init(int src_width, int src_height, enum AVPixelFormat src_format, int dst_width,
                       int dst_height, enum AVPixelFormat dst_format) {
    src_width_ = src_width;
    src_height_ = src_height;
    src_format_ = src_format;
    dst_width_ = dst_width;
    dst_height_ = dst_height;
    dst_format_ = dst_format;

    int buffer_size = av_image_get_buffer_size(src_format_, dst_width_, dst_height_, 1);
    frame_buffer_ = (uint8_t *) av_malloc(buffer_size * sizeof(uint8_t));
    av_image_fill_arrays(dst_frame_->data, dst_frame_->linesize,
                         frame_buffer_, src_format_, dst_width_, dst_height_, 1);

    return true;
}

NativeImage *LibyuvScale::Scale(AVFrame *src_frame) {

    auto beforeTime = std::chrono::steady_clock::now();
    libyuv::I420Scale(src_frame->data[0], src_frame->linesize[0],
              src_frame->data[1], src_frame->linesize[1],
              src_frame->data[2], src_frame->linesize[2],
              src_width_, src_height_,
              dst_frame_->data[0], dst_frame_->linesize[0],
              dst_frame_->data[1], dst_frame_->linesize[1],
              dst_frame_->data[2], dst_frame_->linesize[2],
              dst_width_, dst_height_, libyuv::kFilterNone);

    NativeImage *image = new NativeImage();
    image->format = IMAGE_FORMAT_RGBA;
    image->width = dst_width_;
    image->height = dst_height_;
    NativeImageUtil::AllocNativeImage(image);

    // libyuv的ARGB和android bitmap的ARGB_8888的存储顺序是不一样的。
    // ARGB_8888的存储顺序实际上是RGBA，对应的是libyuv的ABGR。
    // 因此如果想对应上android bitmap的ARGB_8888的存储顺序，需要按以下规律转换：
    //  I420转RGBA使用libyuv的I420ToABGR函数
    //  YV12转RGBA使用libyuv的I420ToABGR函数
    //  NV12转RGBA使用libyuv的NV12ToABGR函数
    //  NV21转RGBA使用libyuv的NV21ToABGR函数
    libyuv::I420ToABGR(dst_frame_->data[0], dst_frame_->linesize[0],
               dst_frame_->data[1], dst_frame_->linesize[1],
               dst_frame_->data[2], dst_frame_->linesize[2],
               image->ppPlane[0], image->pLineSize[0],
               dst_width_, dst_height_);
    auto afterTime = std::chrono::steady_clock::now();
    double duration_microsecond = std::chrono::duration<double, std::micro>(afterTime - beforeTime).count();
    double duration_millsecond = std::chrono::duration<double, std::milli>(afterTime - beforeTime).count();
//    LOGE("libyuv used : %lf microsecond(%lf millsecond)", duration_microsecond, duration_millsecond);

     /* save to rgba file
     static int i = 0;
     if(i++ < 20){
        FILE *fp;
        fp = fopen("/storage/emulated/0/amedia/2.rgba","ab");
        for (int j = 0; j < dst_height_; j++) {
          fwrite(image->ppPlane[0] + j*image->pLineSize[0], dst_width_*4, 1, fp);
        }
        fclose(fp);
     }
     // */

    return image;
}

void LibyuvScale::UnInit() {

}
