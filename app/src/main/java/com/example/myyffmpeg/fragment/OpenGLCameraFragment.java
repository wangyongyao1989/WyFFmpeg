package com.example.myyffmpeg.fragment;

import android.content.Intent;
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
import com.example.myyffmpeg.databinding.FragmentOpenglCameraLayoutBinding;
import com.wangyongyao.glplay.OpenGLPlayCallJni;
import com.wangyongyao.glplay.test.GLActivity;
import com.wangyongyao.glplay.view.CameraPreView;
import com.wangyongyao.glplay.view.GLCameraView;
import com.wangyongyao.glplay.view.GLFlashLightView;
import com.wangyongyao.glplay.view.GLTextureCPlusVideoPlayerView;

public class OpenGLCameraFragment extends BaseFragment {

    private static final String TAG = OpenGLCameraFragment.class.getSimpleName();
    private FragmentOpenglCameraLayoutBinding mBinding;
    private Button mBtnGlBack;
    private FFViewModel mFfViewModel;
    private FrameLayout mGlShow;
    private OpenGLPlayCallJni mFFPlayCallJni;
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
        mFFPlayCallJni = new OpenGLPlayCallJni();
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
            mGLTextureVideoPlayerView = new GLTextureCPlusVideoPlayerView(getActivity()
                    , mFFPlayCallJni);
            mGlShow.addView(mGLTextureVideoPlayerView);
//            Intent intent = new Intent(getActivity(), GLActivity.class);
//            startActivity(intent);
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
