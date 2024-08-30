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

import com.example.myyffmpeg.FFPlayCallJni;
import com.example.myyffmpeg.FFViewModel;
import com.example.myyffmpeg.databinding.FragmentOpenglCameraLayoutBinding;
import com.example.myyffmpeg.view.CameraPreView;
import com.example.myyffmpeg.view.GLFlashLightView;
import com.example.myyffmpeg.view.GLTextureCPlusVideoPlayerView;

public class OpenGLCameraFragment extends BaseFragment {

    private static final String TAG = OpenGLCameraFragment.class.getSimpleName();
    private FragmentOpenglCameraLayoutBinding mBinding;
    private Button mBtnGlBack;
    private FFViewModel mFfViewModel;
    private FrameLayout mGlShow;
    private FFPlayCallJni mFFPlayCallJni;
    private CameraPreView mCameraPreView;
    private Button mBtnCameraPre;
    private Button mBtnGlCamera1;
    private Button mBtnGlCamera2;
    private GLTextureCPlusVideoPlayerView mGLTextureVideoPlayerView;

    @Override
    public View getLayoutDataBing(@NonNull LayoutInflater inflater
            , @Nullable ViewGroup container
            , @Nullable Bundle savedInstanceState) {
        mBinding = FragmentOpenglCameraLayoutBinding.inflate(inflater);
        return mBinding.getRoot();
    }

    @Override
    public void initView() {
        mBtnGlBack = mBinding.btnGlBack;
        mBtnCameraPre = mBinding.btnCameraPre;
        mBtnGlCamera1 = mBinding.btnGlCamera1;
        mBtnGlCamera2 = mBinding.btnGlCamera2;
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

        mBtnGlBack.setOnClickListener(view -> {
            mFfViewModel.getSwitchFragment().postValue(FFViewModel.FRAGMENT_STATUS.MAIN);
        });

        mBtnCameraPre.setOnClickListener(view -> {
            mGlShow.removeAllViews();
            if (mCameraPreView == null) {
                mCameraPreView = new CameraPreView(getContext());
            }
            mGlShow.addView(mCameraPreView);
        });

        mBtnGlCamera1.setOnClickListener(view -> {
            mGlShow.removeAllViews();
            GLFlashLightView glFlashLight
                    = new GLFlashLightView(getActivity(), mFFPlayCallJni);
            mGlShow.addView(glFlashLight);
        });

        mBtnGlCamera2.setOnClickListener(view -> {
            mGlShow.removeAllViews();
//            GLCameraView glCameraView
//                    = new GLCameraView(getActivity(), mFFPlayCallJni);
            mGLTextureVideoPlayerView = new GLTextureCPlusVideoPlayerView(getActivity()
                    , mFFPlayCallJni);
            mGlShow.addView(mGLTextureVideoPlayerView);
        });

    }


    @Override
    public void onDestroy() {
        if (mCameraPreView != null) {
            mCameraPreView.onDestroy();
        }
        if (mGLTextureVideoPlayerView != null) {
            mGLTextureVideoPlayerView.destroyRender();
        }

        super.onDestroy();
    }


}
