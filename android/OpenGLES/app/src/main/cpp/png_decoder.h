#ifndef OPENGLES_PNG_DECODER_H
#define OPENGLES_PNG_DECODER_H

extern "C" {
	#include "libpng/image.h"
}

typedef unsigned char byte;

/**
 * PNG图像解码
 */
class PngPicDecoder {
private:
	FILE* pngFile;

	byte* compressedData;
	int actualSize;
public:
	PngPicDecoder();
	/** 开启解码线程 **/
	virtual ~PngPicDecoder();
	/** 打开本地文件不需要传递 探针的参数以及重试策略 **/
	int openFile(char *pngFilePath);
	RawImageData getRawImageData();
	void releaseRawImageData(const RawImageData* data);
	void closeFile();

};

#endif //OPENGLES_PNG_DECODER_H

