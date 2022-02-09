#ifndef FFMPEG_PLAYER_AUDIO_FRAME_H
#define FFMPEG_PLAYER_AUDIO_FRAME_H

class AudioFrame {
public:
    AudioFrame(int dataSize) {
        data_size_ = dataSize;
        data_ = static_cast<uint8_t *>(malloc(data_size_));
    }

    ~AudioFrame() {
        if (data_)
            free(data_);
        data_ = nullptr;
    }

    uint8_t *data_ = nullptr;
    int data_size_ = 0;
    double clock_ = 0.0;
};
#endif // FFMPEG_PLAYER_AUDIO_FRAME_H
