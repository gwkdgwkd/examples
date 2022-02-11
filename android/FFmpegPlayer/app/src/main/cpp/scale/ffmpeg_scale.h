#ifndef FFMPEG_PLAYER_FFMPEG_SCALE_H
#define FFMPEG_PLAYER_FFMPEG_SCALE_H

#include "scale_factory.h"

extern "C" {
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
};

class FFmpegScale : public ScaleBase {
public:
    FFmpegScale();
    ~FFmpegScale();

    virtual bool Init(int src_width, int src_height, enum AVPixelFormat src_format, int dst_width,
                      int dst_height, enum AVPixelFormat dst_format) override;
    virtual NativeImage *Scale(AVFrame *src_frame) override;
    virtual void UnInit() override;

private:
    SwsContext *sws_ctx_;
    AVFrame *dst_frame_;
    uint8_t *frame_buffer_;
};

class FFmpegScaleFactory : public ScaleFactory {
public:
    virtual ScaleBase *CreateScale() { return new FFmpegScale(); }
};

#endif // FFMPEG_PLAYER_FFMPEG_SCALE_H
