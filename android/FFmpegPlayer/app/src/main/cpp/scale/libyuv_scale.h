#ifndef FFMPEG_PLAYER_LIBYUV_SCALE_H
#define FFMPEG_PLAYER_LIBYUV_SCALE_H

#include "scale_factory.h"

#include <libyuv.h>

class LibyuvScale : public ScaleBase {
public:
    virtual bool Init(int src_width, int src_height, enum AVPixelFormat src_format, int dst_width,
                      int dst_height, enum AVPixelFormat dst_format) override;
    virtual bool Scale() override;
    virtual void UnInit() override;
};

class LibyuvScaleFactory : public ScaleFactory {
public:
    ~LibyuvScaleFactory() {}
    virtual ScaleBase *CreateScale() { return new LibyuvScale(); }
};


#endif // FFMPEG_PLAYER_LIBYUV_SCALE_H
