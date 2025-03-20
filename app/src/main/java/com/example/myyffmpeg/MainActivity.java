package com.example.myyffmpeg;

import androidx.appcompat.app.ActionBar;
import androidx.appcompat.app.AppCompatActivity;
import androidx.fragment.app.FragmentManager;
import androidx.fragment.app.FragmentTransaction;
import androidx.lifecycle.Observer;
import androidx.lifecycle.ViewModelProviders;

import android.Manifest;
import android.annotation.SuppressLint;
import android.content.pm.ActivityInfo;
import android.content.pm.PackageManager;
import android.os.Build;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.FrameLayout;

import com.example.myyffmpeg.databinding.ActivityMainBinding;
import com.example.myyffmpeg.fragment.FFmpegPlayFragment;
import com.example.myyffmpeg.fragment.H264Fragment;
import com.example.myyffmpeg.fragment.HevcH265Fragment;
import com.example.myyffmpeg.fragment.MainFragment;
import com.example.myyffmpeg.fragment.OpenGLCameraFboFragment;
import com.example.myyffmpeg.fragment.OpenGLCameraFragment;
import com.example.myyffmpeg.fragment.RtmpFragment;

public class MainActivity extends AppCompatActivity {
    private static final String TAG = MainActivity.class.getSimpleName();
    private ActivityMainBinding mBinding;
    private FFmpegPlayFragment mFmpegPlayFragment;
    private FrameLayout mFlMain;
    private FrameLayout mFlPlay;
    private MainFragment mMainFragment;
    private FFViewModel mFfViewModel;
    private RtmpFragment mRtmpFragment;
    private OpenGLCameraFragment mGLCameraFragment;
    private OpenGLCameraFboFragment mFboFragment;
    private HevcH265Fragment mHevcH265Fragment;
    private H264Fragment mH264Fragment;

    private FrameLayout mFlRtmp;
    private FrameLayout mFlGlCamera;
    private FrameLayout mFlGlFbo;
    private FrameLayout mFlHevcH265;
    private FrameLayout mFlH264;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        checkPermission();
        mBinding = ActivityMainBinding.inflate(getLayoutInflater());
        setContentView(mBinding.getRoot());
        ActionBar supportActionBar = getSupportActionBar();
        if (supportActionBar != null) {
            supportActionBar.hide();
        }
        if (getRequestedOrientation() != ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE) {
            setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE);
        }
        getWindow().getDecorView().setSystemUiVisibility(View.SYSTEM_UI_FLAG_FULLSCREEN);//隐藏状态栏
        initView();
        initData();
        initObserver();
        initListener();
        addFragment();
    }

    private void initObserver() {
        mFfViewModel = ViewModelProviders.of(this).get(FFViewModel.class);

        mFfViewModel.getSwitchFragment().observe(this, fragmentStatus -> {
            selectionFragment(fragmentStatus);
        });
    }


    @SuppressLint("SetTextI18n")
    private void initListener() {

    }

    private void initData() {

    }

    private void initView() {
        mFlMain = mBinding.flMain;
        mFlPlay = mBinding.flPlay;
        mFlRtmp = mBinding.flRtmp;
        mFlGlCamera = mBinding.flGlCamera;
        mFlGlFbo = mBinding.flGlFbo;
        mFlHevcH265 = mBinding.flHevcH265;
        mFlH264 = mBinding.flH264;

    }

    private void addFragment() {
        mMainFragment = new MainFragment();
        FragmentManager fragmentManager = getSupportFragmentManager();
        FragmentTransaction mFragmentTransaction = fragmentManager.beginTransaction();
        mFragmentTransaction
                .add(mFlMain.getId(), mMainFragment)
                .commit();


    }

    private void selectionFragment(FFViewModel.FRAGMENT_STATUS status) {
        FragmentManager fragmentManager = getSupportFragmentManager();
        FragmentTransaction fragmentTransaction = fragmentManager.beginTransaction();
        hideTransaction(fragmentTransaction);
        switch (status) {
            case PLAY: {
                if (mFmpegPlayFragment == null) {
                    mFmpegPlayFragment = new FFmpegPlayFragment();
                    fragmentTransaction
                            .add(mFlPlay.getId(), mFmpegPlayFragment);
                }
                fragmentTransaction.show(mFmpegPlayFragment);
                fragmentTransaction.commit();
            }
            break;
            case MAIN: {
                fragmentTransaction.show(mMainFragment);
                fragmentTransaction.commit();
            }
            break;
            case RTMP: {
                if (mRtmpFragment == null) {
                    mRtmpFragment = new RtmpFragment();
                    fragmentTransaction
                            .add(mFlRtmp.getId(), mRtmpFragment);
                }
                fragmentTransaction.show(mRtmpFragment);
                fragmentTransaction.commit();
            }
            break;
            case OPENGL_CAMERA: {
                if (mGLCameraFragment == null) {
                    mGLCameraFragment = new OpenGLCameraFragment();
                    fragmentTransaction
                            .add(mFlGlCamera.getId(), mGLCameraFragment);
                }
                fragmentTransaction.show(mGLCameraFragment);
                fragmentTransaction.commit();
            }
            break;
            case OPENGL_CAMERA_FBO: {
                if (mFboFragment == null) {
                    mFboFragment = new OpenGLCameraFboFragment();
                    fragmentTransaction
                            .add(mFlGlFbo.getId(), mFboFragment);
                }
                fragmentTransaction.show(mFboFragment);
                fragmentTransaction.commit();
            }
            break;
            case HEVC_H265: {
                if (mHevcH265Fragment == null) {
                    mHevcH265Fragment = new HevcH265Fragment();
                    fragmentTransaction
                            .add(mFlHevcH265.getId(), mHevcH265Fragment);
                }
                fragmentTransaction.show(mHevcH265Fragment);
                fragmentTransaction.commit();
            }
            break;
            case H264: {
                if (mH264Fragment == null) {
                    mH264Fragment = new H264Fragment();
                    fragmentTransaction
                            .add(mFlH264.getId(), mH264Fragment);
                }
                fragmentTransaction.show(mH264Fragment);
                fragmentTransaction.commit();
            }
            break;
        }
    }

    private void hideTransaction(FragmentTransaction ftr) {
        if (mMainFragment != null) {
            ftr.hide(mMainFragment);
        }
        if (mFmpegPlayFragment != null) {
            ftr.hide(mFmpegPlayFragment);
        }
        if (mRtmpFragment != null) {
            ftr.hide(mRtmpFragment);
        }
        if (mGLCameraFragment != null) {
            ftr.hide(mGLCameraFragment);
        }
        if (mFboFragment != null) {
            ftr.hide(mFboFragment);
        }
        if (mHevcH265Fragment != null) {
            ftr.hide(mHevcH265Fragment);
        }
        if (mH264Fragment != null) {
            ftr.hide(mH264Fragment);
        }
    }

    public boolean checkPermission() {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M && checkSelfPermission(
                android.Manifest.permission.WRITE_EXTERNAL_STORAGE) != PackageManager.PERMISSION_GRANTED) {
            requestPermissions(new String[]{
                    android.Manifest.permission.READ_EXTERNAL_STORAGE,
                    Manifest.permission.CAMERA,
                    Manifest.permission.WRITE_EXTERNAL_STORAGE,
                    Manifest.permission.RECORD_AUDIO,
            }, 1);

        }
        return false;
    }
}