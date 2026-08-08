package com.example.myyffmpeg.fragment;

import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.TextureView;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.lifecycle.ViewModelProviders;

import com.example.myyffmpeg.FFViewModel;
import com.example.myyffmpeg.databinding.FragmentRtmpLayoutBinding;
import com.example.rtmplive.LiveManger;

public class RtmpFragment extends BaseFragment {

    private FragmentRtmpLayoutBinding mBinding;
    private FFViewModel mFfViewModel;
    private LiveManger mLiveManger;
    private boolean isPushing = false;
    private String rtmpPushPath;

    @Override
    public View getLayoutDataBing(@NonNull LayoutInflater inflater
            , @Nullable ViewGroup container
            , @Nullable Bundle savedInstanceState) {
        mBinding = FragmentRtmpLayoutBinding.inflate(inflater, container, false);
        return mBinding.getRoot();
    }

    @Override
    public void initView() {
    }

    @Override
    public void initData() {
        mLiveManger = new LiveManger(this.getViewLifecycleOwner(),getContext(), mBinding.textureView);
        rtmpPushPath = "rtmp://live-push.bilivideo.com/live-bvc/?streamname=live_345162489_81809986&key=6ba7ec38481c5dd2b3f8e4fb2b5fb8e0&schedule=rtmp&pflag=1";
    }

    @Override
    public void initObserver() {
        mFfViewModel = ViewModelProviders.of(requireActivity())
                .get(FFViewModel.class);
    }

    @Override
    public void initListener() {
        mBinding.btnRtmpBack.setOnClickListener(view -> {
            mLiveManger.releaseRtmp();
            mFfViewModel.getSwitchFragment().postValue(FFViewModel.FRAGMENT_STATUS.MAIN);
        });

        mBinding.btnFfPush.setOnClickListener(view -> {
            mLiveManger.startRtmpPush(rtmpPushPath);
            isPushing = true;
        });

        mBinding.btnFfStop.setOnClickListener(view -> {
            mLiveManger.stopRtmpPush();
            isPushing = false;
        });

        mBinding.btnFfPause.setOnClickListener(view -> {
            mLiveManger.pauseRtmp();
        });
    }


    @Override
    public void onDestroy() {
        if (mLiveManger != null) {
            if (isPushing) {
                mLiveManger.stopRtmpPush();
            }
            mLiveManger.releaseRtmp();
            mLiveManger.release();
        }
        super.onDestroy();
    }
}
