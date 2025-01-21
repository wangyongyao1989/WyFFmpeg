package com.example.myyffmpeg.fragment;

import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.FrameLayout;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.lifecycle.ViewModelProviders;

import com.example.myyffmpeg.FFViewModel;
import com.example.myyffmpeg.databinding.FragmentGlCameraFboLayoutBinding;
import com.wangyongyao.glplay.OpenGLPlayCallJni;
import com.wangyongyao.glplay.view.GLFboDrawTextSurfaceView;

/**
 * author : wangyongyao https://github.com/wangyongyao1989
 * Create Time : 2025/1/21
 * Descibe : MyyFFmpeg com.example.myyffmpeg.fragment
 */
public class OpenGLCameraFboFragment extends BaseFragment {

    private FragmentGlCameraFboLayoutBinding mBinding;
    private Button mBtnGlFboBack;
    private FFViewModel mFfViewModel;
    private Button mBtnFboPre;
    private FrameLayout mGlFboShow;
    private GLFboDrawTextSurfaceView mFboDrawTextSurfaceView;
    private OpenGLPlayCallJni mFFPlayCallJni;


    @Override
    public View getLayoutDataBing(@NonNull LayoutInflater inflater,
                                  @Nullable ViewGroup container,
                                  @Nullable Bundle savedInstanceState) {
        mBinding = FragmentGlCameraFboLayoutBinding.inflate(inflater);
        return mBinding.getRoot();
    }

    @Override
    public void initView() {
        mBtnGlFboBack = mBinding.btnGlFboBack;
        mBtnFboPre = mBinding.btnCameraFboPre;

        mGlFboShow = mBinding.glFboShow;
    }

    @Override
    public void initData() {
        mFFPlayCallJni = new OpenGLPlayCallJni();

    }

    @Override
    public void initObserver() {
        mFfViewModel = ViewModelProviders.of(requireActivity())
                .get(FFViewModel.class);

    }

    @Override
    public void initListener() {
        mBtnGlFboBack.setOnClickListener(view -> {
            mFfViewModel.getSwitchFragment().postValue(FFViewModel.FRAGMENT_STATUS.MAIN);
        });

        mBtnFboPre.setOnClickListener(view -> {
            onDestroyGLView();
            mGlFboShow.removeAllViews();
            if (mFboDrawTextSurfaceView == null) {
                mFboDrawTextSurfaceView = new GLFboDrawTextSurfaceView(getActivity(), mFFPlayCallJni);
            }
            mGlFboShow.addView(mFboDrawTextSurfaceView);
        });

    }

    private void onDestroyGLView() {

    }
}
