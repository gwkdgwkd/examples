#include <chrono>
#include <iostream>
#include <mutex>
#include <thread>
using namespace std;

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavfilter/buffersink.h>
#include <libavfilter/buffersrc.h>
#include <libavformat/avformat.h>
#include <libavutil/audio_fifo.h>
#include <libavutil/channel_layout.h>
#include <libavutil/opt.h>
}

static AVFormatContext *fmt_ctx1;
static AVFormatContext *fmt_ctx2;
static AVFormatContext *fmt_ctx_o;

static AVCodecContext *dec_ctx1;
static AVCodecContext *dec_ctx2;
static AVCodecContext *dec_ctx_o;

static int audio_stream_index_1 = -1;
static int audio_stream_index_2 = -1;
static int audio_stream_index_o = -1;

AVAudioFifo *fifo_1 = NULL;
AVAudioFifo *fifo_2 = NULL;

// "aresample=8000,aformat=sample_fmts=s16:channel_layouts=mono";
static const char *filter_descr = "[in0][in1]amix=inputs=2[out]";

AVFilterContext *buffersink_ctx;
AVFilterContext *buffersrc_ctx1;
AVFilterContext *buffersrc_ctx2;

AVFilterGraph *filter_graph;

enum CaptureState { PREPARED, RUNNING, STOPPED, FINISHED };
CaptureState state = CaptureState::PREPARED;

static mutex m1, m2;

static int OpenInputFile(const char *filename, AVFormatContext **fmt_ctx,
                         AVCodecContext **dec_ctx, int *audio_stream_index) {
  int ret;
  const AVCodec *dec;
  if ((ret = avformat_open_input(fmt_ctx, filename, NULL, NULL)) < 0) {
    av_log(NULL, AV_LOG_ERROR, "Cannot open input file\n");
    return ret;
  }

  if ((ret = avformat_find_stream_info(*fmt_ctx, NULL)) < 0) {
    av_log(NULL, AV_LOG_ERROR, "Cannot find stream information\n");
    return ret;
  }

  /* select the audio stream */
  ret = av_find_best_stream(*fmt_ctx, AVMEDIA_TYPE_AUDIO, -1, -1, &dec, 0);
  if (ret < 0) {
    av_log(NULL, AV_LOG_ERROR,
           "Cannot find an audio stream in the input file\n");
    return ret;
  }

  *audio_stream_index = ret;
  *dec_ctx = avcodec_alloc_context3(dec);
  if (!*dec_ctx) {
    av_log(NULL, AV_LOG_ERROR, "avcodec_alloc_context3 failed\n");
    return AVERROR(ENOMEM);
  }
  avcodec_parameters_to_context(
      *dec_ctx, (*fmt_ctx)->streams[*audio_stream_index]->codecpar);
  av_opt_set_int(dec_ctx, "refcounted_frames", 1, 0);

  /* init the audio decoder */
  if ((ret = avcodec_open2(*dec_ctx, dec, NULL)) < 0) {
    av_log(NULL, AV_LOG_ERROR, "Cannot open audio decoder\n");
    return ret;
  }

  av_dump_format(*fmt_ctx, *audio_stream_index, filename, 0);

  return 0;
}

