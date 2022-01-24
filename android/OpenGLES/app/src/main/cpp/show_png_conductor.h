#ifndef OPENGLES_SHOW_PNG_CONDUCTOR_H
#define OPENGLES_SHOW_PNG_CONDUCTOR_H

#include <thread>
#include <mutex>
#include <condition_variable>

#include <android/native_window.h>
#include "wapped_shader_program.h"
#include "wapped_texture.h"
#include "egl_core.h"
#include "png_decoder.h"

class ShowPngConductor {

public:
    ShowPngConductor();
    virtual ~ShowPngConductor() {}

    bool Start(char* spriteFilePath);
    void Stop();
    bool SetWindow(ANativeWindow* window);
    void ResetSize(int width, int height);

private:
    void RenderLoop();
    void UpdateTexImage();
    void DrawFrame();
    void Render();
    void Destroy();
    bool Initialize();

    enum RenderThreadMessage {
        MSG_NONE = 0, MSG_WINDOW_SET, MSG_RENDER_LOOP_EXIT
    } msg_;

    int screen_left_;
    int screen_right_;
    int screen_width_;
    int screen_height_;
    std::thread thread_;
    std::mutex mutex_;
    std::condition_variable condition_variable_;

    EGLCore *egl_core_ptr_;
    WappedShaderProgram *wapped_shader_program_ptr_;
    WappedTexture *wapped_texture_ptr_;
    PngPicDecoder* png_pic_decoder_ptr_;
    ANativeWindow* window_;
    static const char* TAG_;
};

#endif //OPENGLES_SHOW_PNG_CONDUCTOR_H
