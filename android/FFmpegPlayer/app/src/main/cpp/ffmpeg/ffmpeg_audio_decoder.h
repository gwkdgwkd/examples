#ifndef FFMPEG_PLAYER_FFMPEG_AUDIO_DECODER_H
#define FFMPEG_PLAYER_FFMPEG_AUDIO_DECODER_H

#include "thread_base.h"
#include "thread_safe_queue.h"
#include "play_control_observer_interface.h"
#include "seek_observer_interface.h"
#include "ffmpeg_demuxer.h"
#include "ffmpeg_codec_base.h"
#include "audio_frame.h"
#include "log.h"

extern "C" {
#include <libavutil/opt.h>
#include <libavutil/channel_layout.h>
#include <libavutil/samplefmt.h>
#include <libavutil/timestamp.h>
#include <libswresample/swresample.h>
};

class FFmpegAudioDecoder : public ThreadBase,
                           public CodecBase,
                           public PlayControlObserverInterface,
                           public SeekObserverInterface {
public:
    FFmpegAudioDecoder(FFmpegDemuxer *ffmpeg_demuxer);
    ~FFmpegAudioDecoder();

    bool Init();
    AudioFrame *GetAudioFrame();
    double GetClock() { return clock_; }

    virtual void OnControlEvent(ControlType type) override;
    virtual void OnSeekTo(float position) override;

private:
    virtual void Process() override;
    virtual int OutputFrame(AVFrame *frame) override;
    virtual void FlushFrameQueue() override;

    enum AVMediaType type_;
    SwrContext *swr_ctx_;
    int dst_nb_samples_;
    int dst_frame_data_size_;
    ThreadSafeQueue<AudioFrame *> audio_frame_queue_;
    FFmpegDemuxer *ffmpeg_demuxer_;

    int audio_frame_count_;
    int audio_packet_count_;
    double clock_;
};

#endif // FFMPEG_PLAYER_FFMPEG_AUDIO_DECODER_H
