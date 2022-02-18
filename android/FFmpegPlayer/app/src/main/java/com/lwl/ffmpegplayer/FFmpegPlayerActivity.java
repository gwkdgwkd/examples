package com.lwl.ffmpegplayer;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.content.res.Configuration;
import android.content.res.Resources;
import android.graphics.Color;
import android.graphics.Point;
import android.media.AudioFormat;
import android.media.AudioTrack;
import android.opengl.GLSurfaceView;
import android.os.Build;
import android.os.Bundle;
import android.os.Environment;
import android.util.Log;
import android.view.Display;
import android.view.MotionEvent;
import android.view.ScaleGestureDetector;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.widget.RelativeLayout;
import android.widget.SeekBar;

import androidx.annotation.NonNull;

import com.lwl.ffmpegplayer.util.MediaInfo;
import com.lwl.ffmpegplayer.util.MyButton;
import com.lwl.ffmpegplayer.util.MySeekBar;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

public class FFmpegPlayerActivity extends Activity implements NativeFFmpegPlayer.EventCallback, ScaleGestureDetector.OnScaleGestureListener {
    private static final String TAG = "FFmpegPlayerActivity";
    private RelativeLayout relativeLayout;
    private NativeFFmpegPlayer nativeFFmpegPlayer = null;

    private GLSurfaceView mVideoGLSurfaceView = null;
    private GLSurfaceView mAudioGLSurfaceView = null;
    private SurfaceView mVideoSurfaceView = null;
    private MySeekBar mySeekBar;
    private MyButton myButton;
    private volatile boolean isPlaying = true;
    private boolean isStarted = false;

    private MediaInfo mMediaInfo = null;

    private AudioTrack audioTrack;
    private int minbufsize;

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
    private PlayerInfo playerInfo;

    private String mVideoPath = Environment.getExternalStorageDirectory().getAbsolutePath() + "/amedia/one_piece.mp4";
    private String mVRVideoPath = Environment.getExternalStorageDirectory().getAbsolutePath() + "/amedia/vr.mp4";

    private SurfaceHolder.Callback mVideoRender = new SurfaceHolder.Callback() {
        @Override
        public void surfaceCreated(@NonNull SurfaceHolder surfaceHolder) {
            Log.d(TAG, "FFmpegPlayerActivity surfaceCreated");
            nativeFFmpegPlayer.OnSurfaceCreated(surfaceHolder.getSurface(), 0);
        }

        @Override
        public void surfaceChanged(@NonNull SurfaceHolder surfaceHolder, int format, int width, int height) {
            Log.d(TAG, "FFmpegPlayerActivity surfaceChanged " + width + " " + height);
            nativeFFmpegPlayer.OnSurfaceChanged(width, height, 0);
            if(!isStarted) {
                nativeFFmpegPlayer.Play();
                if (playerInfo.getAudioRenderType() == AudioRenderType.AUDIOTRACK1 ||
                        playerInfo.getAudioRenderType() == AudioRenderType.AUDIOTRACK2) {
                    audioTrack.play();
                }
                if (playerInfo.getAudioRenderType() == AudioRenderType.AUDIOTRACK1) {
                    audioUpdateThread.start();
                }
            }
        }

        @Override
        public void surfaceDestroyed(@NonNull SurfaceHolder surfaceHolder) {
            Log.d(TAG, "FFmpegPlayerActivity surfaceDestroyed");
            nativeFFmpegPlayer.Stop();
            nativeFFmpegPlayer.UnInit();
        }
    };

    private GLSurfaceView.Renderer mVideoGLRender = new GLSurfaceView.Renderer() {
        @Override
        public void onSurfaceCreated(GL10 gl10, EGLConfig eglConfig) {
            Log.d(TAG, "FFmpegPlayerActivity onSurfaceCreated ");
            nativeFFmpegPlayer.OnSurfaceCreated(null, 0);
        }

        @Override
        public void onSurfaceChanged(GL10 gl10, int i, int i1) {
            Log.d(TAG, "FFmpegPlayerActivity surfaceChanged " + i + " " + i1);
            nativeFFmpegPlayer.OnSurfaceChanged(i, i1, 0);
            if(!isStarted) {
                nativeFFmpegPlayer.Play();
                if (playerInfo.getAudioRenderType() == AudioRenderType.AUDIOTRACK1 ||
                        playerInfo.getAudioRenderType() == AudioRenderType.AUDIOTRACK2) {
                    audioTrack.play();
                }
                if (playerInfo.getAudioRenderType() == AudioRenderType.AUDIOTRACK1) {
                    audioUpdateThread.start();
                }
            }
        }

        @Override
        public void onDrawFrame(GL10 gl10) {
            nativeFFmpegPlayer.OnDrawFrame(0);
        }
    };

