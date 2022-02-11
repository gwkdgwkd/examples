#ifndef FFMPEG_PLAYER_OPENGL_SCALE_H
#define FFMPEG_PLAYER_OPENGL_SCALE_H

#include "scale_factory.h"

extern "C" {
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
};

class OpenglScale : public ScaleBase {
public:
    virtual bool Init(int src_width, int src_height, enum AVPixelFormat src_format, int dst_width,
                      int dst_height, enum AVPixelFormat dst_format) override;
    virtual NativeImage *Scale(AVFrame *src_frame) override;
    virtual void UnInit() override;
};

class OpenglScaleFactory : public ScaleFactory {
public:
    ~OpenglScaleFactory() {}
    virtual ScaleBase *CreateScale() { return new OpenglScale(); }
};


#endif // FFMPEG_PLAYER_OPENGL_SCALE_H
