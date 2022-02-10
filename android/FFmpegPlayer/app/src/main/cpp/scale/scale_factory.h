#ifndef FFMPEG_PLAYER_SCALE_FACTORY_H
#define FFMPEG_PLAYER_SCALE_FACTORY_H

extern "C" {
#include <libavutil/pixfmt.h>
};

class ScaleBase {
public:
    virtual ~ScaleBase() {}

    virtual bool Init(int src_width, int src_height, enum AVPixelFormat src_format, int dst_width,
                      int dst_height, enum AVPixelFormat dst_format) = 0;
    virtual bool Scale() = 0;
    virtual void UnInit() = 0;

protected:
    int src_width_;
    int src_height_;
    enum AVPixelFormat src_format_;
    int dst_width_;
    int dst_height_;
    enum AVPixelFormat dst_format_;
};

class ScaleFactory {
public:
    virtual ~ScaleFactory() {}
    virtual ScaleBase *CreateScale() = 0;
};

#endif // FFMPEG_PLAYER_SCALE_FACTORY_H
