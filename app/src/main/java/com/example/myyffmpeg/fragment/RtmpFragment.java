package com.example.myyffmpeg.fragment;

import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.TextureView;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.RelativeLayout;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.lifecycle.ViewModelProviders;

import com.example.myyffmpeg.FFViewModel;
import com.example.myyffmpeg.databinding.FragmentRtmpLayoutBinding;
import com.example.rtmplive.LiveManger;
import com.example.rtmplive.RtmpLivePusher;

public class RtmpFragment extends BaseFragment {

    private com.example.myyffmpeg.databinding.FragmentRtmpLayoutBinding mBinding;
    private Button mBtnFfPush;
    private RtmpLivePusher mRtmpLivePusher;
    private RelativeLayout mRlClose;
    private FFViewModel mFfViewModel;
    private Button mBtnFfStop;
    private TextureView mTextureView;
    private LiveManger mLiveManger;
    private boolean isPushing = false;
    private String rtmpPushPath;

    @Override
    public View getLayoutDataBing(@NonNull LayoutInflater inflater
            , @Nullable ViewGroup container
            , @Nullable Bundle savedInstanceState) {
        mBinding = FragmentRtmpLayoutBinding.inflate(inflater);
        return mBinding.getRoot();
    }

    @Override
    public void initView() {
        mRlClose = mBinding.rlClose;
        mBtnFfPush = mBinding.btnFfPush;
        mBtnFfStop = mBinding.btnFfStop;
        mTextureView = mBinding.textureView;

    }

    @Override
    public void initData() {
        mLiveManger = new LiveManger(getContext(), mTextureView);
        rtmpPushPath = "rtmp://live-push.bilivideo.com/live-bvc/?streamname=live_345162489_81809986&key=6ba7ec38481c5dd2b3f8e4fb2b5fb8e0&schedule=rtmp&pflag=1";
    }

    @Override
    public void initObserver() {
        mFfViewModel = ViewModelProviders.of(requireActivity())
                .get(FFViewModel.class);
    }

    @Override
    public void initListener() {
        mRlClose.setOnClickListener(view -> {
            mFfViewModel.getSwitchFragment().postValue(FFViewModel.FRAGMENT_STATUS.MAIN);
        });

        mBtnFfPush.setOnClickListener(view -> {
            mLiveManger.startRtmpPush(rtmpPushPath);
        });

        mBtnFfStop.setOnClickListener(view -> {
            mLiveManger.stopRtmpPush();
        });
    }


    @Override
    public void onDestroy() {
        if (mLiveManger != null) {
            if (isPushing) {
                mLiveManger.stopRtmpPush();
            }
            mLiveManger.release();
        }
        super.onDestroy();
    }
}
