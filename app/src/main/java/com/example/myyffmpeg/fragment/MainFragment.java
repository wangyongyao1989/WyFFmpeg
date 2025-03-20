package com.example.myyffmpeg.fragment;

import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.lifecycle.ViewModelProviders;

import com.example.myyffmpeg.FFViewModel;
import com.example.myyffmpeg.databinding.FragmentMainLayoutBinding;

public class MainFragment extends BaseFragment {

    private com.example.myyffmpeg.databinding.FragmentMainLayoutBinding mBinding;
    private Button mBtnFfPlay;
    private FFViewModel mFfViewModel;
    private Button mBtnFfRtmp;
    private Button mGlCamera;
    private Button mBtnFfGlFbo;
    private Button mBtnFfH265;
    private Button mBtnFfH264;

    @Override
    public View getLayoutDataBing(@NonNull LayoutInflater inflater
            , @Nullable ViewGroup container, @Nullable Bundle savedInstanceState) {
        mBinding = FragmentMainLayoutBinding.inflate(inflater);
        return mBinding.getRoot();
    }

    @Override
    public void initView() {
        mBtnFfPlay = mBinding.btnFfPlay;
        mBtnFfRtmp = mBinding.btnFfRtmp;
        mGlCamera = mBinding.btnFfGlCamera;
        mBtnFfGlFbo = mBinding.btnFfGlFbo;
        mBtnFfH265 = mBinding.btnFfH265;
        mBtnFfH264 = mBinding.btnFfH264;
    }

    @Override
    public void initData() {

    }

    @Override
    public void initObserver() {
        mFfViewModel = ViewModelProviders.of(requireActivity())
                .get(FFViewModel.class);
    }

    @Override
    public void initListener() {
        mBtnFfPlay.setOnClickListener(view -> {
            mFfViewModel.getSwitchFragment().postValue(FFViewModel.FRAGMENT_STATUS.PLAY);
        });
        mBtnFfRtmp.setOnClickListener(view -> {
            mFfViewModel.getSwitchFragment().postValue(FFViewModel.FRAGMENT_STATUS.RTMP);
        });
        mGlCamera.setOnClickListener(view -> {
            mFfViewModel.getSwitchFragment().postValue(FFViewModel.FRAGMENT_STATUS.OPENGL_CAMERA);
        });
        mBtnFfGlFbo.setOnClickListener(view -> {
            mFfViewModel.getSwitchFragment().postValue(FFViewModel.FRAGMENT_STATUS.OPENGL_CAMERA_FBO);
        });
        mBtnFfH265.setOnClickListener(view -> {
            mFfViewModel.getSwitchFragment().postValue(FFViewModel.FRAGMENT_STATUS.HEVC_H265);
        });
        mBtnFfH264.setOnClickListener(view -> {
            mFfViewModel.getSwitchFragment().postValue(FFViewModel.FRAGMENT_STATUS.H264);
        });
    }
}
