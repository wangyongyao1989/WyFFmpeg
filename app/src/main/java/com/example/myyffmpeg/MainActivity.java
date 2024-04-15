package com.example.myyffmpeg;

import androidx.appcompat.app.AppCompatActivity;

import android.Manifest;
import android.annotation.SuppressLint;
import android.content.pm.PackageManager;
import android.os.Build;
import android.os.Bundle;
import android.util.Log;
import android.widget.Button;
import android.widget.TextView;

import com.example.myyffmpeg.databinding.ActivityMainBinding;
import com.example.myyffmpeg.utils.DirectoryPath;
import com.example.myyffmpeg.utils.FileUtils;
import com.example.myyffmpeg.utils.SaveMediaUtils;

import java.io.File;
import java.text.SimpleDateFormat;
import java.util.Date;

public class MainActivity extends AppCompatActivity {
    private static final String TAG = MainActivity.class.getSimpleName();
    private ActivityMainBinding mBinding;
    private TextView mSampleText;
    private CallJni mCallJni;
    private Button mBtn1;
    private Button mBtn2;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        checkPermission();

        mBinding = ActivityMainBinding.inflate(getLayoutInflater());
        setContentView(mBinding.getRoot());

        initView();
        initData();
        initListener();
    }

    @SuppressLint("SetTextI18n")
    private void initListener() {
        mBtn1.setOnClickListener(v -> {
            mSampleText.setText(mCallJni.callStringForJNI()
                    + mCallJni.callIntForJNI()
                    + mCallJni.getFFmpegVersion());
        });
        mBtn2.setOnClickListener(v -> {
            String modelFilePath = FileUtils.getModelFilePath(getApplication(), "woman.mp4");
            @SuppressLint("SimpleDateFormat") SimpleDateFormat formatter
                    = new SimpleDateFormat("yy_MM_dd_HH_mm_ss");
            Date curDate = new Date(System.currentTimeMillis());//获取当前时间
            String str = formatter.format(curDate);
            String aviDir = DirectoryPath.createSDCardDataDir(getApplication());
            String aviName = aviDir + File.pathSeparator + str + ".avi";
            mCallJni.mp4CAvi(modelFilePath,aviName);
            SaveMediaUtils.saveImgFileToAlbum(getApplication(), aviName);
        });


    }

    private void initData() {
        mCallJni = new CallJni();

    }

    private void initView() {
        mSampleText = mBinding.sampleText;
        mBtn1 = mBinding.btn1;
        mBtn2 = mBinding.btn2;
    }

    public boolean checkPermission() {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M && checkSelfPermission(
                android.Manifest.permission.WRITE_EXTERNAL_STORAGE) != PackageManager.PERMISSION_GRANTED) {
            requestPermissions(new String[]{
                    android.Manifest.permission.READ_EXTERNAL_STORAGE,
                    Manifest.permission.WRITE_EXTERNAL_STORAGE,
            }, 1);

        }
        return false;
    }
}