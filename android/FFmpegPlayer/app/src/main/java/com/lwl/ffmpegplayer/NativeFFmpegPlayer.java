package com.lwl.ffmpegplayer;

public class NativeFFmpegPlayer {
    static {
        System.loadLibrary("ffmpegplayer");
    }

    private EventCallback eventCallback = null;
    public void AddEventCallback(EventCallback callback) {
        eventCallback = callback;
    }

    public native String GetFFmpegVersion();
    public native void Init(String url, int playerType, int renderType);
    public native void Play();
    public native void SeekToPosition(float position);
    public native void Pause();
    public native void Stop();
    public native void UnInit();

    // for GL render
    public native void OnSurfaceCreated(Object surface);
    public native void OnSurfaceChanged(int width, int height);
    public native void OnDrawFrame();

    public interface EventCallback {
        void onPlayerEvent(int msgType, float msgValue);
    }
}
