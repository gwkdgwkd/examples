#include "ffmpeg_video_decoder.h"

FFmpegVideoDecoder::FFmpegVideoDecoder(FFmpegDemuxer *ffmpeg_demuxer, ScaleFactory *scale_factory)
        : ffmpeg_demuxer_(ffmpeg_demuxer) {
    TRACE_FUNC();
    type_ = AVMEDIA_TYPE_VIDEO;

    video_frame_count_ = 0;
    video_packet_count_ = 0;

    scale_base_ = scale_factory->CreateScale();
    delete scale_factory;
}

FFmpegVideoDecoder::~FFmpegVideoDecoder() {
    TRACE_FUNC();
}

bool FFmpegVideoDecoder::Init() {
    TRACE_FUNC();

    video_width_ = ffmpeg_demuxer_->GetCodecContext(type_)->width;
    video_height_ = ffmpeg_demuxer_->GetCodecContext(type_)->height;
    enum AVPixelFormat video_format_ = ffmpeg_demuxer_->GetCodecContext(type_)->pix_fmt;

    return true;
}

void FFmpegVideoDecoder::SetRenderSize(int width, int height) {
    render_width_ = width;
    render_height_ = height;
    render_format_ = AV_PIX_FMT_RGBA;

    LOGI("src: src_width: %d, src_height: %d, src_format: %d", video_width_, video_height_, video_format_);
    LOGI("dst: dst_width: %d, dst_height: %d, dst_format: %d", render_width_, render_height_,
         render_format_);

    scale_base_->Init(video_width_, video_height_, video_format_, render_width_, render_height_,
                      render_format_);
}

void FFmpegVideoDecoder::OnPlay() {
    TRACE_FUNC();
    Start();
}

void FFmpegVideoDecoder::OnPause() {
    TRACE_FUNC();
    Pause();
}

void FFmpegVideoDecoder::OnResume() {
    TRACE_FUNC();
    Resume();
}

void FFmpegVideoDecoder::OnStop() {}

void FFmpegVideoDecoder::OnSeekTo(float position) {
    TRACE_FUNC();
    LOGE("FFmpegVideoDecoder OnSeekTo %f", position);
}

void FFmpegVideoDecoder::Process() {
//    TRACE_FUNC();

    AVPacket *pkt = av_packet_alloc();
    pkt = ffmpeg_demuxer_->GetPacket(type_);
    if (pkt == nullptr) {
//        LOGE("pkt is null");
        if (ffmpeg_demuxer_->GetDemuxerState()) {
            LOGE("thread puase");
            Pause();
        }
        return;
    }
//    LOGE("video packet n:%d size:%d pts:%s\n",
//         video_packet_count_++, pkt->size,
//         av_ts2timestr(pkt->pts, &ffmpeg_demuxer_->GetCodecContext(type_)->time_base));
    if (!DecodePacket(ffmpeg_demuxer_->GetCodecContext(type_), pkt) == 0) {
        LOGE("decode packet failed!");
    }

    return;
}

int FFmpegVideoDecoder::OutputFrame(AVFrame *frame) {
//    LOGE("video frame n:%d nb_samples:%d pts:%s\n",
//         video_frame_count_++, frame->nb_samples,
//         av_ts2timestr(frame->pts, &ffmpeg_demuxer_->GetCodecContext(type_)->time_base));

    NativeImage *image = scale_base_->Scale(frame);

//    int fps1 = ffmpeg_demuxer_->GetAVStream(type_)->avg_frame_rate.num /
//               ffmpeg_demuxer_->GetAVStream(type_)->avg_frame_rate.den;
//    LOGE("A-V fps1 %d, stream time base: %lf, codec time base %lf", fps1,
//         av_q2d(ffmpeg_demuxer_->GetAVStream(type_)->time_base), av_q2d(ffmpeg_demuxer_->GetCodecContext(type_)->time_base));
    double fps = av_q2d(ffmpeg_demuxer_->GetAVStream(type_)->avg_frame_rate);
//    LOGE("A-V fps %lf",fps);
    if (isnan(fps) || fps == 0) {
        fps = av_q2d(ffmpeg_demuxer_->GetAVStream(type_)->r_frame_rate);
    }
//    LOGE("A-V fps %lf",fps);
//    if(isnan(fps) || fps == 0) {
//        fps = av_q2d(av_guess_frame_rate(avFormatContext, ffmpeg_demuxer_->GetAVStream(type_)));
//    }
    double frame_delay = 1.0 / fps;
//    LOGE("A-V frame_delay %lf", frame_delay);
    image->delay = frame_delay + frame->repeat_pict / (2 * fps);
//    LOGE("A-V image->delay %lf", image->delay);
    image->clock = frame->best_effort_timestamp *
                   av_q2d(ffmpeg_demuxer_->GetAVStream(type_)->time_base);
//    LOGE("A-V image->clock %lf %lf", frame->best_effort_timestamp, image->clock);
//    LOGE("A-V++++++++++++fps %lf, delay %lf, clock %lf", fps, image->delay, image->clock);
    video_image_queue_.Push(image);

    return 0;
}

NativeImage *FFmpegVideoDecoder::GetVideoImage() {
    while (video_image_queue_.Empty()) {
        std::this_thread::sleep_for(std::chrono::microseconds(10));
    }
    return video_image_queue_.Pop();
}