#ifndef FFMPEG_PLAYER_OPENGLES_WAPPED_SHADER_H
#define FFMPEG_PLAYER_OPENGLES_WAPPED_SHADER_H

#include <string>

#include <GLES3/gl3.h>
#include <GLES3/gl3ext.h>
#include <glm.hpp>

class WappedShaderProgram {
public:
    WappedShaderProgram();
    ~WappedShaderProgram();


    bool Init(const char *vs, const char *fs, const char *un = nullptr);
    GLuint GetProgram() { return program_; }
    GLint GetUniformSampler() { return uniform_sampler_; }

    void SetBool(const std::string &name, bool value) {
        glUniform1i(glGetUniformLocation(program_, name.c_str()), (int) value);
    }

    void SetInt(const std::string &name, int value) {
        glUniform1i(glGetUniformLocation(program_, name.c_str()), value);
    }

    void SetFloat(const std::string &name, float value) {
        glUniform1f(glGetUniformLocation(program_, name.c_str()), value);
    }

    void SetVec2(const std::string &name, const glm::vec2 &value) {
        glUniform2fv(glGetUniformLocation(program_, name.c_str()), 1, &value[0]);
    }

    void SetVec2(const std::string &name, float x, float y) {
        glUniform2f(glGetUniformLocation(program_, name.c_str()), x, y);
    }

    void SetVec3(const std::string &name, const glm::vec3 &value) {
        glUniform3fv(glGetUniformLocation(program_, name.c_str()), 1, &value[0]);
    }

    void SetVec3(const std::string &name, float x, float y, float z) {
        glUniform3f(glGetUniformLocation(program_, name.c_str()), x, y, z);
    }

    void SetVec4(const std::string &name, const glm::vec4 &value) {
        glUniform4fv(glGetUniformLocation(program_, name.c_str()), 1, &value[0]);
    }

    void SetVec4(const std::string &name, float x, float y, float z, float w) {
        glUniform4f(glGetUniformLocation(program_, name.c_str()), x, y, z, w);
    }

    void SetMat2(const std::string &name, const glm::mat2 &mat) {
        glUniformMatrix2fv(glGetUniformLocation(program_, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }

    void SetMat3(const std::string &name, const glm::mat3 &mat) {
        glUniformMatrix3fv(glGetUniformLocation(program_, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }

    void SetMat4(const std::string &name, const glm::mat4 &mat) {
        glUniformMatrix4fv(glGetUniformLocation(program_, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }

    glm::vec3 TexCoordToVertexCoord(glm::vec2 texCoord) {
        return glm::vec3(2 * texCoord.x - 1, 1 - 2 * texCoord.y, 0);
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
