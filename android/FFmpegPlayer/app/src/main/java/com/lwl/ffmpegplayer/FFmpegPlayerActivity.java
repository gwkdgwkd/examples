package com.lwl.ffmpegplayer;

import android.Manifest;
import android.content.pm.PackageManager;
import android.graphics.Point;
import android.os.Build;
import android.os.Bundle;
import android.os.Environment;
import android.util.Log;
import android.view.Display;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.widget.RelativeLayout;
import android.widget.Toast;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;

import com.lwl.ffmpegplayer.util.CommonUtils;

public class FFmpegPlayerActivity extends AppCompatActivity implements SurfaceHolder.Callback, NativeFFmpegPlayer.EventCallback {
    private static final String TAG = "FFmpegPlayerActivity";
    private RelativeLayout relativeLayout;
    private SurfaceView surfaceView = null;
    private NativeFFmpegPlayer nativeFFmpegPlayer = null;
    private String mVideoPath = Environment.getExternalStorageDirectory().getAbsolutePath() + "/amedia/one_piece.mp4";

    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.ffmpeg_player);
        relativeLayout = (RelativeLayout) findViewById(R.id.ffmpeg_player_layout);
        surfaceView = new SurfaceView(FFmpegPlayerActivity.this);
        surfaceView.getHolder().addCallback(this);
        relativeLayout.addView(surfaceView, 0);

        nativeFFmpegPlayer = new NativeFFmpegPlayer();
        nativeFFmpegPlayer.AddEventCallback(this);
        Log.i(TAG,mVideoPath);
        nativeFFmpegPlayer.Init(mVideoPath, 1, 1);

        RelativeLayout.LayoutParams params = new RelativeLayout.LayoutParams(1080,606);
        params.topMargin = 300;
        params.leftMargin = 0;
        surfaceView.setLayoutParams(params);

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
        nativeFFmpegPlayer.Play();
    }

    @Override
    public void surfaceDestroyed(@NonNull SurfaceHolder surfaceHolder) {
        Log.d(TAG,"FFmpegPlayerActivity surfaceDestroyed");
        nativeFFmpegPlayer.Stop();
        nativeFFmpegPlayer.UnInit();
    }

    @Override
    public void onPlayerEvent(int msgType, float msgValue) {
        Log.d(TAG, "================----- msgType " + msgType + ", msgValue " + msgValue);
//        RelativeLayout.LayoutParams params = new RelativeLayout.LayoutParams(1080,606);
//        params.topMargin = 300;
//        params.leftMargin = 0;
//        surfaceView.setLayoutParams(params);
    }
}
