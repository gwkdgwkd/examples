#include "png_decoder.h"
#include <errno.h>
#include "log.h"

PngPicDecoder::PngPicDecoder() : png_file_(nullptr) {}

int PngPicDecoder::OpenFile(char *pngFilePath) {
    TRACE_FUNC();

    LOGI("open file : %s", pngFilePath);
    if (png_file_ == nullptr) {
        if ((png_file_ = fopen(pngFilePath, "rb")) == nullptr) {
            LOGE("open file failed : %d ,%s", errno, strerror(errno));
        }
    }

    fseek(png_file_, 0, SEEK_END);
    int data_length = ftell(png_file_);
    rewind(png_file_);
    compressed_data_ = new byte[data_length];
    actual_size_ = fread(compressed_data_, 1, data_length, png_file_);
    LOGI("file size %d, read size: %d", data_length, actual_size_);

    // 没有返回值会发生错误：Fatal signal 5 (SIGTRAP), code 1 (TRAP_BRKPT)
    return data_length;
}

RawImageData PngPicDecoder::GetRawImageData() {
    TRACE_FUNC();
    RawImageData data = get_raw_image_data_from_png((void *) compressed_data_, actual_size_);
    return data;
}

void PngPicDecoder::ReleaseRawImageData(const RawImageData *data) {
    TRACE_FUNC();
    release_raw_image_data(data);
}

void PngPicDecoder::CloseFile() {
    TRACE_FUNC();
    delete[] compressed_data_;
    fclose(png_file_);
}
