package com.example.audio;

import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;
import androidx.core.content.ContextCompat;

import android.Manifest;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;

public class MainActivity extends AppCompatActivity implements View.OnClickListener {

    // Used to load the 'audio' library on application startup.
    static {
        System.loadLibrary("audio");
    }

    static private String TAG = "com.example.audio.MainActivity";
    private Button startPlayPcm;
    private Button stopPlayPcm;
    private Button startRecorderPcm;
    private Button stopRecorderPcm;


    public MainActivity() {
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        startPlayPcm = findViewById(R.id.start_play_pcm);
        stopPlayPcm = findViewById(R.id.stop_play_pcm);
        startRecorderPcm = findViewById(R.id.start_recorder_pcm);
        stopRecorderPcm = findViewById(R.id.stop_recorder_pcm);
        startPlayPcm.setOnClickListener(this);
        stopPlayPcm.setOnClickListener(this);
        startRecorderPcm.setOnClickListener(this);
        stopRecorderPcm.setOnClickListener(this);

        int READ_EXTERNAL_STORAGE_REQUEST_CODE = 1;
        if (ContextCompat.checkSelfPermission(this, Manifest.permission.READ_EXTERNAL_STORAGE)
                != PackageManager.PERMISSION_GRANTED) {
            // 申请READ_EXTERNAL_STORAGE权限
            ActivityCompat.requestPermissions(this, new String[]{Manifest.permission.READ_EXTERNAL_STORAGE},
                    READ_EXTERNAL_STORAGE_REQUEST_CODE);
        }

        int WRITE_EXTERNAL_STORAGE_REQUEST_CODE = 2;
        if (ContextCompat.checkSelfPermission(this, Manifest.permission.WRITE_EXTERNAL_STORAGE)
                != PackageManager.PERMISSION_GRANTED) {
            // 申请WRITE_EXTERNAL_STORAGE权限
            ActivityCompat.requestPermissions(this, new String[]{Manifest.permission.WRITE_EXTERNAL_STORAGE},
                    WRITE_EXTERNAL_STORAGE_REQUEST_CODE);
        }

        int RECORD_AUDIO_CODE = 3;
        if (ContextCompat.checkSelfPermission(this, Manifest.permission.RECORD_AUDIO)
                != PackageManager.PERMISSION_GRANTED) {
            // 申请WRITE_EXTERNAL_STORAGE权限
            ActivityCompat.requestPermissions(this, new String[]{Manifest.permission.RECORD_AUDIO},
                    RECORD_AUDIO_CODE);
        }



    }

    @Override
    public void onClick(View view) {
        switch (view.getId()) {
            case R.id.start_play_pcm:
                onClickStartPlayPcm(view);
                break;
            case R.id.stop_play_pcm:
                onClickStopPlayPcm(view);
                break;
            case R.id.start_recorder_pcm:
                onClickStartRecorderPcm(view);
                break;
            case R.id.stop_recorder_pcm:
                onClickStopRecorderPcm(view);
                break;
            default:
                break;
        }
    }

    private void onClickStartPlayPcm(View view) {
        Log.v(TAG, " : ============ start play pcm");
        startPlayPcm();
    }

    private void onClickStopPlayPcm(View view) {
        Log.v(TAG, " : ============ stop play pcm");
        stopPlayPcm();
    }

    private void onClickStartRecorderPcm(View view) {
        Log.v(TAG, " : ============ start recorder pcm");
        startRecorderPcm();
    }

    private void onClickStopRecorderPcm(View view) {
        Log.v(TAG, " : ============ stop recorder pcm");
        stopRecorderPcm();
    }

    /**
     * A native method that is implemented by the 'audio' native library,
     * which is packaged with this application.
     */
    // cd java
    // javah -classpath ../java -d ../cpp -jni com.example.audio.MainActivity
    native public void startPlayPcm();
    native public void stopPlayPcm();
    native public void startRecorderPcm();
    native public void stopRecorderPcm();
}