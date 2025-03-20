package com.example.myyffmpeg.fragment;

import android.annotation.SuppressLint;
import android.os.Bundle;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.lifecycle.ViewModelProviders;

import com.example.myyffmpeg.FFViewModel;
import com.example.myyffmpeg.databinding.FragmentH264LayoutBinding;
import com.example.myyffmpeg.utils.FileUtils;
import com.wangyongyao.h264.H264CallJni;
import com.wangyongyao.h265.H265CallJni;

/**
 * author : wangyongyao https://github.com/wangyongyao1989
 * Create Time : 2025/3/20 16:04
 * Descibe : MyyFFmpeg com.example.myyffmpeg.fragment
 */
public class H264Fragment extends BaseFragment {

    private static final String TAG = H264Fragment.class.getSimpleName();

    private Button mH264Back;
    private FFViewModel mFfViewModel;
    private H264CallJni mH264CallJni;
    private Button mBtn1;
    private FragmentH264LayoutBinding mBinding;


    @Override
    public View getLayoutDataBing(@NonNull LayoutInflater inflater
            , @Nullable ViewGroup container
            , @Nullable Bundle savedInstanceState) {
        mBinding = FragmentH264LayoutBinding.inflate(inflater);
        return mBinding.getRoot();
    }

    @Override
    public void initView() {
        mH264Back = mBinding.btnHevcH264Back;
        mBtn1 = mBinding.btnH2641;
    }

    @Override
    public void initData() {
        mH264CallJni = new H264CallJni();
    }

    @Override
    public void initObserver() {
        mFfViewModel = ViewModelProviders.of(requireActivity())
                .get(FFViewModel.class);
    }

    @Override
    public void initListener() {
        mH264Back.setOnClickListener(view -> {
            mFfViewModel.getSwitchFragment().postValue(FFViewModel.FRAGMENT_STATUS.MAIN);
        });

        mBtn1.setOnClickListener(view -> {
            String h264FilePath = FileUtils.getModelFilePath(getActivity(), "out.h264");
            mH264CallJni.testH264(h264FilePath);
        });

    }

}