int OpenOutputFile(const char *fileName, AVFormatContext **fmt_ctx,
                   int *audio_stream_index) {
  int ret = 0;
  ret = avformat_alloc_output_context2(fmt_ctx, NULL, NULL, fileName);
  if (ret < 0) {
    printf("Mixer: failed to call avformat_alloc_output_context2\n");
    return -1;
  }
  AVStream *stream_a = NULL;
  stream_a = avformat_new_stream(*fmt_ctx, NULL);
  if (stream_a == NULL) {
    printf("Mixer: failed to call avformat_new_stream\n");
    return -1;
  }
  *audio_stream_index = 0;

  stream_a->codecpar->codec_type = AVMEDIA_TYPE_AUDIO;

  stream_a->codecpar->codec_id = AV_CODEC_ID_MP3;
  stream_a->codecpar->sample_rate = 44100;
  stream_a->codecpar->channels = 2;
  stream_a->codecpar->channel_layout = av_get_default_channel_layout(2);
  stream_a->codecpar->format = AV_SAMPLE_FMT_FLTP;
  stream_a->codecpar->bit_rate = 128000;
  stream_a->time_base.num = 1;
  stream_a->time_base.den = stream_a->codecpar->sample_rate;
  stream_a->codecpar->codec_tag = 0;
  if ((*fmt_ctx)->oformat->flags & AVFMT_GLOBALHEADER)
    stream_a->codecpar->codec_tag |= AV_CODEC_FLAG_GLOBAL_HEADER;

  const AVCodec *codec_mp3 = avcodec_find_encoder(AV_CODEC_ID_MP3);
  if (!codec_mp3) {
    av_log(NULL, AV_LOG_ERROR, "avcodec_find_encoder failed");
    return -1;
  }

  dec_ctx_o = avcodec_alloc_context3(codec_mp3);
  if (!dec_ctx_o) {
    av_log(NULL, AV_LOG_ERROR, "avcodec_alloc_context3 failed\n");
    return AVERROR(ENOMEM);
  }
  avcodec_parameters_to_context(dec_ctx_o, stream_a->codecpar);

  if (avcodec_open2(dec_ctx_o, codec_mp3, NULL) < 0) {
    av_log(NULL, AV_LOG_ERROR, "Mixer: failed to call avcodec_open2\n");
    return -1;
  }
  if (!((*fmt_ctx)->oformat->flags & AVFMT_NOFILE)) {
    if (avio_open(&((*fmt_ctx)->pb), fileName, AVIO_FLAG_WRITE) < 0) {
      av_log(NULL, AV_LOG_ERROR, "Mixer: failed to call avio_open\n");
      return -1;
    }
  }

  if (avformat_write_header(*fmt_ctx, NULL) < 0) {
    av_log(NULL, AV_LOG_ERROR, "Mixer: failed to call avformat_write_header\n");
    return -1;
  }

  bool b = (!(*fmt_ctx)->streams[0]->time_base.num &&
            (*fmt_ctx)->streams[0]->time_base.num);

  av_dump_format(*fmt_ctx, *audio_stream_index, fileName, 1);

  return 0;
}

void InitFifo() {
  fifo_1 = av_audio_fifo_alloc(
      (enum AVSampleFormat)fmt_ctx1->streams[audio_stream_index_1]
          ->codecpar->format,
      fmt_ctx1->streams[audio_stream_index_1]->codecpar->channels,
      44100 * fmt_ctx1->streams[audio_stream_index_1]->codecpar->frame_size);
  cout << fmt_ctx1->streams[audio_stream_index_1]->codecpar->format << endl;
  cout << fmt_ctx1->streams[audio_stream_index_1]->codecpar->channels << endl;
  cout << fmt_ctx1->streams[audio_stream_index_1]->codecpar->frame_size << endl;
  cout << av_audio_fifo_space(fifo_1) << endl;

  fifo_2 = av_audio_fifo_alloc(
      (enum AVSampleFormat)fmt_ctx2->streams[audio_stream_index_2]
          ->codecpar->format,
      fmt_ctx2->streams[audio_stream_index_2]->codecpar->channels,
      44100 * fmt_ctx2->streams[audio_stream_index_2]->codecpar->frame_size);
  cout << fmt_ctx2->streams[audio_stream_index_2]->codecpar->format << endl;
  cout << fmt_ctx2->streams[audio_stream_index_2]->codecpar->channels << endl;
  cout << fmt_ctx2->streams[audio_stream_index_2]->codecpar->frame_size << endl;
  cout << av_audio_fifo_space(fifo_2) << endl;
}

