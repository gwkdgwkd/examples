#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#ifndef OPENGLES_WAPPED_SHADER_H
#define OPENGLES_WAPPED_SHADER_H

class WappedShaderProgram {
public:
    WappedShaderProgram();
    ~WappedShaderProgram();

    enum {
        ATTRIBUTE_VERTEX, ATTRIBUTE_TEXCOORD,
    };

    bool Init();
    GLuint GetProgram() { return program_; }
    GLint GetUniformSampler() { return uniformSampler_; }

private:
    bool CheckShaderErrors(GLuint shader);
    bool CheckProgramErrors(GLuint program);
    bool CompileShader(GLuint *shader, GLenum type, const char *source);

    GLuint vertex_shader_;
    GLuint fragment_shader_;
    GLuint program_;
    GLint uniformSampler_;
    static const char* TAG_;
};

#endif // OPENGLES_WAPPED_SHADER_H
