package com.example.opengles;

import android.graphics.Point;
import android.os.Build;
import android.os.Bundle;
import android.os.Handler;
import android.util.Log;
import android.view.Display;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceHolder.Callback;
import android.view.SurfaceView;
import android.widget.RelativeLayout;

import androidx.appcompat.app.AppCompatActivity;

public class ShowPngActivity extends AppCompatActivity {
    private SurfaceView surfaceView;
    private RelativeLayout preview_parent_layout;

    private String picPath = "/storage/emulated/0/Download/WeiXin/1.png";

    private Callback previewCallback = new Callback() {
        public void surfaceCreated(SurfaceHolder holder) {
            init(picPath);
            setSurface(holder.getSurface());
        }

        public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
            resetSize(width, height);
        }

        public void surfaceDestroyed(SurfaceHolder holder) {
        }
    };


    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.texture_pic_preview_layout);
        findView();
    }

    private void findView() {
        preview_parent_layout = (RelativeLayout) findViewById(R.id.preview_parent_layout);
        surfaceView = new SurfaceView(ShowPngActivity.this);
        SurfaceHolder mSurfaceHolder = surfaceView.getHolder();
        mSurfaceHolder.addCallback(previewCallback);
        if(Build.VERSION.SDK_INT < Build.VERSION_CODES.HONEYCOMB)
            mSurfaceHolder.setType(SurfaceHolder.SURFACE_TYPE_PUSH_BUFFERS);
        preview_parent_layout.addView(surfaceView, 0);
        Display display = getWindowManager().getDefaultDisplay();
        Point outSize = new Point();
        display.getSize(outSize);
        surfaceView.getLayoutParams().width = outSize.x;
        surfaceView.getLayoutParams().height = outSize.y;
    }

    protected void onStop() {
        super.onStop();
        stop();
    }

    // javah -classpath /disk/as/SDK/platforms/android-32/android.jar:../java -d ../cpp -jni com.example.opengles.ShowPngActivity
    public native void init(String picPath);
    public native void setSurface(Surface surface);
    public native void resetSize(int width, int height);
    public native void stop();
}
