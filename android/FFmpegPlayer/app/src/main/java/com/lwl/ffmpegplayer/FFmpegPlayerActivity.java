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
import android.widget.RelativeLayout;
import android.widget.Toast;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;

import com.lwl.ffmpegplayer.util.CommonUtils;

public class FFmpegPlayerActivity extends AppCompatActivity implements SurfaceHolder.Callback, NativeFFmpegPlayer.EventCallback {
    private static final String TAG = "FFmpegPlayerActivity";
    private static final String[] REQUEST_PERMISSIONS = {
            Manifest.permission.READ_EXTERNAL_STORAGE,
            Manifest.permission.WRITE_EXTERNAL_STORAGE,
            Manifest.permission.MODIFY_AUDIO_SETTINGS,
            Manifest.permission.RECORD_AUDIO,
            Manifest.permission.INTERNET,
    };
    private static final int PERMISSION_REQUEST_CODE = 1;
    private SurfaceView surfaceView = null;
    private NativeFFmpegPlayer nativeFFmpegPlayer = null;
    private String mVideoPath = Environment.getExternalStorageDirectory().getAbsolutePath() + "/amedia/one_piece.mp4";

    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.ffmpeg_player);
        surfaceView = new SurfaceView(FFmpegPlayerActivity.this);
        surfaceView.getHolder().addCallback(this);
        if (!hasPermissionsGranted(REQUEST_PERMISSIONS)) {
            ActivityCompat.requestPermissions(this, REQUEST_PERMISSIONS, PERMISSION_REQUEST_CODE);
        }

        nativeFFmpegPlayer = new NativeFFmpegPlayer();
        nativeFFmpegPlayer.AddEventCallback(this);
        Log.i(TAG,mVideoPath);
        nativeFFmpegPlayer.Init(mVideoPath, 1, 1);
    }

    @Override
    protected void onResume() {
        super.onResume();

    }
    @Override
    public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions, @NonNull int[] grantResults) {
        if (requestCode == PERMISSION_REQUEST_CODE) {
            if (!hasPermissionsGranted(REQUEST_PERMISSIONS)) {
                Toast.makeText(this, "We need the permission: WRITE_EXTERNAL_STORAGE", Toast.LENGTH_SHORT).show();
            }
        } else {
            super.onRequestPermissionsResult(requestCode, permissions, grantResults);
        }
    }

    protected boolean hasPermissionsGranted(String[] permissions) {
        for (String permission : permissions) {
            if (ActivityCompat.checkSelfPermission(this, permission)
                    != PackageManager.PERMISSION_GRANTED) {
                return false;
            }
        }
        return true;
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
