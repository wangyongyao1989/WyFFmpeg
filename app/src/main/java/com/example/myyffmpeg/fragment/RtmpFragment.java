package com.example.myyffmpeg.fragment;

import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.RelativeLayout;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.lifecycle.ViewModelProviders;

import com.example.myyffmpeg.FFViewModel;
import com.example.myyffmpeg.databinding.FragmentRtmpLayoutBinding;
import com.example.rtmplive.RtmpLivePusher;

public class RtmpFragment extends BaseFragment {

    private com.example.myyffmpeg.databinding.FragmentRtmpLayoutBinding mBinding;
    private Button mBtnFfPush;
    private RtmpLivePusher mRtmpLivePusher;
    private RelativeLayout mRlClose;
    private FFViewModel mFfViewModel;

    @Override
    public View getLayoutDataBing(@NonNull LayoutInflater inflater, @Nullable ViewGroup container, @Nullable Bundle savedInstanceState) {
        mBinding = FragmentRtmpLayoutBinding.inflate(inflater);
        return mBinding.getRoot();
    }

    @Override
    public void initView() {
        mRlClose = mBinding.rlClose;
        mBtnFfPush = mBinding.btnFfPush;
    }

    @Override
    public void initData() {
        mRtmpLivePusher = new RtmpLivePusher();
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

        });
    }
}
