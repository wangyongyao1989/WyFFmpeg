package com.example.myyffmpeg.fragment;

import android.annotation.SuppressLint;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.FrameLayout;
import android.widget.Toast;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.lifecycle.ViewModelProviders;

import com.example.myyffmpeg.FFViewModel;
import com.example.myyffmpeg.databinding.FragmentGlCameraFboLayoutBinding;
import com.example.myyffmpeg.utils.DirectoryPath;
import com.wangyongyao.glplay.OpenGLPlayCallJni;
import com.wangyongyao.glplay.view.GLFBOPostProcessingView;
import com.wangyongyao.glplay.view.GLFboDrawTextSurfaceView;

import java.io.File;
import java.text.SimpleDateFormat;
import java.util.Date;

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
    private Button mBtnFboRecord;
    private boolean isRecording;
    private Button mBtnSenior7;
    private Button mBtnSenior8;
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
        mBtnFboRecord = mBinding.btnCameraFboRecord;
        mBtnSenior7 = mBinding.btnSenior7;
        mBtnSenior8 = mBinding.btnSenior8;
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

        mBtnFboRecord.setOnClickListener(v -> {
            if (mFboDrawTextSurfaceView != null) {
                if (isRecording) {
                    mFboDrawTextSurfaceView.stopRecord();
                    isRecording = false;
                    mBtnFboRecord.setText("开始视频录制");
                } else {
                    @SuppressLint("SimpleDateFormat") SimpleDateFormat formatter
                            = new SimpleDateFormat("yy_MM_dd_HH_mm_ss");
                    Date curDate = new Date(System.currentTimeMillis());//获取当前时间
                    String str = formatter.format(curDate);
                    String videoDir = DirectoryPath.createVideoDir(getActivity());
                    String videoName = videoDir + File.pathSeparator + str + ".mp4";
                    mFboDrawTextSurfaceView.startRecord(videoName);
                    isRecording = true;
                    mBtnFboRecord.setText("停止视频录制");
                }
            } else {
                Toast.makeText(getActivity(), "GL FBO摄像头预览没开启"
                        , Toast.LENGTH_SHORT).show();
            }
        });

        mBtnSenior7.setOnClickListener(view -> {
            mGlFboShow.removeAllViews();
            mGlView = new GLFBOPostProcessingView(getActivity()
                    , mFFPlayCallJni);
            mGlFboShow.addView(mGlView);
        });

        mBtnSenior8.setOnClickListener(view -> {
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
                mBtnSenior7.setText("反相");
            }
            break;
            case 1: {
                mBtnSenior7.setText("图像灰度化");
            }
            break;
            case 2: {
                mBtnSenior7.setText("灰度化加权");
            }
            break;
            case 3: {
                mBtnSenior7.setText("核效果");
            }
            break;
        }
    }

    private void onDestroyGLView() {

    }
}
