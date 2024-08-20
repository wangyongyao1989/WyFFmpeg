package com.example.myyffmpeg.fragment;

import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.TextureView;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.FrameLayout;
import android.widget.RelativeLayout;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.lifecycle.ViewModelProviders;

import com.example.myyffmpeg.FFPlayCallJni;
import com.example.myyffmpeg.FFViewModel;
import com.example.myyffmpeg.databinding.FragmentOpenglCameraLayoutBinding;
import com.example.myyffmpeg.databinding.FragmentRtmpLayoutBinding;
import com.example.myyffmpeg.view.GLFlashLightView;
import com.example.rtmplive.LiveManger;
import com.example.rtmplive.RtmpLivePusher;

public class OpenGLCameraFragment extends BaseFragment {

    private static final String TAG = OpenGLCameraFragment.class.getSimpleName();
    private FragmentOpenglCameraLayoutBinding mBinding;
    private RelativeLayout mRlClose;
    private Button mBtnGlCamera1;
    private FFViewModel mFfViewModel;
    private FrameLayout mGlShow;
    private FFPlayCallJni mFFPlayCallJni;

    @Override
    public View getLayoutDataBing(@NonNull LayoutInflater inflater
            , @Nullable ViewGroup container
            , @Nullable Bundle savedInstanceState) {
        mBinding = FragmentOpenglCameraLayoutBinding.inflate(inflater);
        return mBinding.getRoot();
    }

    @Override
    public void initView() {
        mRlClose = mBinding.rlClose;
        mBtnGlCamera1 = mBinding.btnGlCamera1;
        mGlShow = mBinding.glShow;

    }

    @Override
    public void initData() {
        mFFPlayCallJni = new FFPlayCallJni();
    }

    @Override
    public void initObserver() {
        mFfViewModel = ViewModelProviders.of(requireActivity())
                .get(FFViewModel.class);
    }

    @Override
    public void initListener() {
        mRlClose.setOnClickListener(view -> {

            mFfViewModel.getSwitchFragment().postValue(FFViewModel.FRAGMENT_STATUS.OPENGL_CAMERA);
        });
        mBtnGlCamera1.setOnClickListener(view -> {
            mGlShow.removeAllViews();
            GLFlashLightView glFlashLight
                    = new GLFlashLightView(getActivity(), mFFPlayCallJni);
            mGlShow.addView(glFlashLight);
        });

    }


    @Override
    public void onDestroy() {

        super.onDestroy();
    }
}
