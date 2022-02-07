#ifndef FFMPEG_PLAYER_OPENGLES_WAPPED_SHADER_H
#define FFMPEG_PLAYER_OPENGLES_WAPPED_SHADER_H

#include <GLES3/gl3.h>
#include <GLES3/gl3ext.h>
#include <vec2.hpp>

class WappedShaderProgram {
public:
    WappedShaderProgram();
    ~WappedShaderProgram();


    bool Init(const char *vs, const char *fs, const char *un);
    GLuint GetProgram() { return program_; }
    GLint GetUniformSampler() { return uniform_sampler_; }

    void SetInt(const std::string &name, int value) {
        glUniform1i(glGetUniformLocation(program_, name.c_str()), value);
    }

    void SetFloat(const std::string &name, float value) {
        glUniform1f(glGetUniformLocation(program_, name.c_str()), value);
    }

    void SetVec2(const std::string &name, const glm::vec2 &value) {
        glUniform2fv(glGetUniformLocation(program_, name.c_str()), 1, &value[0]);
    }

private:
    bool CheckShaderErrors(GLuint shader);
    bool CheckProgramErrors(GLuint program);
    bool CompileShader(GLuint *shader, GLenum type, const char *source);

    GLuint vertex_shader_;
    GLuint fragment_shader_;
    GLuint program_;
    GLint uniform_sampler_;
    static const char *TAG_;
};

#endif // FFMPEG_PLAYER_OPENGLES_WAPPED_SHADER_H
