#include "wapped_texture.h"
#include "log.h"

const char *WappedTexture::TAG_ = "WappedTexture";

bool WappedTexture::CreateTexture() {
    TRACE_FUNC();
    if (InitTexture() < 0) {
        LOGE("init texture failed...");
        Dealloc();
        return false;
    }
    return true;
}

void WappedTexture::UpdateTexImage(int id, UpdateTextureType type, unsigned char *pixels, int width,
                                   int height) {
    // TRACE_FUNC();
    if (id > texture_num_ - 1) {
        LOGE("texture id error!");
        return;
    }

    if (pixels) {
        glActiveTexture(GL_TEXTURE0 + id);
        glBindTexture(GL_TEXTURE_2D, textures_[id]);
        if (CheckGlError("glBindTexture")) {
            return;
        }
        if (type == kTypeRGBA) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
                         GL_UNSIGNED_BYTE, pixels);
        } else if (type == kTypeLUMINANCE) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, width, height, 0, GL_LUMINANCE,
                         GL_UNSIGNED_BYTE, pixels);
        } else if (type == kTypeLUMINANCEALPHA) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE_ALPHA, width, height, 0, GL_LUMINANCE_ALPHA,
                         GL_UNSIGNED_BYTE, pixels);
        }

        glBindTexture(GL_TEXTURE_2D, GL_NONE);
    }
}

bool WappedTexture::BindTexture(const std::vector<std::string> &tex_names) {
    // TRACE_FUNC();

    if (tex_names.size() != texture_num_) {
        LOGE("texture name num error");
        return false;
    }

    for (int i = 0; i < texture_num_; ++i) {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, textures_[i]);
        if (CheckGlError("glBindTexture")) {
            return false;
        }
        program_->SetInt(tex_names[i].c_str(), i);
    }

//    glActiveTexture(GL_TEXTURE0 + i);
//    glBindTexture(GL_TEXTURE_2D, m_TextureIds[i]);
//    char samplerName[64] = {0};
//    sprintf(samplerName, "s_texture%d", i);
//    GLUtils::setInt(m_ProgramObj, samplerName, i);
//    glUniform1i(glGetUniformLocation(program_, name.c_str()), value);

    return true;
}

int WappedTexture::InitTexture() {
    TRACE_FUNC();
    glGenTextures(texture_num_, textures_);
    for (int i = 0; i < texture_num_; ++i) {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, textures_[i]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glBindTexture(GL_TEXTURE_2D, GL_NONE);
    }

    return 1;
}

bool WappedTexture::CheckGlError(const char *op) {
    // TRACE_FUNC();
    GLint error;
    for (error = glGetError(); error; error = glGetError()) {
        LOGI("error::after %s() glError (0x%x)\n", op, error);
        return true;
    }
    return false;
}

void WappedTexture::Dealloc() {
    TRACE_FUNC();
    glDeleteTextures(texture_num_, textures_);
}
