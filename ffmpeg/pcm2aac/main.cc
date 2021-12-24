#include <iostream>

using namespace std;

extern "C" {
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libavutil/avutil.h"
#include "libswresample/swresample.h"
}

int main() {
  cout << "pcm to aac" << endl;

  int ret = 0;

  // 没有定义
  // av_register_all();
  // avcodec_register_all();

  char inputfile[] = "../resources/audio.pcm";
  char outputfile[] = "audio.aac";

  const AVCodec *codec = avcodec_find_encoder(AV_CODEC_ID_AAC);
  if (!codec) {
    cout << "avcodec_find_encoder failed" << endl;
    return -1;
  }

  // 配置编码器上下文
  AVCodecContext *ac = avcodec_alloc_context3(codec);
  if (!ac) {
    cout << "avcodec_alloc_context3 failed" << endl;
    return -1;
  }

  ac->sample_rate = 44100;
  ac->channels = 2;
  ac->channel_layout = AV_CH_LAYOUT_STEREO;
  ac->bit_rate = 64000;
  ac->sample_fmt = AV_SAMPLE_FMT_FLTP;

  // 给所有的音频帧设置一个统一的头部
  ac->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;

  // 打开音频编码器
  ret = avcodec_open2(ac, codec, NULL);
  if (ret < 0) {
    cout << "avcodec_open2 failed" << endl;
    return -1;
  }

  // 创建输出上下文
  AVFormatContext *oc = NULL;
  avformat_alloc_output_context2(&oc, NULL, NULL, outputfile);
  if (!oc) {
    cout << "avformat_alloc_output_context2 failed" << endl;
    return -1;
  }

  // 为输出上下文创建一个音频流
  AVStream *st = avformat_new_stream(oc, NULL);
  st->codecpar->codec_tag = 0;  // 不加的话，会有坑

  // 用编码器的上下文来配置音频流中编码器的参数
  avcodec_parameters_from_context(st->codecpar, ac);

  // 打印音频或视频流的信息
  av_dump_format(oc, 0, outputfile, 1);

  // 真正打开文件
  avio_open(&oc->pb, outputfile, AVIO_FLAG_WRITE);
  if (ret < 0) {
    cout << "avio_open failed" << endl;
    return -1;
  }

  avformat_write_header(oc, NULL);

  SwrContext *ctx = NULL;
  ctx = swr_alloc_set_opts(ctx,
                           // 输出的音频参数
                           ac->channel_layout, ac->sample_fmt, ac->sample_rate,
                           // 输入的音频参数
                           AV_CH_LAYOUT_STEREO, AV_SAMPLE_FMT_S16, 44100,
                           // 偏移量和记录日志
                           0, 0);
  if (!ctx) {
    cout << "swr_alloc_set_opts failed" << endl;
    return -1;
  }

  ret = swr_init(ctx);
  if (ret < 0) {
    cout << "swr_init failed" << endl;
    return -1;
  }

  // AVFrame存放原始数据
  AVFrame *frame = av_frame_alloc();
  frame->format = AV_SAMPLE_FMT_FLTP;
  frame->channels = 2;
  frame->channel_layout = AV_CH_LAYOUT_STEREO;
  frame->nb_samples = 1024;  // 一帧音频的样本数量

  ret = av_frame_get_buffer(frame, 0);
  if (ret < 0) {
    cout << "av_frame_get_buffer failed" << endl;
    return -1;
  }

  int readSize = frame->nb_samples * 2 * 2;
  char *pcms = new char[readSize];

  // 打开输入文件
  FILE *fp = fopen(inputfile, "rb");

  for (;;) {
    int len = fread(pcms, 1, readSize, fp);
    if (len <= 0) {
      break;
    }

    // 重采样之前的数据
    const uint8_t *data[1];
    data[0] = (uint8_t *)pcms;

    // 重采样
    len = swr_convert(ctx,
                      // 重采样之后的数据
                      frame->data, frame->nb_samples,
                      // 重采样之前的数据
                      data, frame->nb_samples);
    if (len <= 0) {
      break;
    }

    // AVPacket保存编码后的数据
    AVPacket pkt;
    av_init_packet(&pkt);

    // Todo
    // 有2帧没有处理，需要放到循环里。发送和接受，哪个要放到循环里？
    // [aac @ 0x563774a91b40] 2 frames left in the queue on closing

    // 将重采样的数据发送到编码线程
    ret = avcodec_send_frame(ac, frame);
    if (ret < 0) {
      continue;
    }

    ret = avcodec_receive_packet(ac, &pkt);
    if (ret != 0) {
      continue;
    }

    pkt.stream_index = 0;  // 0表示音频流
    pkt.dts = 0;
    pkt.pts = 0;

    av_interleaved_write_frame(oc, &pkt);
    cout << len << endl;
  }

  delete pcms;
  pcms = NULL;

  // 写入索引，播放时可以seek
  av_write_trailer(oc);

  // 关闭打开的文件的IO流
  avio_close(oc->pb);

  // 关闭编码器
  avcodec_close(ac);
  // 清理了编码器设置的参数
  avcodec_free_context(&ac);

  avformat_free_context(oc);
}

// g++ -I../source/ffmpeg_build/include/ main.cc -L../source/ffmpeg_build/lib/
// -lavformat -lavcodec -lavutil -lswresample -lpthread -lfdk-aac -lx264 -lx265
// -lSvtAv1Enc -lopus -lvpx -laom -ldav1d -lvorbis -lvorbisenc -lmp3lame -lz -lm
// -ldl -lnuma -llzma -lva -lva-x11 -lva-drm -lX11 -lvdpau -lgnutls -lbz2