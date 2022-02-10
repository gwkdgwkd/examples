#ifndef FFMPEG_PLAYER_FFMPEG_VIDEO_DECODER_H
#define FFMPEG_PLAYER_FFMPEG_VIDEO_DECODER_H

#include "thread_base.h"
#include "thread_safe_queue.h"
#include "ffmpeg_demuxer.h"
#include "ffmpeg_codec_base.h"
#include "native_image.h"
#include "../render/video/video_render_interface.h"
#include "../scale/scale_factory.h"

extern "C" {
#include <libavutil/timestamp.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
};

class VideoRenderInterface;
class FFmpegVideoDecoder : public ThreadBase, CodecBase{
public:
    FFmpegVideoDecoder(FFmpegDemuxer *ffmpeg_demuxer, ScaleFactory *scale_factory);
    ~FFmpegVideoDecoder();

    bool Init(VideoRenderInterface *video_render);
    NativeImage *GetVideoImage();
private:
    virtual void Process() override;
    virtual int OutputFrame(AVFrame *frame) override;

    enum AVMediaType type_;
    AVFrame *rgb_frame_;
    uint8_t *frame_buffer_;
    SwsContext *sws_ctx_;
    FFmpegDemuxer *ffmpeg_demuxer_;

    int render_width_;
    int render_height_;
    int video_width_;
    int video_height_;
    VideoRenderInterface *video_render_;
    ThreadSafeQueue<NativeImage *> video_image_queue_;
    ScaleFactory *scale_factory_;
    ScaleBase *scale_base_;

    int video_frame_count_;
    int video_packet_count_;
};

#endif // FFMPEG_PLAYER_FFMPEG_VIDEO_DECODER_H
