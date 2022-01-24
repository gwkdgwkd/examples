#ifndef OPENGLES_PNG_DECODER_H
#define OPENGLES_PNG_DECODER_H

extern "C" {
	#include "libpng/image.h"
}

typedef unsigned char byte;

class PngPicDecoder {
private:
	FILE* png_file_;

	byte* compressed_data_;
	int actual_size_;
public:
	PngPicDecoder();

	virtual ~PngPicDecoder() {};
	int OpenFile(char *pngFilePath);
	RawImageData GetRawImageData();
	void ReleaseRawImageData(const RawImageData* data);
	void CloseFile();

};

#endif //OPENGLES_PNG_DECODER_H

