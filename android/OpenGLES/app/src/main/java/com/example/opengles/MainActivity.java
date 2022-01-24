package com.example.opengles;

import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;
import androidx.core.content.ContextCompat;

import android.Manifest;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

import com.example.opengles.databinding.ActivityMainBinding;

public class MainActivity extends AppCompatActivity implements View.OnClickListener {

    // Used to load the 'opengles' library on application startup.
    static {
        System.loadLibrary("opengles");
    }
    static private String TAG = "com.example.opengles.MainActivity";
    private ActivityMainBinding binding;
    private Button show_png;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        setContentView(R.layout.activity_main);
        show_png = (Button) findViewById(R.id.show_png);
        show_png.setOnClickListener(this);


        int READ_EXTERNAL_STORAGE_REQUEST_CODE = 1;
        if (ContextCompat.checkSelfPermission(this, Manifest.permission.READ_EXTERNAL_STORAGE)
                != PackageManager.PERMISSION_GRANTED) {
            Log.i(TAG,"====================2");
            // 申请READ_EXTERNAL_STORAGE权限
            ActivityCompat.requestPermissions(this, new String[]{Manifest.permission.READ_EXTERNAL_STORAGE},
                    READ_EXTERNAL_STORAGE_REQUEST_CODE);
        }
        Log.i(TAG,"====================1");
    }

    @Override
    public void onClick(View view) {
        switch (view.getId()) {
            case R.id.show_png:
                Intent intent = new Intent(MainActivity.this, ShowPngActivity.class);
                startActivity(intent);
                break;
            default:
                break;
        }
    }

}