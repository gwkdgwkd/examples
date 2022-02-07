#include "wapped_texture.h"
#include "log.h"

const char *WappedTexture::TAG_ = "WappedTexture";

bool WappedTexture::CreateTexture() {
    TRACE_FUNC();
    texture_ = 0;
    if (InitTexture() < 0) {
        LOGE("init texture failed...");
        Dealloc();
        return false;
    }
    return true;
}

void WappedTexture::UpdateTexImage(unsigned char *pixels, int frameWidth, int frameHeight) {
    TRACE_FUNC();
    if (pixels) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture_);
        if (CheckGlError("glBindTexture")) {
            return;
        }
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, frameWidth, frameHeight, 0, GL_RGBA,
                     GL_UNSIGNED_BYTE, pixels);
        glBindTexture(GL_TEXTURE_2D, GL_NONE);
    }
}

bool WappedTexture::BindTexture(GLint uniformSampler) {
    TRACE_FUNC();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture_);
    if (CheckGlError("glBindTexture")) {
        return false;
    }
    glUniform1i(uniformSampler, 0);

    return true;
}

int WappedTexture::InitTexture() {
    TRACE_FUNC();
    glGenTextures(1, &texture_);
    glBindTexture(GL_TEXTURE_2D, texture_);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_2D, GL_NONE);

    return 1;
}

bool WappedTexture::CheckGlError(const char *op) {
    TRACE_FUNC();
    GLint error;
    for (error = glGetError(); error; error = glGetError()) {
        LOGI("error::after %s() glError (0x%x)\n", op, error);
        return true;
    }
    return false;
}

void WappedTexture::Dealloc() {
    TRACE_FUNC();
    if (texture_) {
        glDeleteTextures(1, &texture_);
    }
}
