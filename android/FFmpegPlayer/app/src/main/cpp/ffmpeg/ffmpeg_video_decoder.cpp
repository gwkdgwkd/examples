#include "ffmpeg_video_decoder.h"

FFmpegVideoDecoder::FFmpegVideoDecoder(FFmpegDemuxer *ffmpeg_demuxer, ScaleFactory *scale_factory)
        : ffmpeg_demuxer_(ffmpeg_demuxer), scale_factory_(scale_factory) {
    TRACE_FUNC();
    type_ = AVMEDIA_TYPE_VIDEO;

    rgb_frame_ = nullptr;
    frame_buffer_ = nullptr;
    sws_ctx_ = nullptr;

    video_frame_count_ = 0;
    video_packet_count_ = 0;

    scale_base_ = scale_factory_->CreateScale();
}

FFmpegVideoDecoder::~FFmpegVideoDecoder() {
    TRACE_FUNC();
    if (sws_ctx_) {
        sws_freeContext(sws_ctx_);
        sws_ctx_ = nullptr;
    }
}

bool FFmpegVideoDecoder::Init(VideoRenderInterface *video_render) {
    TRACE_FUNC();

    video_render_ = video_render;

    video_width_ = ffmpeg_demuxer_->GetCodecContext(type_)->width;
    video_height_ = ffmpeg_demuxer_->GetCodecContext(type_)->height;
    int src_width = ffmpeg_demuxer_->GetCodecContext(type_)->width;
    int src_height = ffmpeg_demuxer_->GetCodecContext(type_)->height;
    enum AVPixelFormat src_format = ffmpeg_demuxer_->GetCodecContext(type_)->pix_fmt;

    int dstSize[2] = {0};
    video_render_->Init(video_width_, video_height_, dstSize, this);
    render_width_ = dstSize[0];
    render_height_ = dstSize[1];
    enum AVPixelFormat render_format = AV_PIX_FMT_RGBA;

    LOGI("src: src_width: %d, src_height: %d, src_format: %d", src_width, src_height, src_format);
    LOGI("dst: dst_width: %d, dst_height: %d, dst_format: %d", render_width_, render_height_,
         render_format);

    rgb_frame_ = av_frame_alloc();
    int buffer_size = av_image_get_buffer_size(render_format, render_width_, render_height_, 1);
    frame_buffer_ = (uint8_t *) av_malloc(buffer_size * sizeof(uint8_t));
    av_image_fill_arrays(rgb_frame_->data, rgb_frame_->linesize,
                         frame_buffer_, render_format, render_width_, render_height_, 1);

    sws_ctx_ = sws_getContext(src_width, src_height, src_format, render_width_, render_height_,
                              render_format,
                              SWS_FAST_BILINEAR, NULL, NULL, NULL);

    scale_base_->Init(src_width, src_height, src_format, render_width_, render_height_,
                      render_format);

    return true;
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

    sws_scale(sws_ctx_, frame->data, frame->linesize, 0,
              ffmpeg_demuxer_->GetCodecContext(type_)->height, rgb_frame_->data,
              rgb_frame_->linesize);
//    NativeImage image;
//    image.format = IMAGE_FORMAT_RGBA;
//    image.width = render_width_;
//    image.height = render_height_;
//    image.ppPlane[0] = rgb_frame_->data[0];
//    image.pLineSize[0] = image.width * 4;
//    video_render_->RenderVideoFrame(&image);

    NativeImage *image = new NativeImage();
    image->format = IMAGE_FORMAT_RGBA;
    image->width = render_width_;
    image->height = render_height_;
    NativeImageUtil::AllocNativeImage(image);
    memcpy(image->ppPlane[0], rgb_frame_->data[0], image->width * image->height * 4);
    image->pLineSize[0] = image->width * 4;
    int fps1 = ffmpeg_demuxer_->GetAVStream(type_)->avg_frame_rate.num /
               ffmpeg_demuxer_->GetAVStream(type_)->avg_frame_rate.den;
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