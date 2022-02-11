#include "opengl_scale.h"
#include <string.h>

bool OpenglScale::Init(int src_width, int src_height, enum AVPixelFormat src_format, int dst_width,
                       int dst_height, enum AVPixelFormat dst_format) {
    src_width_ = src_width;
    src_height_ = src_height;
    src_format_ = src_format;
    dst_width_ = dst_width;
    dst_height_ = dst_height;
    dst_format_ = dst_format;

    return true;
}

NativeImage *OpenglScale::Scale(AVFrame *src_frame) {
    NativeImage *image = new NativeImage();
    if (src_format_ == AV_PIX_FMT_YUV420P || src_format_ == AV_PIX_FMT_YUVJ420P) {
        image->format = IMAGE_FORMAT_I420;
        image->width = src_frame->width;
        image->height = src_frame->height;
        NativeImageUtil::AllocNativeImage(image);
        // y plane
        if (image->pLineSize[0] != src_frame->linesize[0]) {
            for (int i = 0; i < src_frame->height; ++i) {
                memcpy(image->ppPlane[0] + i * image->pLineSize[0],
                       src_frame->data[0] + i * src_frame->linesize[0], image->width);
            }
        } else {
            memcpy(image->ppPlane[0], src_frame->data[0],
                   image->pLineSize[0] * src_frame->height);
        }

        // u plane
        if (src_frame->linesize[1] != image->pLineSize[1]) {
            for (int i = 0; i < src_frame->height / 2; ++i) {
                memcpy(image->ppPlane[1] + i * image->pLineSize[1],
                       src_frame->data[1] + i * src_frame->linesize[1], image->width / 2);
            }
        } else {
            memcpy(image->ppPlane[1], src_frame->data[1],
                   image->pLineSize[1] * src_frame->height / 2);
        }

        // v plane
        if (src_frame->linesize[2] != image->pLineSize[2]) {
            for (int i = 0; i < src_frame->height / 2; ++i) {
                memcpy(image->ppPlane[2] + i * image->pLineSize[2],
                       src_frame->data[2] + i * src_frame->linesize[2], image->width / 2);
            }
        } else {
            memcpy(image->ppPlane[2], src_frame->data[2],
                   image->pLineSize[2] * src_frame->height / 2);
        }

        if (src_frame->data[0] && src_frame->data[1] && !src_frame->data[2] &&
            src_frame->linesize[0] == src_frame->linesize[1] && src_frame->linesize[2] == 0) {
            // on some android device, output of h264 mediacodec decoder is NV12 兼容某些设备可能出现的格式不匹配问题
            image->format = IMAGE_FORMAT_NV12;
        }
    } else if (src_format_ == AV_PIX_FMT_NV12 || src_format_ == AV_PIX_FMT_NV21) {
        image->format = (src_format_ == AV_PIX_FMT_NV12 ? IMAGE_FORMAT_NV12 : IMAGE_FORMAT_NV21);
        image->width = src_frame->width;
        image->height = src_frame->height;
        NativeImageUtil::AllocNativeImage(image);
        // y plane
        if (src_frame->linesize[0] != image->pLineSize[0]) {
            for (int i = 0; i < src_frame->height; ++i) {
                memcpy(image->ppPlane[0] + i * image->pLineSize[0],
                       src_frame->data[0] + i * src_frame->linesize[0], image->width);
            }
        } else {
            memcpy(image->ppPlane[0], src_frame->data[0],
                   image->pLineSize[0] * src_frame->height);
        }

        // uv plane
        if (src_frame->linesize[1] != image->pLineSize[1]) {
            for (int i = 0; i < src_frame->height / 2; ++i) {
                memcpy(image->ppPlane[1] + i * image->pLineSize[1],
                       src_frame->data[1] + i * src_frame->linesize[1], image->width);
            }
        } else {
            memcpy(image->ppPlane[1], src_frame->data[1],
                   image->pLineSize[1] * src_frame->height / 2);
        }
    } else if (src_format_ == AV_PIX_FMT_RGBA) {
        image->format = IMAGE_FORMAT_RGBA;
        image->width = src_frame->width;
        image->height = src_frame->height;
        NativeImageUtil::AllocNativeImage(image);
        if (src_frame->linesize[0] != image->pLineSize[0]) {
            for (int i = 0; i < src_frame->height; ++i) {
                memcpy(image->ppPlane[0] + i * image->pLineSize[0],
                       src_frame->data[0] + i * src_frame->linesize[0], image->width * 4);
            }
        } else {
            memcpy(image->ppPlane[0], src_frame->data[0],
                   src_frame->linesize[0] * src_frame->height);
        }
    }

    return image;
}

void OpenglScale::UnInit() {

}