static int init_filters(const char *filters_descr) {
  char args1[512];
  char args2[512];
  int ret = 0;
  const AVFilter *abuffersrc1 = avfilter_get_by_name("abuffer");
  const AVFilter *abuffersrc2 = avfilter_get_by_name("abuffer");
  const AVFilter *abuffersink = avfilter_get_by_name("abuffersink");

  AVFilterInOut *outputs1 = avfilter_inout_alloc();
  AVFilterInOut *outputs2 = avfilter_inout_alloc();
  AVFilterInOut *inputs = avfilter_inout_alloc();

  static const enum AVSampleFormat out_sample_fmts[] = {AV_SAMPLE_FMT_FLTP,
                                                        AV_SAMPLE_FMT_NONE};
  static const int64_t out_channel_layouts[] = {AV_CH_LAYOUT_STEREO, -1};
  static const int out_sample_rates[] = {44100, -1};

  const AVFilterLink *outlink;
  AVRational time_base_1 = fmt_ctx1->streams[audio_stream_index_1]->time_base;
  AVRational time_base_2 = fmt_ctx2->streams[audio_stream_index_2]->time_base;
  filter_graph = avfilter_graph_alloc();
  if (!outputs1 || !inputs || !filter_graph) {
    ret = AVERROR(ENOMEM);
    goto end;
  }

  if (!dec_ctx1->channel_layout) {
    dec_ctx1->channel_layout =
        av_get_default_channel_layout(dec_ctx1->channels);
  }

  snprintf(
      args1, sizeof(args1),
      "time_base=%d/%d:sample_rate=%d:sample_fmt=%s:channel_layout=0x%" PRIx64,
      time_base_1.num, time_base_1.den, dec_ctx1->sample_rate,
      av_get_sample_fmt_name(dec_ctx1->sample_fmt), dec_ctx1->channel_layout);
  av_log(NULL, AV_LOG_INFO, "%s\n", args1);

  ret = avfilter_graph_create_filter(&buffersrc_ctx1, abuffersrc1, "in1", args1,
                                     NULL, filter_graph);
  if (ret < 0) {
    av_log(NULL, AV_LOG_ERROR, "Cannot create audio buffer source\n");
    goto end;
  }

  if (!dec_ctx2->channel_layout) {
    dec_ctx2->channel_layout =
        av_get_default_channel_layout(dec_ctx2->channels);
  }
  snprintf(
      args2, sizeof(args2),
      "time_base=%d/%d:sample_rate=%d:sample_fmt=%s:channel_layout=0x%" PRIx64,
      time_base_2.num, time_base_2.den, dec_ctx2->sample_rate,
      av_get_sample_fmt_name(dec_ctx2->sample_fmt), dec_ctx2->channel_layout);
  av_log(NULL, AV_LOG_INFO, "%s\n", args2);

  ret = avfilter_graph_create_filter(&buffersrc_ctx2, abuffersrc1, "in2", args2,
                                     NULL, filter_graph);
  if (ret < 0) {
    av_log(NULL, AV_LOG_ERROR, "Cannot create audio buffer source\n");
    goto end;
  }

  ret = avfilter_graph_create_filter(&buffersink_ctx, abuffersink, "out", NULL,
                                     NULL, filter_graph);
  if (ret < 0) {
    av_log(NULL, AV_LOG_ERROR, "Cannot create audio buffer sink\n");
    goto end;
  }

  ret = av_opt_set_int_list(buffersink_ctx, "sample_fmts", out_sample_fmts, -1,
                            AV_OPT_SEARCH_CHILDREN);
  if (ret < 0) {
    av_log(NULL, AV_LOG_ERROR, "Cannot set output sample format\n");
    goto end;
  }

  ret = av_opt_set_int_list(buffersink_ctx, "channel_layouts",
                            out_channel_layouts, -1, AV_OPT_SEARCH_CHILDREN);
  if (ret < 0) {
    av_log(NULL, AV_LOG_ERROR, "Cannot set output channel layout\n");
    goto end;
  }

  ret = av_opt_set_int_list(buffersink_ctx, "sample_rates", out_sample_rates,
                            -1, AV_OPT_SEARCH_CHILDREN);
  if (ret < 0) {
    av_log(NULL, AV_LOG_ERROR, "Cannot set output sample rate\n");
    goto end;
  }

  outputs1->name = av_strdup("in0");
  outputs1->filter_ctx = buffersrc_ctx1;
  outputs1->pad_idx = 0;
  outputs1->next = outputs2;

  outputs2->name = av_strdup("in1");
  outputs2->filter_ctx = buffersrc_ctx2;
  outputs2->pad_idx = 0;
  outputs2->next = NULL;

  inputs->name = av_strdup("out");
  inputs->filter_ctx = buffersink_ctx;
  inputs->pad_idx = 0;
  inputs->next = NULL;

  AVFilterInOut *filter_outputs[2];
  filter_outputs[0] = outputs1;
  filter_outputs[1] = outputs2;

  if ((ret = avfilter_graph_parse_ptr(filter_graph, filters_descr, &inputs,
                                      &outputs1, NULL)) < 0) {
    av_log(NULL, AV_LOG_ERROR, "parse ptr fail, ret: %d\n", ret);
    goto end;
  }

  if ((ret = avfilter_graph_config(filter_graph, NULL)) < 0) {
    av_log(NULL, AV_LOG_ERROR, "config graph fail, ret: %d\n", ret);
    goto end;
  }

  outlink = buffersink_ctx->inputs[0];
  av_get_channel_layout_string(args1, sizeof(args1), -1,
                               outlink->channel_layout);

  av_log(NULL, AV_LOG_INFO, "Output: srate:%dHz fmt:%s chlayout:%s\n",
         (int)outlink->sample_rate,
         (char *)av_x_if_null(
             av_get_sample_fmt_name(
                 static_cast<enum AVSampleFormat>(outlink->format)),
             "?"),
         args1);

end:
  avfilter_inout_free(&inputs);
  avfilter_inout_free(&outputs1);
  return ret;
}

