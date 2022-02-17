#ifndef FFMPEG_PLAYER_FFMPEG_VIDEO_DECODER_H
#define FFMPEG_PLAYER_FFMPEG_VIDEO_DECODER_H

#include "thread_base.h"
#include "thread_safe_queue.h"
#include "play_control_observer_interface.h"
#include "seek_observer_interface.h"
#include "ffmpeg_demuxer.h"
#include "ffmpeg_codec_base.h"
#include "native_image.h"
#include "../render/video/video_render_interface.h"
#include "../scale/scale_factory.h"

extern "C" {
#include <libavutil/timestamp.h>
};

class VideoRenderInterface;
class FFmpegVideoDecoder : public ThreadBase,
                           public CodecBase,
                           public PlayControlObserverInterface,
                           public SeekObserverInterface {
public:
    FFmpegVideoDecoder(FFmpegDemuxer *ffmpeg_demuxer, ScaleFactory *scale_factory);
    ~FFmpegVideoDecoder();

    bool Init();
    void SetRenderSize(int width, int height);
    NativeImage *GetVideoImage();

    virtual void OnControlEvent(ControlType type) override;
    virtual void OnSeekTo(float position) override;
private:
    virtual void Process() override;
    virtual int OutputFrame(AVFrame *frame) override;
    virtual void FlushFrameQueue() override;

    enum AVMediaType type_;
    FFmpegDemuxer *ffmpeg_demuxer_;

    int render_width_;
    int render_height_;
    enum AVPixelFormat render_format_;
    int video_width_;
    int video_height_;
    enum AVPixelFormat video_format_;

    ThreadSafeQueue<NativeImage *> video_image_queue_;
    ScaleBase *scale_base_;

    int video_frame_count_;
    int video_packet_count_;
};

#endif // FFMPEG_PLAYER_FFMPEG_VIDEO_DECODER_H
