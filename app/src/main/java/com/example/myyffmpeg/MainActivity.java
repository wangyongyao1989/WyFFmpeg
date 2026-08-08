package com.example.myyffmpeg;

import androidx.activity.OnBackPressedCallback;
import androidx.appcompat.app.ActionBar;
import androidx.appcompat.app.AppCompatActivity;
import androidx.fragment.app.Fragment;
import androidx.fragment.app.FragmentManager;
import androidx.fragment.app.FragmentTransaction;
import androidx.lifecycle.ViewModelProviders;

import android.Manifest;
import android.annotation.SuppressLint;
import android.content.pm.ActivityInfo;
import android.content.pm.PackageManager;
import android.os.Build;
import android.os.Bundle;
import android.view.View;

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
    private FFViewModel mFfViewModel;
    private Fragment currentFragment;

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
        getWindow().getDecorView().setSystemUiVisibility(View.SYSTEM_UI_FLAG_FULLSCREEN);
        initObserver();
        
        if (savedInstanceState == null) {
            selectionFragment(FFViewModel.FRAGMENT_STATUS.MAIN);
        }

        getOnBackPressedDispatcher().addCallback(this, new OnBackPressedCallback(true) {
            @Override
            public void handleOnBackPressed() {
                if (currentFragment instanceof MainFragment) {
                    finish();
                } else {
                    selectionFragment(FFViewModel.FRAGMENT_STATUS.MAIN);
                }
            }
        });
    }

    private void initObserver() {
        mFfViewModel = ViewModelProviders.of(this).get(FFViewModel.class);
        mFfViewModel.getSwitchFragment().observe(this, this::selectionFragment);
    }

    private void selectionFragment(FFViewModel.FRAGMENT_STATUS status) {
        FragmentManager fragmentManager = getSupportFragmentManager();
        FragmentTransaction fragmentTransaction = fragmentManager.beginTransaction();
        
        Fragment targetFragment = null;
        String tag = status.name();
        targetFragment = fragmentManager.findFragmentByTag(tag);

        if (targetFragment == null) {
            switch (status) {
                case MAIN:
                    targetFragment = new MainFragment();
                    break;
                case PLAY:
                    targetFragment = new FFmpegPlayFragment();
                    break;
                case RTMP:
                    targetFragment = new RtmpFragment();
                    break;
                case OPENGL_CAMERA:
                    targetFragment = new OpenGLCameraFragment();
                    break;
                case OPENGL_CAMERA_FBO:
                    targetFragment = new OpenGLCameraFboFragment();
                    break;
                case HEVC_H265:
                    targetFragment = new HevcH265Fragment();
                    break;
                case H264:
                    targetFragment = new H264Fragment();
                    break;
            }
        }

        if (currentFragment != null) {
            fragmentTransaction.hide(currentFragment);
        }

        if (targetFragment != null) {
            if (!targetFragment.isAdded()) {
                fragmentTransaction.add(R.id.fragment_container, targetFragment, tag);
            } else {
                fragmentTransaction.show(targetFragment);
            }
            currentFragment = targetFragment;
        }
        
        fragmentTransaction.commit();
    }

    public boolean checkPermission() {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M && (
                checkSelfPermission(Manifest.permission.WRITE_EXTERNAL_STORAGE) != PackageManager.PERMISSION_GRANTED ||
                checkSelfPermission(Manifest.permission.CAMERA) != PackageManager.PERMISSION_GRANTED ||
                checkSelfPermission(Manifest.permission.RECORD_AUDIO) != PackageManager.PERMISSION_GRANTED)) {
            requestPermissions(new String[]{
                    Manifest.permission.READ_EXTERNAL_STORAGE,
                    Manifest.permission.CAMERA,
                    Manifest.permission.WRITE_EXTERNAL_STORAGE,
                    Manifest.permission.RECORD_AUDIO,
            }, 1);
        }
        return false;
    }
}