    private GLSurfaceView.Renderer mAudioGLRender = new GLSurfaceView.Renderer() {
        @Override
        public void onSurfaceCreated(GL10 gl10, EGLConfig eglConfig) {
            nativeFFmpegPlayer.OnSurfaceCreated(null, 1);
        }

        @Override
        public void onSurfaceChanged(GL10 gl10, int w, int h) {
            nativeFFmpegPlayer.OnSurfaceChanged(w, h, 1);
        }

        @Override
        public void onDrawFrame(GL10 gl10) {
            nativeFFmpegPlayer.OnDrawFrame(1);
        }
    };

    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        Intent intent = getIntent();
        playerInfo = (PlayerInfo) getIntent().getSerializableExtra("playerInfo");
        if (playerInfo.getViedoRenderType() == ViedoRenderType.ANWINDOW) {
            playerInfo.setEffectType(EffectType.NOEFFECT);
            playerInfo.setViewType(ViewType.SURFACEVIEW);
            if (playerInfo.getScaleType() == ScaleType.OPENGLES) {
                playerInfo.setScaleType(ScaleType.FFMPEG);
            }
        }
        this.requestWindowFeature(Window.FEATURE_NO_TITLE);
        getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN, WindowManager.LayoutParams.FLAG_FULLSCREEN);
        if (playerInfo.getEffectType() == EffectType.VISUALAUDIO) {
            setContentView(R.layout.ffmpeg_player_with_visual_audio);
            mAudioGLSurfaceView = findViewById(R.id.audio_glsurface_view);
            mAudioGLSurfaceView.setEGLContextClientVersion(3);
            mAudioGLSurfaceView.setRenderer(mAudioGLRender);
            mAudioGLSurfaceView.setRenderMode(GLSurfaceView.RENDERMODE_WHEN_DIRTY);

            playerInfo.setViewType(ViewType.GLSURFACEVIEW);
            mVideoGLSurfaceView = findViewById(R.id.video_gl_view);
            mVideoGLSurfaceView.setEGLContextClientVersion(3);
            mVideoGLSurfaceView.setRenderer(mVideoGLRender);
            mVideoGLSurfaceView.setRenderMode(GLSurfaceView.RENDERMODE_WHEN_DIRTY);
            mySeekBar = (MySeekBar) findViewById(R.id.surface_view_seekbar1);
            myButton = (MyButton) findViewById(R.id.surface_view_button1);
        } else if (playerInfo.getViewType() == ViewType.SURFACEVIEW) {
            setContentView(R.layout.ffmpeg_player_surfaceview);
            mVideoSurfaceView = findViewById(R.id.video_surface_view);
            mVideoSurfaceView.getHolder().addCallback(mVideoRender);
            mySeekBar = (MySeekBar) findViewById(R.id.surface_view_seekbar);
            myButton = (MyButton) findViewById(R.id.surface_view_button);
        } else if (playerInfo.getViewType() == ViewType.GLSURFACEVIEW) {
            setContentView(R.layout.ffmpeg_player_glsurfaceview);
            mVideoGLSurfaceView = findViewById(R.id.video_glsurface_view);
            mVideoGLSurfaceView.setEGLContextClientVersion(3);
            mVideoGLSurfaceView.setRenderer(mVideoGLRender);
            mVideoGLSurfaceView.setRenderMode(GLSurfaceView.RENDERMODE_WHEN_DIRTY);
            mySeekBar = (MySeekBar) findViewById(R.id.glsurface_view_seekbar);
            myButton = (MyButton) findViewById(R.id.glsurface_view_button);
        }


        Log.d(TAG, "view_type is " + playerInfo.getViewType().name());
        Log.d(TAG, "audio_render_type is " + playerInfo.getAudioRenderType().name());
        Log.d(TAG, "video_render_type is " + playerInfo.getViedoRenderType().name());
        Log.d(TAG, "effect_type is " + playerInfo.getEffectType().name());
        Log.d(TAG, "scale_type is " + playerInfo.getScaleType().name());

        mySeekBar.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            float pos;

            public void seek(SeekBar seekBar) {
                pos = seekBar.getProgress();
                Log.d(TAG, "seek to " + pos);
                nativeFFmpegPlayer.SeekToPosition(pos);
            }

            @Override
            public void onProgressChanged(SeekBar seekBar, int i, boolean b) {
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {
//                seek(seekBar);
                Log.d(TAG, "seek start");
            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {
                seek(seekBar);
                Log.d(TAG, "seek stop");
            }
        });

        myButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                myButton.changeIsPlay();
                myButton.invalidate();
                if (isPlaying) {
                    isPlaying = false;
                    nativeFFmpegPlayer.Pause();
                    if (playerInfo.getAudioRenderType() == AudioRenderType.AUDIOTRACK1 ||
                            playerInfo.getAudioRenderType() == AudioRenderType.AUDIOTRACK2) {
                        audioTrack.pause();
                    }
                } else {
                    isPlaying = true;
                    nativeFFmpegPlayer.Resume();
                    if (playerInfo.getAudioRenderType() == AudioRenderType.AUDIOTRACK1 ||
                            playerInfo.getAudioRenderType() == AudioRenderType.AUDIOTRACK2) {
                        audioTrack.play();
                    }
                }
                Log.d(TAG, " isPlaying is " + isPlaying);
            }
        });

        nativeFFmpegPlayer = new NativeFFmpegPlayer();
        nativeFFmpegPlayer.AddEventCallback(this);
        if (playerInfo.getEffectType() == EffectType.VR) {
            mVideoPath = mVRVideoPath;
        }
        Log.i(TAG, "video path: " + mVideoPath);
        nativeFFmpegPlayer.Init(mVideoPath, playerInfo.getViewType().ordinal(),
                playerInfo.getAudioRenderType().ordinal(), playerInfo.getViedoRenderType().ordinal(),
                playerInfo.getEffectType().ordinal(), playerInfo.getScaleType().ordinal());
        mMediaInfo = new MediaInfo();
        nativeFFmpegPlayer.GetMediaInfo(mMediaInfo);
        Log.d(TAG, "sample_fmt is " + mMediaInfo.sample_fmt);
        Log.d(TAG, "channels is " + mMediaInfo.channels);
        Log.d(TAG, "sample_rate is " + mMediaInfo.sample_rate);
        Log.d(TAG, "width is " + mMediaInfo.width);
        Log.d(TAG, "height is " + mMediaInfo.height);
        Log.d(TAG, "duration is " + mMediaInfo.duration);
        mySeekBar.setMax((int) mMediaInfo.duration);

        if (playerInfo.getAudioRenderType() == AudioRenderType.AUDIOTRACK1 ||
                playerInfo.getAudioRenderType() == AudioRenderType.AUDIOTRACK2) {
            initAudioTrack();
        }

        if (playerInfo.getViewType() == ViewType.SURFACEVIEW) {
            setViewPosition(mMediaInfo.width, mMediaInfo.height, mVideoSurfaceView);
        } else if (playerInfo.getViewType() == ViewType.GLSURFACEVIEW) {
            setViewPosition(mMediaInfo.width, mMediaInfo.height, mVideoGLSurfaceView);
        }

        mScaleGestureDetector = new ScaleGestureDetector(this, this);
    }

    @Override
    public void onConfigurationChanged(Configuration newConfig) {
        super.onConfigurationChanged(newConfig);
        Log.d(TAG, "onConfigurationChanged");
        if (playerInfo.getViewType() == ViewType.SURFACEVIEW) {
            setViewPosition(mMediaInfo.width, mMediaInfo.height, mVideoSurfaceView);
        } else if (playerInfo.getViewType() == ViewType.GLSURFACEVIEW) {
            setViewPosition(mMediaInfo.width, mMediaInfo.height, mVideoGLSurfaceView);
        }
    }

    private void initAudioTrack() {
        Log.d(TAG, "sample_rate is " + mMediaInfo.sample_rate);
        Log.d(TAG, "channels is " + mMediaInfo.channels);
        Log.d(TAG, "sampleformat is " + mMediaInfo.sample_fmt);
        int samplerate = mMediaInfo.sample_rate;
        int channeltype;
        if (mMediaInfo.channels == 2) { // AV_CH_LAYOUT_STEREO
            channeltype = AudioFormat.CHANNEL_OUT_STEREO;
        } else { // AV_CH_LAYOUT_MONO and other
            channeltype = AudioFormat.CHANNEL_OUT_MONO;
        }
        int sampleformat = AudioFormat.ENCODING_PCM_16BIT;
        minbufsize = AudioTrack.getMinBufferSize(samplerate, channeltype, sampleformat);
        Log.d(TAG, "minbufsize is " + minbufsize);
        audioTrack = new AudioTrack(android.media.AudioManager.STREAM_MUSIC, samplerate, channeltype,
                sampleformat, minbufsize * 2, AudioTrack.MODE_STREAM);
    }

    private final Thread audioUpdateThread = new Thread() {
        public void run() {
            while (true) {
                if (!isPlaying) {
                    try {
                        Thread.sleep(10);
                    } catch (InterruptedException e) {
                        e.printStackTrace();
                    }
                }
                byte[] pcm = new byte[minbufsize];
                int dsize = nativeFFmpegPlayer.GetPcmBuffer(pcm, minbufsize);
                if (audioTrack.write(pcm, 0, dsize) < dsize) {
                    Log.w(null, "Data not written completely");
                }
            }
        }
    };


    private int getNavigationBarHeight() {
        int navigationBarHeight = -1;
        Resources resources = this.getResources();
        int resourceId = resources.getIdentifier("navigation_bar_height", "dimen", "android");
        if (resourceId > 0) {
            navigationBarHeight = resources.getDimensionPixelSize(resourceId);
        }
        return navigationBarHeight;
    }

    private void setViewPosition(int videoWidth, int videoHeight, View view) {
        Log.d(TAG, "video size: " + videoWidth + ", " + videoHeight);

        if (this.getResources().getConfiguration().orientation != Configuration.ORIENTATION_PORTRAIT) {
            getWindow().getDecorView().setSystemUiVisibility(
                    View.SYSTEM_UI_FLAG_LAYOUT_STABLE
                            | View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN
                            | View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION
                            | View.SYSTEM_UI_FLAG_HIDE_NAVIGATION
                            | View.SYSTEM_UI_FLAG_FULLSCREEN
                            | View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY);
        } else {
            getWindow().getDecorView().setSystemUiVisibility(0);
        }

        Display display = getWindowManager().getDefaultDisplay();
        Point size = new Point();
        display.getSize(size);
        int screenWidth = size.x;
        int screenHeight = size.y;
        if (this.getResources().getConfiguration().orientation != Configuration.ORIENTATION_PORTRAIT) {
            screenWidth += getNavigationBarHeight();
        }
        Log.d(TAG, "window size: " + screenWidth + ", " + screenHeight);

        int finalWidth, finalHeight, finalTop, finalLeft;
        float widthScaledRatio = screenWidth * 1.0f / videoWidth;
        float heightScaledRatio = screenHeight * 1.0f / videoHeight;
        float Ratio = (float) videoWidth / (float) videoHeight;
        if (widthScaledRatio > heightScaledRatio) {
            // use heightScaledRatio
            finalWidth = (int) (videoWidth * heightScaledRatio);
            finalHeight = screenHeight;
            finalTop = 0;
            finalLeft = (screenWidth - finalWidth) / 2;
        } else {
            // use widthScaledRatio
            finalWidth = screenWidth;
            finalHeight = (int) (videoHeight * widthScaledRatio);
            finalTop = (screenHeight - finalHeight) / 2;
            finalLeft = 0;
        }
        Log.d(TAG, "final size: " + finalLeft + ", " + finalTop + ", " + finalWidth + ", " + finalHeight);

        RelativeLayout.LayoutParams params = new RelativeLayout.LayoutParams(finalWidth, finalHeight);
        if (playerInfo.getEffectType() != EffectType.VISUALAUDIO) {
            params.topMargin = finalTop;
            params.leftMargin = finalLeft;
        }
        view.setLayoutParams(params);
    }

    @Override
    public void onPlayerEvent(int msgType, float msgValue) {
        switch (msgType) {
            case 0:
                if (isPlaying) {
                    if (playerInfo.getViewType() == ViewType.GLSURFACEVIEW) {
                        mVideoGLSurfaceView.requestRender();
                    }
                    if (playerInfo.getEffectType() == EffectType.VISUALAUDIO) {
                        mAudioGLSurfaceView.requestRender();
                    }
                }
                break;
            case 1:
                mySeekBar.setProgress((int) msgValue);
        }

    }

    @Override
    public void onWritePcm(byte[] pcm, int len) {
        if (audioTrack.write(pcm, 0, len) < len) {
            Log.w(null, "Data not written completely");
        }
        return;
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
