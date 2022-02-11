#include <string.h>
#include <fstream>
#include <streambuf>

#include "wapped_shader_program.h"
#include "log.h"

const char *WappedShaderProgram::TAG_ = "WappedShaderProgram";

WappedShaderProgram::WappedShaderProgram() {
    TRACE_FUNC();
    vertex_shader_ = 0;
    fragment_shader_ = 0;
    program_ = 0;
}

WappedShaderProgram::~WappedShaderProgram() {
    TRACE_FUNC();
    if (vertex_shader_)
        glDeleteShader(vertex_shader_);
    if (fragment_shader_)
        glDeleteShader(fragment_shader_);
    if (program_) {
        glDeleteProgram(program_);
        program_ = 0;
    }
}

bool WappedShaderProgram::Init(const char *vs, const char *fs) {
    TRACE_FUNC();
    int ret = false;

    // create program
    program_ = glCreateProgram();

    // attach vertex shader
    if (!CompileShader(&vertex_shader_, GL_VERTEX_SHADER, vs)) {
        return ret;
    }
    glAttachShader(program_, vertex_shader_);

    // attach fragment shader
    if (!CompileShader(&fragment_shader_, GL_FRAGMENT_SHADER, fs)) {
        return ret;
    }
    glAttachShader(program_, fragment_shader_);

    // link program
    glLinkProgram(program_);

    // check status
    if (!CheckProgramErrors(program_)) {
        return ret;
    }

    return true;
}

bool WappedShaderProgram::CheckShaderErrors(GLuint shader) {
    TRACE_FUNC();
    GLint success;
    GLchar infoLog[1024];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 1024, NULL, infoLog);
        LOGE("ERROR::SHADER_COMPILATION_ERROR : %d\n%s", shader, infoLog);
        return false;
    }

    return true;
}

bool WappedShaderProgram::CheckProgramErrors(GLuint program) {
    TRACE_FUNC();
    GLint success;
    GLchar log[1024];
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program, 1024, NULL, log);
        LOGE("ERROR::PROGRAM_LINKING_ERROR : %d\n%s", program, log);
        return false;
    }

    return true;
}

bool WappedShaderProgram::CompileShader(GLuint *shader, GLenum type, const char *source) {
    TRACE_FUNC();
    // create shader
    *shader = glCreateShader(type);
    if (*shader == 0 || *shader == GL_INVALID_ENUM) {
        LOGI("Failed to create shader %d", type);
        return false;
    }

    // add source
    glShaderSource(*shader, 1, &source, NULL);

    // compile shader
    glCompileShader(*shader);

    // check status
    return CheckShaderErrors(*shader);
}


