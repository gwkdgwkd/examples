#ifndef FFMPEG_PLAYER_OPENGLES_WAPPED_TEXTURE_H
#define FFMPEG_PLAYER_OPENGLES_WAPPED_TEXTURE_H

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

class WappedTexture {

public:
	WappedTexture() {}
	virtual ~WappedTexture() {}

	bool CreateTexture();
	void UpdateTexImage(unsigned char* pixels, int width, int height);
	bool BindTexture(GLint uniformSampler);
	void Dealloc();
private:
    int InitTexture();
    bool CheckGlError(const char* op);

    GLuint texture_;
    static const char* TAG_;
};

#endif // FFMPEG_PLAYER_OPENGLES_WAPPED_TEXTURE_H
