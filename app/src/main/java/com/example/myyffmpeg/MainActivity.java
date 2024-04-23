package com.example.myyffmpeg;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;

import android.Manifest;
import android.annotation.SuppressLint;
import android.content.pm.PackageManager;
import android.os.Build;
import android.os.Bundle;
import android.os.Environment;
import android.util.Log;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
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
    private Button mBtn3;
    private Button mBtn4;
    private Button mBtn5;
    private SurfaceView mSurfaceView = null;
    private Surface mSurface = null;


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
            String inputFilePath = FileUtils.getModelFilePath(getApplication(), "woman.mp4");
            @SuppressLint("SimpleDateFormat") SimpleDateFormat formatter
                    = new SimpleDateFormat("yy_MM_dd_HH_mm_ss");
            Date curDate = new Date(System.currentTimeMillis());//获取当前时间
            String str = formatter.format(curDate);
            String aviDir = DirectoryPath.createVideoDir(getApplication());
            String aviName = aviDir + File.pathSeparator + str + ".avi";
            mCallJni.mp4CAvi(inputFilePath, aviName);
            SaveMediaUtils.saveImgFileToAlbum(getApplication(), aviName);
        });

        mBtn3.setOnClickListener(v -> {
            String inputFilePath = FileUtils.getModelFilePath(getApplication(), "video.mp4");
            String pngFilePath = FileUtils.getModelFilePath(getApplication(), "water.png");
            String waterMakerDir = DirectoryPath.createVideoDir(getApplication());
            String waterMakerFilePath = waterMakerDir + File.pathSeparator + "watermark.mp4";
            mCallJni.waterMark(inputFilePath, pngFilePath, waterMakerFilePath);
        });
        mBtn4.setOnClickListener(v -> {
            if (mBtn4.isSelected()) {
                mCallJni.stopAudio();
                mBtn4.setSelected(false);
                mBtn4.setText("音频播放");
            } else {
                String audioUrl = FileUtils.getModelFilePath(getApplication(), "liudehua.mp3");
                mCallJni.playAudio(audioUrl);
                mBtn4.setSelected(true);
                mBtn4.setText("停止音频播放");
            }
        });

        mBtn5.setOnClickListener(v -> {
            Log.e(TAG,"mBtn5=======");
            if (mBtn5.isSelected()) {
                mBtn5.setSelected(false);
                mBtn5.setText("ffmpeg播放视频");
                mCallJni.stopVideo();
            } else {
                String videoUrl = FileUtils.getModelFilePath(getApplication(), "woman.mp4");
                mBtn5.setSelected(true);
                mBtn5.setText("ffmpeg播放停止");
                mCallJni.playVideo(videoUrl, mSurface);
            }
        });

        final SurfaceHolder surfaceViewHolder = mSurfaceView.getHolder();
        surfaceViewHolder.addCallback(new SurfaceHolder.Callback() {
            @Override
            public void surfaceCreated(@NonNull SurfaceHolder holder) {
                mSurface = surfaceViewHolder.getSurface();
            }

            @Override
            public void surfaceChanged(@NonNull SurfaceHolder holder, int format, int width, int height) {

            }

            @Override
            public void surfaceDestroyed(@NonNull SurfaceHolder holder) {

            }
        });


    }

    private void initData() {
        mCallJni = new CallJni();

    }

    private void initView() {
        mSampleText = mBinding.sampleText;
        mBtn1 = mBinding.btn1;
        mBtn2 = mBinding.btn2;
        mBtn3 = mBinding.btn3;
        mBtn4 = mBinding.btn4;
        mBtn5 = mBinding.btn5;
        mSurfaceView = mBinding.surface;

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