int DecodecThreadProc1() {
  cout << "DecodecThreadProc1" << endl;
  int ret;

  AVFrame *frame = av_frame_alloc();
  if (!frame) {
    cout << "av_frame_alloc failed" << endl;
    return -1;
  }

  AVPacket *packet = av_packet_alloc();
  if (!packet) {
    cout << "av_packet_alloc failed" << endl;
    return -1;
  }

  while (state == CaptureState::RUNNING) {
    if (av_read_frame(fmt_ctx1, packet) < 0) {
      continue;
    }
    if (packet->stream_index == audio_stream_index_1) {
      ret = avcodec_send_packet(dec_ctx1, packet);
      if (ret < 0) {
        cout << "avcodec_send_packet failed" << endl;
        return -1;
      }

      while (ret >= 0) {
        ret = avcodec_receive_frame(dec_ctx1, frame);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
          continue;
        } else if (ret < 0) {
          break;
        }

        int fifo_spk_space = av_audio_fifo_space(fifo_1);
        while (fifo_spk_space < frame->nb_samples &&
               state == CaptureState::RUNNING) {
          this_thread::sleep_for(chrono::milliseconds(10));
          printf("fifo_1 full !\n");
          fifo_spk_space = av_audio_fifo_space(fifo_1);
        }

        if (fifo_spk_space >= frame->nb_samples) {
          m1.lock();
          int nWritten = av_audio_fifo_write(fifo_1, (void **)frame->data,
                                             frame->nb_samples);
          m1.unlock();
        }
      }
    }
  }

  av_frame_free(&frame);
  av_packet_free(&packet);

  return 0;
}

int DecodecThreadProc2() {
  cout << "DecodecThreadProc2" << endl;
  int ret;

  AVFrame *frame = av_frame_alloc();
  if (!frame) {
    cout << "av_frame_alloc failed" << endl;
    return -1;
  }

  AVPacket *packet = av_packet_alloc();
  if (!packet) {
    cout << "av_packet_alloc failed" << endl;
    return -1;
  }

  while (state == CaptureState::RUNNING) {
    if (av_read_frame(fmt_ctx2, packet) < 0) {
      continue;
    }
    if (packet->stream_index == audio_stream_index_2) {
      ret = avcodec_send_packet(dec_ctx2, packet);
      if (ret < 0) {
        cout << "avcodec_send_packet failed" << endl;
        return -1;
      }

      while (ret >= 0) {
        ret = avcodec_receive_frame(dec_ctx2, frame);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
          continue;
        } else if (ret < 0) {
          break;
        }

        int fifo_spk_space = av_audio_fifo_space(fifo_2);
        while (fifo_spk_space < frame->nb_samples &&
               state == CaptureState::RUNNING) {
          this_thread::sleep_for(chrono::milliseconds(10));
          printf("fifo_2 full !\n");
          fifo_spk_space = av_audio_fifo_space(fifo_2);
        }

        if (fifo_spk_space >= frame->nb_samples) {
          m2.lock();
          int nWritten = av_audio_fifo_write(fifo_2, (void **)frame->data,
                                             frame->nb_samples);
          m2.unlock();
        }
      }
    }
  }

  av_frame_free(&frame);
  av_packet_free(&packet);

  return 0;
}

