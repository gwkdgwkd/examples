package com.lwl.ffmpegplayer;


import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;

import android.Manifest;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.Toast;

import com.lwl.ffmpegplayer.databinding.ActivityMainBinding;
import com.lwl.ffmpegplayer.util.CommonUtils;

public class MainActivity extends AppCompatActivity implements View.OnClickListener {

    private static final String[] REQUEST_PERMISSIONS = {
            Manifest.permission.WRITE_EXTERNAL_STORAGE,
    };
    private static final int PERMISSION_REQUEST_CODE = 1;
    private ActivityMainBinding binding;
    private Button start_play;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        binding = ActivityMainBinding.inflate(getLayoutInflater());
        setContentView(binding.getRoot());

        start_play = (Button) findViewById(R.id.b_play);
        start_play.setOnClickListener(this);
    }

    @Override
    protected void onResume() {
        super.onResume();
        CommonUtils.copyAssetsDirToSDCard(this, "amedia", "/sdcard");
        if (!hasPermissionsGranted(REQUEST_PERMISSIONS)) {
            ActivityCompat.requestPermissions(this, REQUEST_PERMISSIONS, PERMISSION_REQUEST_CODE);
        }
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
    public void onClick(View view) {
        switch (view.getId()) {
            case R.id.b_play:
                Intent play = new Intent(MainActivity.this, FFmpegPlayerActivity.class);
                PlayerInfo playerInfo = new PlayerInfo(ViewType.GLSURFACEVIEW,
                                                       AudioRenderType.AUDIOTRACK2,
                                                       ViedoRenderType.OPENGLES,
                                                       EffectType.VISUALAUDIO,
                                                       ScaleType.FFMPEG);
                play.putExtra("playerInfo", playerInfo);
                startActivity(play);
                break;
        }
    }
}