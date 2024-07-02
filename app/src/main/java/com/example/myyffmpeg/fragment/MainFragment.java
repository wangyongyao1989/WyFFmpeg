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

    @Override
    public View getLayoutDataBing(@NonNull LayoutInflater inflater, @Nullable ViewGroup container, @Nullable Bundle savedInstanceState) {
        mBinding = FragmentMainLayoutBinding.inflate(inflater);
        return mBinding.getRoot();
    }

    @Override
    public void initView() {
        mBtnFfPlay = mBinding.btnFfPlay;
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
    }
}
