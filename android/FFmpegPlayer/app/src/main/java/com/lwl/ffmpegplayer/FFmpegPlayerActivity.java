package com.lwl.ffmpegplayer;

import android.graphics.Point;
import android.os.Build;
import android.os.Bundle;
import android.os.Environment;
import android.util.Log;
import android.view.Display;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.widget.RelativeLayout;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;

public class FFmpegPlayerActivity extends AppCompatActivity implements SurfaceHolder.Callback, NativeFFmpegPlayer.EventCallback {
    private static final String TAG = "FFmpegPlayerActivity";
    private SurfaceView surfaceView = null;
    private NativeFFmpegPlayer nativeFFmpegPlayer = null;
    private String mVideoPath = Environment.getExternalStorageDirectory().getAbsolutePath() + "/byteflow/one_piece.mp4";

    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.ffmpeg_player);
        surfaceView = new SurfaceView(FFmpegPlayerActivity.this);
        surfaceView.getHolder().addCallback(this);
        nativeFFmpegPlayer = new NativeFFmpegPlayer();
        nativeFFmpegPlayer.AddEventCallback(this);
        nativeFFmpegPlayer.Init(mVideoPath, 1, 1);
    }

    @Override
    public void surfaceCreated(@NonNull SurfaceHolder surfaceHolder) {
        Log.d(TAG,"FFmpegPlayerActivity surfaceCreated");
        nativeFFmpegPlayer.OnSurfaceCreated(surfaceHolder.getSurface());
    }

    @Override
    public void surfaceChanged(@NonNull SurfaceHolder surfaceHolder, int format, int width, int height) {
        Log.d(TAG,"FFmpegPlayerActivity surfaceChanged");
        nativeFFmpegPlayer.OnSurfaceChanged(width, height);
    }

    @Override
    public void surfaceDestroyed(@NonNull SurfaceHolder surfaceHolder) {
        Log.d(TAG,"FFmpegPlayerActivity surfaceDestroyed");
        nativeFFmpegPlayer.Stop();
        nativeFFmpegPlayer.UnInit();
    }

    @Override
    public void onPlayerEvent(int msgType, float msgValue) {

    }
}
