#ifndef FFMPEG_PLAYER_OPENGLES_WAPPED_TEXTURE_H
#define FFMPEG_PLAYER_OPENGLES_WAPPED_TEXTURE_H

#include <vector>

#include "wapped_shader_program.h"

class WappedTexture {
public:
	enum UpdateTextureType {
		kTypeRGBA,
		kTypeLUMINANCE,
		kTypeLUMINANCEALPHA
	};

	WappedTexture(int num, WappedShaderProgram *program) : texture_num_(num), program_(program) {}
	virtual ~WappedTexture() {}

	bool CreateTexture();
	void UpdateTexImage(int id, UpdateTextureType type, unsigned char* pixels, int width, int height);
	bool BindTexture(const std::vector<std::string> &tex_names);
	void Dealloc();
private:
    int InitTexture();
    bool CheckGlError(const char* op);

    int texture_num_;
	GLuint textures_[3];
	WappedShaderProgram *program_;
    static const char* TAG_;
};

#endif // FFMPEG_PLAYER_OPENGLES_WAPPED_TEXTURE_H
