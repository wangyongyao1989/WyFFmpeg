package com.example.myyffmpeg.fragment;

import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.FrameLayout;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.lifecycle.ViewModelProviders;

import com.example.myyffmpeg.FFViewModel;
import com.example.myyffmpeg.databinding.FragmentGlCameraFboLayoutBinding;
import com.wangyongyao.glplay.OpenGLPlayCallJni;
import com.wangyongyao.glplay.view.GLFBOPostProcessingView;

/**
 * author : wangyongyao https://github.com/wangyongyao1989
 * Create Time : 2025/1/21
 * Descibe : MyyFFmpeg com.example.myyffmpeg.fragment
 */
public class OpenGLCameraFboFragment extends BaseFragment {

    private FragmentGlCameraFboLayoutBinding mBinding;
    private FFViewModel mFfViewModel;
    private OpenGLPlayCallJni mFFPlayCallJni;
    private GLFBOPostProcessingView mGlView;
    private int switchType;


    @Override
    public View getLayoutDataBing(@NonNull LayoutInflater inflater,
                                  @Nullable ViewGroup container,
                                  @Nullable Bundle savedInstanceState) {
        mBinding = FragmentGlCameraFboLayoutBinding.inflate(inflater, container, false);
        return mBinding.getRoot();
    }

    @Override
    public void initView() {
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
        mBinding.btnGlFboBack.setOnClickListener(view -> {
            mFfViewModel.getSwitchFragment().postValue(FFViewModel.FRAGMENT_STATUS.MAIN);
        });


        mBinding.btnCameraFboPre.setOnClickListener(view -> {
            mBinding.glFboShow.removeAllViews();
            mGlView = new GLFBOPostProcessingView(getActivity()
                    , mFFPlayCallJni);
            mBinding.glFboShow.addView(mGlView);
        });

        mBinding.btnCameraFboSwitch.setOnClickListener(view -> {
            if (mGlView == null) {
                return;
            }
            mGlView.setFBOPostProcessingType(switchType);
            int type = mGlView.getFBOPostProcessingType();
            switchBtnSenior7UI(type);
            switchType++;
        });

    }

    private void switchBtnSenior7UI(int type) {
        switch (type) {
            case 0: {
                mBinding.btnCameraFboPre.setText("正常");
            }
            break;
            case 1: {
                mBinding.btnCameraFboPre.setText("反相");
            }
            break;
            case 2: {
                mBinding.btnCameraFboPre.setText("图像灰度化");
            }
            break;
            case 3: {
                mBinding.btnCameraFboPre.setText("灰度化加权");
            }
            break;
            case 4: {
                mBinding.btnCameraFboPre.setText("核效果");
            }
            break;
        }
    }

    private void onDestroyGLView() {

    }
}
