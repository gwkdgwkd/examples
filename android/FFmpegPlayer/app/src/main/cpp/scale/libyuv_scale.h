#ifndef FFMPEG_PLAYER_LIBYUV_SCALE_H
#define FFMPEG_PLAYER_LIBYUV_SCALE_H

#include <libyuv.h>

#include "scale_factory.h"

extern "C" {
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
};

class LibyuvScale : public ScaleBase {
public:
    LibyuvScale();
    ~LibyuvScale();

    virtual bool Init(int src_width, int src_height, enum AVPixelFormat src_format, int dst_width,
                      int dst_height, enum AVPixelFormat dst_format) override;
    virtual NativeImage *Scale(AVFrame *src_frame) override;
    virtual void UnInit() override;

private:
    AVFrame *dst_frame_;
    uint8_t *frame_buffer_;
};

class LibyuvScaleFactory : public ScaleFactory {
public:
    ~LibyuvScaleFactory() {}
    virtual ScaleBase *CreateScale() { return new LibyuvScale(); }
};


#endif // FFMPEG_PLAYER_LIBYUV_SCALE_H
