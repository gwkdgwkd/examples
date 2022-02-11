#ifndef FFMPEG_PLAYER_OPENGLES_WAPPED_TEXTURE_H
#define FFMPEG_PLAYER_OPENGLES_WAPPED_TEXTURE_H

#include <vector>

#include <GLES3/gl3.h>
#include <GLES3/gl3ext.h>

class WappedTexture {

public:
	WappedTexture(int num) : texture_num_(num) {}
	virtual ~WappedTexture() {}

	bool CreateTexture();
	void UpdateTexImage(int id, int type, unsigned char* pixels, int width, int height);
	bool BindTexture(GLint uniformSampler);
	void Dealloc();
private:
    int InitTexture();
    bool CheckGlError(const char* op);

    int texture_num_;
    std::vector<GLuint> textures_;
    static const char* TAG_;
};

#endif // FFMPEG_PLAYER_OPENGLES_WAPPED_TEXTURE_H