static int write_packet(AVPacket *pkt) {
  int ret;
  static int frame_count = 0;

  pkt->stream_index = audio_stream_index_o;
  pkt->pts = frame_count++ *
             fmt_ctx_o->streams[audio_stream_index_o]->codecpar->frame_size;
  pkt->dts = pkt->pts;
  pkt->duration =
      fmt_ctx_o->streams[audio_stream_index_o]->codecpar->frame_size;

  pkt->pts = av_rescale_q_rnd(
      pkt->pts, fmt_ctx_o->streams[audio_stream_index_o]->time_base,
      fmt_ctx_o->streams[audio_stream_index_o]->time_base,
      (AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
  pkt->dts = pkt->pts;
  pkt->duration = av_rescale_q_rnd(
      pkt->duration, fmt_ctx_o->streams[audio_stream_index_o]->time_base,
      fmt_ctx_o->streams[audio_stream_index_o]->time_base,
      (AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));

  frame_count++;

  ret = av_interleaved_write_frame(fmt_ctx_o, pkt);
  if (ret < 0) {
    printf("Mixer: failed to call av_interleaved_write_frame\n");
  }
  printf("Mixer: write frame to file\n");

  return 0;
}

static int encode(AVCodecContext *ctx, AVFrame *frame, AVPacket *pkt) {
  int ret;

  ret = avcodec_send_frame(ctx, frame);
  if (ret < 0) {
    fprintf(stderr, "Error sending the frame to the encoder\n");
    return -1;
  }

  while (ret >= 0) {
    ret = avcodec_receive_packet(ctx, pkt);
    if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
      return ret;
    else if (ret < 0) {
      fprintf(stderr, "Error encoding audio frame\n");
      return -1;
    }

    write_packet(pkt);

    av_packet_unref(pkt);
  }

  return 0;
}

int main(int argc, char **argv) {
  int ret;
  thread t1, t2;
  int tmpFifoFailed = 0;
  int64_t frame_count = 0;

  if ((ret = OpenInputFile("../resources/1.mp3", &fmt_ctx1, &dec_ctx1,
                           &audio_stream_index_1)) < 0) {
    av_log(NULL, AV_LOG_ERROR, "open input file fail, ret: %d\n", ret);
    goto end;
  }

  if ((ret = OpenInputFile("../resources/2.mp3", &fmt_ctx2, &dec_ctx2,
                           &audio_stream_index_2)) < 0) {
    av_log(NULL, AV_LOG_ERROR, "open input file fail, ret: %d\n", ret);
    goto end;
  }

  if ((ret = OpenOutputFile("output.mp3", &fmt_ctx_o, &audio_stream_index_o)) <
      0) {
    av_log(NULL, AV_LOG_ERROR, "open output file fail, ret: %d\n", ret);
    goto end;
  }

  InitFifo();

  if ((ret = init_filters(filter_descr)) < 0) {
    av_log(NULL, AV_LOG_ERROR, "init filters fail, ret: %d\n", ret);
    goto end;
  }

  state = CaptureState::RUNNING;

  t1 = thread(DecodecThreadProc1);
  t1.detach();
  t2 = thread(DecodecThreadProc2);
  t2.detach();

  while (state != CaptureState::FINISHED) {
    int ret = 0;
    AVFrame *pFrame1 = av_frame_alloc();
    AVFrame *pFrame2 = av_frame_alloc();

    int got_packet_ptr = 0;

    int fifo_1_size = av_audio_fifo_size(fifo_1);
    int fifo_2_size = av_audio_fifo_size(fifo_2);
    int frame1_min_size =
        fmt_ctx1->streams[audio_stream_index_1]->codecpar->frame_size;
    int frame2_min_size =
        fmt_ctx2->streams[audio_stream_index_2]->codecpar->frame_size;
    // cout << "fifo_1_size: " << fifo_1_size
    //      << ",frame1_min_size: " << frame1_min_size << endl;
    // cout << "fifo_2_size: " << fifo_2_size
    //      << ",frame2_min_size: " << frame2_min_size << endl;
    if (fifo_1_size >= frame1_min_size && fifo_2_size >= frame2_min_size) {
      tmpFifoFailed = 0;

      pFrame1->nb_samples = frame1_min_size;
      pFrame1->channel_layout =
          fmt_ctx1->streams[audio_stream_index_1]->codecpar->channel_layout;
      pFrame1->format =
          fmt_ctx1->streams[audio_stream_index_1]->codecpar->format;
      pFrame1->sample_rate =
          fmt_ctx1->streams[audio_stream_index_1]->codecpar->sample_rate;
      av_frame_get_buffer(pFrame1, 0);

      pFrame2->nb_samples = frame2_min_size;
      pFrame2->channel_layout =
          fmt_ctx2->streams[audio_stream_index_2]->codecpar->channel_layout;
      pFrame2->format =
          fmt_ctx2->streams[audio_stream_index_2]->codecpar->format;
      pFrame2->sample_rate =
          fmt_ctx2->streams[audio_stream_index_2]->codecpar->sample_rate;
      av_frame_get_buffer(pFrame2, 0);

      m1.lock();
      ret = av_audio_fifo_read(fifo_1, (void **)pFrame1->data, frame1_min_size);
      m1.unlock();

      m2.lock();
      ret = av_audio_fifo_read(fifo_2, (void **)pFrame2->data, frame2_min_size);
      m2.unlock();

      ret = av_buffersrc_add_frame(buffersrc_ctx1, pFrame1);
      if (ret < 0) {
        printf("Mixer1: failed to call av_buffersrc_add_frame\n");
        break;
      }

      ret = av_buffersrc_add_frame(buffersrc_ctx2, pFrame2);
      if (ret < 0) {
        printf("Mixer2: failed to call av_buffersrc_add_frame\n");
        break;
      }

      while (1) {
        AVFrame *frame_out = av_frame_alloc();
        AVPacket *packet_out = av_packet_alloc();

        ret = av_buffersink_get_frame_flags(buffersink_ctx, frame_out, 0);
        if (ret < 0) {
          // printf("Mixer: failed to call av_buffersink_get_frame_flags\n");
          break;
        }

        encode(dec_ctx_o, frame_out, packet_out);

        av_packet_free(&packet_out);
        av_frame_free(&frame_out);
      }
    } else {
      tmpFifoFailed++;
      this_thread::sleep_for(chrono::milliseconds(20));
      if (tmpFifoFailed > 300) {
        state = CaptureState::STOPPED;
        this_thread::sleep_for(chrono::milliseconds(30));
        break;
      }
    }
    av_frame_free(&pFrame1);
    av_frame_free(&pFrame2);
  }

  av_write_trailer(fmt_ctx_o);
end:
  avcodec_close(dec_ctx1);
  avformat_close_input(&fmt_ctx1);

  avcodec_close(dec_ctx2);
  avformat_close_input(&fmt_ctx2);

  avcodec_close(dec_ctx_o);
  avformat_close_input(&fmt_ctx_o);

  av_audio_fifo_free(fifo_1);
  av_audio_fifo_free(fifo_2);

  avfilter_graph_free(&filter_graph);

  // av_frame_free(&frame);
  // av_frame_free(&filt_frame);

  exit(0);
}

// g++ -I../source/ffmpeg_build/include/ main.cc -L../source/ffmpeg_build/lib/
// -lavformat -lavfilter -lavcodec -lavutil -lswresample -lswscale -lpostproc
// -lpthread -lfdk-aac -lx264 -lx265 -lSvtAv1Enc -lopus -lvpx -laom -ldav1d
// -lvorbis -lvorbisenc -lmp3lame -lz -lm -ldl -lnuma -llzma -lva -lva-x11
// -lva-drm -lX11 -lvdpau -lgnutls -lbz2 -lfreetype -lass