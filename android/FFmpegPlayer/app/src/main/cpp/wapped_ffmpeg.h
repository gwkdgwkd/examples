#ifndef FFMPEG_PLAYER_WAPPED_FFMPEG_H
#define FFMPEG_PLAYER_WAPPED_FFMPEG_H

#include <string>

extern "C" {
#include <libavcodec/version.h>
#include <libavcodec/avcodec.h>
#include <libavformat/version.h>
#include <libavutil/version.h>
#include <libavfilter/version.h>
#include <libswresample/version.h>
#include <libswscale/version.h>
};

class WappedFFmpeg {
public:
    std::string GetFFmpegVersion();
};

#endif //FFMPEG_PLAYER_WAPPED_FFMPEG_H
