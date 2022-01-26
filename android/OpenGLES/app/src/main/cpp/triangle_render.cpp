#include "triangle_render.h"

#include "log.h"

static const char *vertexShaderSource =
        "attribute vec4 vPosition;"
        "void main(){"
        "gl_Position = vPosition;"
        "}";
static const char *fragmentShaderSource =
        "precision mediump float;"
        "uniform vec4 vColor;"
        "void main(){"
        "gl_FragColor = vColor;"
        "}";
static const float triangleCoords[] = {
        0.25, -0.25, 0.0,  // bottom right
        -0.25, -0.25, 0.0, // bottom left
        0, 0.25, 0.0, // top
};
static const float colors[] = { 0.63671875f, 0.76953125f, 0.22265625f };

void TriangleRender::init(int width, int height, ANativeWindow *window) {
    TRACE_FUNC();
    width_ = width;
    height_ = height;
    window_ = window;
    LOGI("EglCore init start");
    //初始化eglCore
    eglCore_ = new EGLCore();
    eglCore_->Init(window);
}

void TriangleRender::render() {
    TRACE_FUNC();
    LOGI("glViewport:width= %d height=%d",width_,height_);
    glViewport(0, 0, width_, height_);
    glClearColor(0.0f, 0.0f, 1.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glEnable(GL_BLEND);
//    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    createProgram();

}

void TriangleRender::destroy() {
    TRACE_FUNC();

    //销毁EGLCore以及EGLSurface
    if (eglCore_) {
        eglCore_->Release();
        eglCore_ = NULL;
    }
}

/**
 * 加载shader
 * @param shaderType shader类型
 * @return  shader
 */
int TriangleRender::loadShader(GLenum shaderType,const char* pSource) {
    TRACE_FUNC();
    GLint status;
    //创建shader
    GLuint shader = glCreateShader(shaderType);
    glShaderSource(shader, 1, &pSource, NULL);
    glCompileShader(shader);
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (!status) {
        GLint infoLen = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
        if (infoLen) {
            char* buf = (char*) malloc(infoLen);
            if (buf) {
                glGetShaderInfoLog(shader, infoLen, NULL, buf);
                LOGI("Could not compile shader %d:\n%s\n", shaderType, buf);
                free(buf);
            }
        } else {
            LOGI( "Guessing at GL_INFO_LOG_LENGTH size\n");
            char* buf = (char*) malloc(0x1000);
            if (buf) {
                glGetShaderInfoLog(shader, 0x1000, NULL, buf);
                LOGI("Could not compile shader %d:\n%s\n", shaderType, buf);
                free(buf);
            }
        }
        glDeleteShader(shader);
        shader = 0;
    }
    return shader;
}

/**
 * 创建显卡可执行程序
 */
void TriangleRender::createProgram() {
    TRACE_FUNC();
    LOGI("glCreateProgram");
    //创建显卡可执行程序
    GLuint programId = glCreateProgram();
    LOGI("glAttachShader start");
    //绑定shader
    GLuint vertexShader = loadShader(GL_VERTEX_SHADER,vertexShaderSource);
    glAttachShader(programId, vertexShader);
    GLuint fragmentShader = loadShader(GL_FRAGMENT_SHADER,fragmentShaderSource);
    glAttachShader(programId,fragmentShader);
    LOGI("glAttachShader end");
    LOGI("glLinkProgram start");
    //链接程序
    glLinkProgram(programId);
    LOGI("glUseProgram start");
    GLint status;
    glGetProgramiv(programId, GL_LINK_STATUS, &status);
    checkGlError("glGetProgramiv");
    if (status == GL_FALSE) {
        GLint bufLength = 0;
        glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &bufLength);
        if (bufLength) {
            char* buf = (char*) malloc(bufLength);
            if (buf) {
                glGetProgramInfoLog(programId, bufLength, NULL, buf);
                LOGI("Could not link program:\n%s\n", buf);
                free(buf);
            }
        }
        glDeleteProgram(programId);
        programId = 0;
        return;
    }
    //使用programId
    glUseProgram(programId);

    //填充数据
    //获取顶点着色器vPosition成员句柄
    GLint mPositionHandle = glGetAttribLocation(programId, "vPosition");
    //启用三角形顶点句柄
    glEnableVertexAttribArray(mPositionHandle);
    //准备三角形坐标数据
    glVertexAttribPointer(mPositionHandle, 3, GL_FLOAT, false, 12, triangleCoords);

    GLint mColorHandle = glGetUniformLocation(programId, "vColor");
    //设置绘制三角形的颜色
    glUniform4fv(mColorHandle, 1, colors);



    LOGI("glDrawElements start");
    //绘制三角形,使用glDrawElements或者glDrawArrays都可以绘制
//    glDrawElements(GL_TRIANGLE_STRIP, 5, GL_UNSIGNED_SHORT, indices);
    glDrawArrays(GL_TRIANGLE_STRIP,0,3);
    LOGI("glDrawElements end");
    eglCore_->SwapBuffers();

}

bool TriangleRender::checkGlError(const char* op) {
    TRACE_FUNC();
    for (GLint error = glGetError(); error; error = glGetError()) {
        LOGE("after %s() glError (0x%x)\n", op, error);
    }
    return true;
}
