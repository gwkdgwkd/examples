#ifndef FFMPEG_PLAYER_OPENGLES_RENDER_INTERFACE_H
#define FFMPEG_PLAYER_OPENGLES_RENDER_INTERFACE_H

class OpenglesRenderInterface {
public:
    virtual ~OpenglesRenderInterface() {}

    virtual void OnSurfaceCreated() = 0;
    virtual void OnSurfaceChanged(int w, int h) = 0;
    virtual void OnDrawFrame() = 0;

    virtual void UpdateMVPMatrix(int angleX, int angleY, float scaleX, float scaleY) = 0;
    virtual void SetTouchLoc(float touchX, float touchY) = 0;
};

#endif //FFMPEG_PLAYER_OPENGLES_RENDER_INTERFACE_H
