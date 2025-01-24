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
import com.wangyongyao.glplay.view.GLFBOPostProcessingView;

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
    private OpenGLPlayCallJni mFFPlayCallJni;
    private Button mBtnFboSwitch;
    private GLFBOPostProcessingView mGlView;
    private int switchType;


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
        mBtnFboSwitch = mBinding.btnCameraFboSwitch;
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
            mGlFboShow.removeAllViews();
            mGlView = new GLFBOPostProcessingView(getActivity()
                    , mFFPlayCallJni);
            mGlFboShow.addView(mGlView);
        });

        mBtnFboSwitch.setOnClickListener(view -> {
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
                mBtnFboPre.setText("正常");
            }
            break;
            case 1: {
                mBtnFboPre.setText("反相");
            }
            break;
            case 2: {
                mBtnFboPre.setText("图像灰度化");
            }
            break;
            case 3: {
                mBtnFboPre.setText("灰度化加权");
            }
            break;
            case 4: {
                mBtnFboPre.setText("核效果");
            }
            break;
        }
    }

    private void onDestroyGLView() {

    }
}
