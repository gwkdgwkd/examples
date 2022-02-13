package com.lwl.ffmpegplayer;

import com.lwl.ffmpegplayer.util.MediaInfo;

public class NativeFFmpegPlayer {
    static {
        System.loadLibrary("ffmpegplayer");
    }

    private EventCallback eventCallback = null;
    public void AddEventCallback(EventCallback callback) {
        eventCallback = callback;
    }
    private void playerEventCallback(int msgType, float msgValue) {
        if(eventCallback != null)
            eventCallback.onPlayerEvent(msgType, msgValue);

    }

    private void writePcm(byte[] pcm,int len) {
        if(eventCallback != null)
            eventCallback.onWritePcm(pcm, len);

    }

    public native String GetFFmpegVersion();
    public native void Init(String url, int view_type, int audio_render_type,
                            int video_render_type, int effect_type, int scale_type);
    public native void Play();
    public native void SeekToPosition(float position);
    public native void Pause();
    public native void Stop();
    public native void UnInit();

    public native int GetPcmBuffer(byte[] pcm,int len);

    // for GL render
    public native void OnSurfaceCreated(Object surface,int type);
    public native void OnSurfaceChanged(int width, int height, int type);
    public native void OnDrawFrame(int type);

    public native void GetMediaInfo(MediaInfo mediaInfo);

    public native void SetGesture(float xRotateAngle, float yRotateAngle, float scale);
    public native void SetTouchLoc(float touchX, float touchY);

    public interface EventCallback {
        void onPlayerEvent(int msgType, float msgValue);
        void onWritePcm(byte[] pcm,int len);
    }
}
