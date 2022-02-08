package com.lwl.ffmpegplayer;

import android.content.Context;
import android.os.Bundle;
import android.os.Environment;
import android.util.Log;
import android.view.MotionEvent;
import android.view.ScaleGestureDetector;
import android.view.SurfaceHolder;
import com.lwl.ffmpegplayer.util.MySurfaceView;
import android.widget.RelativeLayout;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;

public class FFmpegPlayerActivity extends AppCompatActivity implements SurfaceHolder.Callback, NativeFFmpegPlayer.EventCallback, ScaleGestureDetector.OnScaleGestureListener {
    private static final String TAG = "FFmpegPlayerActivity";
    private RelativeLayout relativeLayout;
    private MySurfaceView surfaceView = null;
    private NativeFFmpegPlayer nativeFFmpegPlayer = null;

    private final float TOUCH_SCALE_FACTOR = 180.0f / 320;
    private float mPreviousY;
    private float mPreviousX;
    private int mXAngle;
    private int mYAngle;
    private int mRatioWidth = 0;
    private int mRatioHeight = 0;
    private ScaleGestureDetector mScaleGestureDetector;
    private float mPreScale = 1.0f;
    private float mCurScale = 1.0f;
    private long mLastMultiTouchTime;

    private String mVideoPath = Environment.getExternalStorageDirectory().getAbsolutePath() + "/amedia/one_piece.mp4";

    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.ffmpeg_player);
        relativeLayout = (RelativeLayout) findViewById(R.id.ffmpeg_player_layout);
        surfaceView = new MySurfaceView(FFmpegPlayerActivity.this);
        surfaceView.getHolder().addCallback(this);
        relativeLayout.addView(surfaceView, 0);

        nativeFFmpegPlayer = new NativeFFmpegPlayer();
        nativeFFmpegPlayer.AddEventCallback(this);
        Log.i(TAG, mVideoPath);
        nativeFFmpegPlayer.Init(mVideoPath, 1, 1);

        RelativeLayout.LayoutParams params = new RelativeLayout.LayoutParams(1080, 606);
        params.topMargin = 300;
        params.leftMargin = 0;
        surfaceView.setLayoutParams(params);

        mScaleGestureDetector = new ScaleGestureDetector(this, this);
    }

    @Override
    public void surfaceCreated(@NonNull SurfaceHolder surfaceHolder) {
        Log.d(TAG, "FFmpegPlayerActivity surfaceCreated");
        nativeFFmpegPlayer.OnSurfaceCreated(surfaceHolder.getSurface());
    }

    @Override
    public void surfaceChanged(@NonNull SurfaceHolder surfaceHolder, int format, int width, int height) {
        Log.d(TAG, "FFmpegPlayerActivity surfaceChanged");
        nativeFFmpegPlayer.OnSurfaceChanged(width, height);
        nativeFFmpegPlayer.Play();
    }

    @Override
    public void surfaceDestroyed(@NonNull SurfaceHolder surfaceHolder) {
        Log.d(TAG, "FFmpegPlayerActivity surfaceDestroyed");
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

    @Override
    public boolean onScale(ScaleGestureDetector scaleGestureDetector) {
        float preSpan = scaleGestureDetector.getPreviousSpan();
        float curSpan = scaleGestureDetector.getCurrentSpan();
        if (curSpan < preSpan) {
            mCurScale = mPreScale - (preSpan - curSpan) / 200;
        } else {
            mCurScale = mPreScale + (curSpan - preSpan) / 200;
        }
        mCurScale = Math.max(0.05f, Math.min(mCurScale, 80.0f));
        nativeFFmpegPlayer.SetGesture(mXAngle, mYAngle, mCurScale);

        return false;
    }

    @Override
    public boolean onScaleBegin(ScaleGestureDetector scaleGestureDetector) {
        return true;
    }

    @Override
    public void onScaleEnd(ScaleGestureDetector scaleGestureDetector) {
        mPreScale = mCurScale;
        mLastMultiTouchTime = System.currentTimeMillis();
    }

    @Override
    public boolean onTouchEvent(MotionEvent e) {
        if (e.getPointerCount() == 1) {
            consumeClickEvent(e);
            long currentTimeMillis = System.currentTimeMillis();
            if (currentTimeMillis - mLastMultiTouchTime > 200) {
                float y = e.getY();
                float x = e.getX();
                switch (e.getAction()) {
                    case MotionEvent.ACTION_MOVE:
                        float dy = y - mPreviousY;
                        float dx = x - mPreviousX;
                        mYAngle += dx * TOUCH_SCALE_FACTOR;
                        mXAngle += dy * TOUCH_SCALE_FACTOR;
                }
                mPreviousY = y;
                mPreviousX = x;
                nativeFFmpegPlayer.SetGesture(mXAngle, mYAngle, mCurScale);
            }

        } else {
            mScaleGestureDetector.onTouchEvent(e);
        }

        return true;
    }

    private void consumeClickEvent(MotionEvent event) {
        float touchX = -1, touchY = -1;
        switch (event.getAction()) {
            case MotionEvent.ACTION_UP:
                touchX = event.getX();
                touchY = event.getY();
                nativeFFmpegPlayer.SetTouchLoc(touchX, touchY);
                break;
            default:
                break;
        }
    }
}
