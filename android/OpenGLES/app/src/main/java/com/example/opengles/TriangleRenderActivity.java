package com.example.opengles;

import android.graphics.Point;
import android.os.Build;
import android.os.Bundle;
import android.view.Display;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.widget.RelativeLayout;

import androidx.appcompat.app.AppCompatActivity;

// https://www.cnblogs.com/tony-yang-flutter/p/15565918.html
public class TriangleRenderActivity extends AppCompatActivity {
    static {
        System.loadLibrary("trianglerender");
    }
    private SurfaceView triangleSurfaceView;
    private SurfaceHolder holder = null;
    private RelativeLayout triangleRenderlayout;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.triangle_render_layout);
        triangleRenderlayout = (RelativeLayout) findViewById(R.id.triangle_render_layout);
        triangleSurfaceView = new SurfaceView(TriangleRenderActivity.this);
        holder = triangleSurfaceView.getHolder();
        holder.addCallback(new SurfaceHolder.Callback() {

            @Override
            public void surfaceCreated(SurfaceHolder holder) {

            }

            @Override
            public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
                createGraphical(width,height,holder.getSurface());
            }

            @Override
            public void surfaceDestroyed(SurfaceHolder holder) {

            }
        });
        if(Build.VERSION.SDK_INT < Build.VERSION_CODES.HONEYCOMB)
            holder.setType(SurfaceHolder.SURFACE_TYPE_PUSH_BUFFERS);
        triangleRenderlayout.addView(triangleSurfaceView, 0);
        Display display = getWindowManager().getDefaultDisplay();
        Point outSize = new Point();
        display.getSize(outSize);
        triangleSurfaceView.getLayoutParams().width = outSize.x;
        triangleSurfaceView.getLayoutParams().height = outSize.y;
    }

    public native void createGraphical(int width, int height, Surface surface);
}
