#ifndef FFMPEG_PLAYER_OPENGLES_WAPPED_SHADER_H
#define FFMPEG_PLAYER_OPENGLES_WAPPED_SHADER_H

#include <GLES3/gl3.h>
#include <GLES3/gl3ext.h>

class WappedShaderProgram {
public:
    WappedShaderProgram();
    ~WappedShaderProgram();


    bool Init(const char *vs, const char *fs, const char *un);
    GLuint GetProgram() { return program_; }
    GLint GetUniformSampler() { return uniform_sampler_; }

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
