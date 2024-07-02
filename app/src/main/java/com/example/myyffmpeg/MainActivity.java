package com.example.myyffmpeg;

import androidx.appcompat.app.AppCompatActivity;
import androidx.fragment.app.FragmentManager;
import androidx.fragment.app.FragmentTransaction;
import androidx.lifecycle.Observer;
import androidx.lifecycle.ViewModelProviders;

import android.Manifest;
import android.annotation.SuppressLint;
import android.content.pm.PackageManager;
import android.os.Build;
import android.os.Bundle;
import android.widget.Button;
import android.widget.FrameLayout;

import com.example.myyffmpeg.databinding.ActivityMainBinding;
import com.example.myyffmpeg.fragment.FFmpegPlayFragment;
import com.example.myyffmpeg.fragment.MainFragment;

public class MainActivity extends AppCompatActivity {
    private static final String TAG = MainActivity.class.getSimpleName();
    private ActivityMainBinding mBinding;
    private FFmpegPlayFragment mFmpegPlayFragment;
    private FrameLayout mFlMain;
    private FrameLayout mFlPlay;
    private MainFragment mMainFragment;
    private FFViewModel mFfViewModel;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        checkPermission();
        mBinding = ActivityMainBinding.inflate(getLayoutInflater());
        setContentView(mBinding.getRoot());
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

    }

    private void addFragment() {
        mMainFragment = new MainFragment();
        mFmpegPlayFragment = new FFmpegPlayFragment();
        FragmentManager fragmentManager = getSupportFragmentManager();
        FragmentTransaction mFragmentTransaction = fragmentManager.beginTransaction();
        mFragmentTransaction
                .add(mFlMain.getId(), mMainFragment)
                .add(mFlPlay.getId(), mFmpegPlayFragment)
                .hide(mFmpegPlayFragment)
                .commit();


    }

    private void selectionFragment(FFViewModel.FRAGMENT_STATUS status) {
        FragmentManager fragmentManager = getSupportFragmentManager();
        FragmentTransaction fragmentTransaction = fragmentManager.beginTransaction();
        hideTransaction(fragmentTransaction);
        switch (status) {
            case PLAY: {
                if (mFmpegPlayFragment != null) {
                    fragmentTransaction
                            .show(mFmpegPlayFragment);
                    fragmentTransaction.commit();
                }
            }
            break;
            case MAIN: {
                if (mMainFragment != null) {
                    fragmentTransaction
                            .show(mMainFragment);
                    fragmentTransaction.commit();
                }
            }
            break;
        }
    }

    private void hideTransaction(FragmentTransaction ftr) {
        if (mFmpegPlayFragment != null) {
            ftr.hide(mFmpegPlayFragment);
        }
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