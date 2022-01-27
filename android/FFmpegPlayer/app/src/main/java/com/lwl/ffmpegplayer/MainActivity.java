package com.lwl.ffmpegplayer;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

import com.lwl.ffmpegplayer.databinding.ActivityMainBinding;

public class MainActivity extends AppCompatActivity implements View.OnClickListener{


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
    public void onClick(View view) {
        switch (view.getId()) {
            case R.id.b_play:
                Intent play = new Intent(MainActivity.this, FFmpegPlayerActivity.class);
                startActivity(play);
                break;
        }
    }
}