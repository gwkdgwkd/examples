package com.lwl.ffmpegplayer;

import android.opengl.GLSurfaceView;
import android.os.Bundle;
import android.os.Environment;
import android.util.Log;
import android.view.MotionEvent;
import android.view.ScaleGestureDetector;
import android.widget.RelativeLayout;

import androidx.appcompat.app.AppCompatActivity;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

public class FFmpegPlayerActivity1 extends AppCompatActivity implements GLSurfaceView.Renderer, NativeFFmpegPlayer.EventCallback, ScaleGestureDetector.OnScaleGestureListener {
    private static final String TAG = "FFmpegPlayerActivity";
    private RelativeLayout relativeLayout;
    private NativeFFmpegPlayer nativeFFmpegPlayer = null;

    private GLSurfaceView mVideoGLSurfaceView = null;
    private GLSurfaceView mAudioGLSurfaceView = null;

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

    private GLSurfaceView.Renderer mAudioGLRender = new GLSurfaceView.Renderer() {
        @Override
        public void onSurfaceCreated(GL10 gl10, EGLConfig eglConfig) {
            nativeFFmpegPlayer.OnSurfaceCreated(null);
        }

        @Override
        public void onSurfaceChanged(GL10 gl10, int w, int h) {
            nativeFFmpegPlayer.OnSurfaceChanged(w, h);
        }

        @Override
        public void onDrawFrame(GL10 gl10) {
            nativeFFmpegPlayer.OnDrawFrame();
        }
    };

    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.ffmpeg_player_with_visual_audio);
        mVideoGLSurfaceView = findViewById(R.id.video_surface_view);
        mVideoGLSurfaceView.setEGLContextClientVersion(3);
        mVideoGLSurfaceView.setRenderer(this);
        mVideoGLSurfaceView.setRenderMode(GLSurfaceView.RENDERMODE_WHEN_DIRTY);

        RelativeLayout.LayoutParams params = new RelativeLayout.LayoutParams(1080, 606);
        params.topMargin = 300;
        params.leftMargin = 0;
        mVideoGLSurfaceView.setLayoutParams(params);

//        mAudioGLSurfaceView = findViewById(R.id.audio_surface_view);
//        mAudioGLSurfaceView.setEGLContextClientVersion(3);
//        mAudioGLSurfaceView.setRenderer(mAudioGLRender);
//        mAudioGLSurfaceView.setRenderMode(GLSurfaceView.RENDERMODE_WHEN_DIRTY);

        nativeFFmpegPlayer = new NativeFFmpegPlayer();
        nativeFFmpegPlayer.AddEventCallback(this);
        Log.i(TAG, mVideoPath);
        nativeFFmpegPlayer.Init(mVideoPath, 1, 1);

        mScaleGestureDetector = new ScaleGestureDetector(this, this);
    }

    @Override
    public void onPlayerEvent(int msgType, float msgValue) {
        mVideoGLSurfaceView.requestRender();
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

    @Override
    public void onSurfaceCreated(GL10 gl10, EGLConfig eglConfig) {
        nativeFFmpegPlayer.OnSurfaceCreated(eglConfig.getClass());
    }

    @Override
    public void onSurfaceChanged(GL10 gl10, int i, int i1) {
        nativeFFmpegPlayer.OnSurfaceChanged(i, i1);
        nativeFFmpegPlayer.Play();
    }

    @Override
    public void onDrawFrame(GL10 gl10) {
        nativeFFmpegPlayer.OnDrawFrame();
    }
}
