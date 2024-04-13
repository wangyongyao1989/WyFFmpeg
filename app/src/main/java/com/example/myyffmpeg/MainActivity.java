package com.example.myyffmpeg;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.widget.TextView;

import com.example.myyffmpeg.databinding.ActivityMainBinding;

public class MainActivity extends AppCompatActivity {
    private ActivityMainBinding binding;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        binding = ActivityMainBinding.inflate(getLayoutInflater());
        setContentView(binding.getRoot());

        CallJni callJni = new CallJni();
        TextView tv = binding.sampleText;
        tv.setText(callJni.callStringForJNI() + callJni.callIntForJNI());
    }

}