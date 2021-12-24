#include <string.h>

#include <iostream>

using namespace std;

// WAV是最常见的声音文件格式之一，是微软公司专门为Windows开发的一种标准数字音频文件，
// 该文件能记录各种单声道或立体声的声音信息，并能保证声音不失真。但WAV文件有一个缺点，
// 就是它所占用的磁盘空间太大（每分钟的音乐大约需要12兆磁盘空间）。
// 它符合资源互换文件格式（RIFF）规范，用于保存Windows平台的音频信息资源。

// 与PCM格式一样，都是保存原始数据，但WAV有文件头，播放器可以之间播放。
// WAV文件头信息由44个字节组成，所以只需要在PCM文件头部添加44个字节的WAV文件头，就可以生成WAV格式文件。
// 44个字节，表示没有附加块吧

// WAV文件头由哪几部分构成：
// 　RIFF块（RIFF-Chunk）
// 　格式化块（Format-Chunk）
// 　附加块（Fact-Chunk）块偏移地址需要根据实际数据变更
// 　数据块（Data-Chunk）块偏移地址需要根据实际数据变更

// wav的头部分第一部分
typedef struct WAV_HEADER {
  char chunkid[4];
  unsigned int chunksize;  // 4字节
  char format[4];
} pcmHeader;

// wav头的第二部分，保存着文件的采样率、通道数、码率等等一些基础参数
typedef struct WAV_FMT {
  char subformat[4];
  unsigned int sbusize;
  unsigned short audioFormat;  // 2字节
  unsigned short numchannels;
  unsigned int sampleRate;
  unsigned int byteRate;
  unsigned short blockAlign;
  unsigned short bitPerSample;
} pcmFmt;

typedef struct WAV_DATA {
  char wavdata[4];
  unsigned int dataSize;
} pcmData;

long getFileSize(char* filename) {
  FILE* fp = fopen(filename, "r");
  if (!fp) {
    return -1;
  }

  fseek(fp, 0, SEEK_END);
  long size = ftell(fp);
  fclose(fp);

  return size;
}

int pcvToWav(const char* pcmpath, int channles, int sample_rate, int fmtsize,
             const char* wavpath) {
  FILE *fp, *fpout;
  WAV_HEADER pcmHeader;
  WAV_FMT pcmFmt;
  WAV_DATA pcmData;

  int bits = 16;

  fp = fopen(pcmpath, "rb");
  if (fp == NULL) {
    cout << "fopen failed" << endl;
    return -1;
  }

  memcpy(pcmHeader.chunkid, "RIFF", strlen("RIFF"));
  long fileSize = 44 + getFileSize((char*)pcmpath) - 8;
  pcmHeader.chunksize = fileSize;
  memcpy(pcmHeader.format, "WAVE", strlen("WAVE"));

  memcpy(pcmFmt.subformat, "fmt ", strlen("fmt "));
  pcmFmt.sbusize = fmtsize;
  pcmFmt.audioFormat = 1;
  pcmFmt.numchannels = channles;
  pcmFmt.sampleRate = sample_rate;
  pcmFmt.byteRate = sample_rate * channles * bits / 8;

  pcmFmt.blockAlign = channles * bits / 8;
  pcmFmt.bitPerSample = bits;

  memcpy(pcmData.wavdata, "data", strlen("data"));
  pcmData.dataSize = getFileSize((char*)pcmpath);

  fpout = fopen(wavpath, "wb");
  if (fpout == NULL) {
    cout << "fopen failed" << endl;
    return -1;
  }

  fwrite(&pcmHeader, sizeof(pcmHeader), 1, fpout);
  fwrite(&pcmFmt, sizeof(pcmFmt), 1, fpout);
  fwrite(&pcmData, sizeof(pcmData), 1, fpout);

  char* buff = (char*)malloc(512);
  int len;

  while ((len = fread(buff, sizeof(char), 512, fp)) != 0) {
    fwrite(buff, sizeof(char), len, fpout);
  }

  free(buff);
  fclose(fp);
  fclose(fpout);

  return 0;
}

int main() {
  pcvToWav("../resources/audio.pcm", 2, 44100, 16, "outdio.wav");

  return 0;
}