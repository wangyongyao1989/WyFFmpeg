package com.example.myyffmpeg.fragment;

import android.annotation.SuppressLint;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Toast;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.lifecycle.ViewModelProviders;

import com.example.myyffmpeg.FFViewModel;
import com.example.myyffmpeg.databinding.FragmentOpenglCameraLayoutBinding;
import com.wangyongyao.common.utils.DirectoryPath;
import com.wangyongyao.glplay.OpenGLPlayCallJni;
import com.wangyongyao.glplay.view.GLCameraPreView;
import com.wangyongyao.glplay.view.GLDrawTextSurfaceView;
import com.wangyongyao.glplay.view.GLFlashLightView;
import com.wangyongyao.glplay.view.WyyGLSurfaceView;
import com.wangyongyao.glplay.view.GLTextureCPlusVideoPlayerView;
import com.wangyongyao.glplay.view.GLTextureFilterPlayerView;
import com.wangyongyao.glplay.view.WyyGLSurfaceViewNew;

import java.io.File;
import java.text.SimpleDateFormat;
import java.util.Date;

public class OpenGLCameraFragment extends BaseFragment {

    private static final String TAG = OpenGLCameraFragment.class.getSimpleName();
    private FragmentOpenglCameraLayoutBinding mBinding;
    private FFViewModel mFfViewModel;
    private OpenGLPlayCallJni mFFPlayCallJni;
    private GLCameraPreView mCameraPreView;
    private GLTextureCPlusVideoPlayerView mGLTextureVideoPlayerView;
    private GLTextureFilterPlayerView mGLTextureFilterPlayerView;

    private int type;
    private WyyGLSurfaceView mWyyGLSurfaceView;
    private WyyGLSurfaceViewNew mWyyGLSurfaceViewNew;
    private boolean isRecording;
    private GLDrawTextSurfaceView mDrawTextSurfaceView;


    @Override
    public View getLayoutDataBing(@NonNull LayoutInflater inflater
            , @Nullable ViewGroup container
            , @Nullable Bundle savedInstanceState) {
        mBinding = FragmentOpenglCameraLayoutBinding.inflate(inflater, container, false);
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

        mBinding.btnGlBack.setOnClickListener(view -> {
            mFfViewModel.getSwitchFragment().postValue(FFViewModel.FRAGMENT_STATUS.MAIN);
        });

        mBinding.btnCameraPre.setOnClickListener(view -> {
            onDestroyGLView();
            mBinding.glShow.removeAllViews();
            if (mCameraPreView == null) {
                mCameraPreView = new GLCameraPreView(getContext());
            }
            mBinding.glShow.addView(mCameraPreView);
        });

        mBinding.btnGlCamera1.setOnClickListener(view -> {
            onDestroyGLView();
            mBinding.glShow.removeAllViews();
            GLFlashLightView glFlashLight
                    = new GLFlashLightView(getActivity(), mFFPlayCallJni);
            mBinding.glShow.addView(glFlashLight);
        });

        mBinding.btnGlCamera2.setOnClickListener(view -> {
            onDestroyGLView();
            mBinding.glShow.removeAllViews();
            mGLTextureVideoPlayerView = new GLTextureCPlusVideoPlayerView(getActivity()
                    , mFFPlayCallJni);
            mBinding.glShow.addView(mGLTextureVideoPlayerView);
        });

        mBinding.btnGlFilter.setOnClickListener(view -> {
            onDestroyGLView();
            mBinding.glShow.removeAllViews();
            if (mGLTextureFilterPlayerView == null) {
                mGLTextureFilterPlayerView = new GLTextureFilterPlayerView(getActivity()
                        , mFFPlayCallJni);
            }
            mBinding.glShow.addView(mGLTextureFilterPlayerView);
        });

        mBinding.btnGlFilterC.setOnClickListener(view -> {
            type++;
            if (mGLTextureFilterPlayerView != null)
                mGLTextureFilterPlayerView.setFilterType(type);
            switchFilter();
        });

        mBinding.btnSurface.setOnClickListener(view -> {
            onDestroyGLView();
            mBinding.glShow.removeAllViews();
            if (mWyyGLSurfaceView == null) {
                mWyyGLSurfaceView = new WyyGLSurfaceView(getActivity(), mFFPlayCallJni);
            }
            mBinding.glShow.addView(mWyyGLSurfaceView);
        });

        mBinding.btnSurfaceNew.setOnClickListener(view -> {
            onDestroyGLView();
            mBinding.glShow.removeAllViews();
            if (mWyyGLSurfaceViewNew == null) {
                mWyyGLSurfaceViewNew = new WyyGLSurfaceViewNew(getActivity(), mFFPlayCallJni);
            }
            mBinding.glShow.addView(mWyyGLSurfaceViewNew);
        });

        mBinding.btnSurfaceNewRecord.setOnClickListener(v -> {
            if (mWyyGLSurfaceViewNew != null) {
                if (isRecording) {
                    mWyyGLSurfaceViewNew.stopRecord();
                    isRecording = false;
                    mBinding.btnSurfaceNewRecord.setText("录制 SelfGL");
                } else {
                    @SuppressLint("SimpleDateFormat") SimpleDateFormat formatter
                            = new SimpleDateFormat("yy_MM_dd_HH_mm_ss");
                    Date curDate = new Date(System.currentTimeMillis());//获取当前时间
                    String str = formatter.format(curDate);
                    String videoDir = DirectoryPath.createVideoDir(getActivity());
                    String videoName = videoDir + File.pathSeparator + str + ".mp4";
                    mWyyGLSurfaceViewNew.startRecord(videoName);
                    isRecording = true;
                    mBinding.btnSurfaceNewRecord.setText("停止 SelfGL");
                }
            } else {
                Toast.makeText(getActivity(), "自定GLSurfaceViewNew没开启"
                        , Toast.LENGTH_SHORT).show();
            }
        });

        mBinding.btnDrawTextSurface.setOnClickListener(view -> {
            onDestroyGLView();
            mBinding.glShow.removeAllViews();
            if (mDrawTextSurfaceView == null) {
                mDrawTextSurfaceView = new GLDrawTextSurfaceView(getActivity(), mFFPlayCallJni);
            }
            mBinding.glShow.addView(mDrawTextSurfaceView);
        });

        mBinding.btnDrawTextRecord.setOnClickListener(v -> {
            if (mDrawTextSurfaceView != null) {
                if (isRecording) {
                    mDrawTextSurfaceView.stopRecord();
                    isRecording = false;
                    mBinding.btnDrawTextRecord.setText("录制 水印");
                } else {
                    @SuppressLint("SimpleDateFormat") SimpleDateFormat formatter
                            = new SimpleDateFormat("yy_MM_dd_HH_mm_ss");
                    Date curDate = new Date(System.currentTimeMillis());//获取当前时间
                    String str = formatter.format(curDate);
                    String videoDir = DirectoryPath.createVideoDir(getActivity());
                    String videoName = videoDir + File.pathSeparator + str + ".mp4";
                    mDrawTextSurfaceView.startRecord(videoName);
                    isRecording = true;
                    mBinding.btnDrawTextRecord.setText("停止 水印");
                }
            } else {
                Toast.makeText(getActivity(), "GL视频中绘制文本没开启"
                        , Toast.LENGTH_SHORT).show();
            }
        });

    }

    @SuppressLint("SetTextI18n")
    private void switchFilter() {
        int type = 0;
        if (mGLTextureFilterPlayerView != null) {
            type = mGLTextureFilterPlayerView.getFilterType();
        }
        switch (type) {
            case 0: {
                mBinding.btnGlFilterC.setText("GL滤镜切换");
            }
            break;
            case 1: {
                mBinding.btnGlFilterC.setText("模糊滤镜");
            }
            break;
            case 2: {
                mBinding.btnGlFilterC.setText("鱼眼滤镜");
            }
            break;
            case 3: {
                mBinding.btnGlFilterC.setText("旋流过滤器");
            }
            break;
            case 4: {
                mBinding.btnGlFilterC.setText("放大镜滤光片");
            }
            break;
            case 5: {
                mBinding.btnGlFilterC.setText("利希滕斯坦式过滤器");
            }
            break;
            case 6: {
                mBinding.btnGlFilterC.setText("三角形马赛克滤镜");
            }
            break;
            case 7: {
                mBinding.btnGlFilterC.setText("像素过滤器");
            }
            break;
            case 8: {
                mBinding.btnGlFilterC.setText("交叉缝合过滤器");
            }
            break;
            case 9: {
                mBinding.btnGlFilterC.setText("Toonify过滤器");
            }
            break;
            case 10: {
                mBinding.btnGlFilterC.setText("捕食者热视觉滤镜");
            }
            break;
            case 11: {
                mBinding.btnGlFilterC.setText("压花过滤器");
            }
            break;
            case 12: {
                mBinding.btnGlFilterC.setText("边缘检测滤波器");
            }
            break;

        }

    }

    private void onDestroyGLView() {
        if (mGLTextureVideoPlayerView != null) {
            mGLTextureVideoPlayerView.destroyRender();
            mGLTextureVideoPlayerView = null;
        }
        if (mGLTextureFilterPlayerView != null) {
            mGLTextureFilterPlayerView.destroyRender();
            mGLTextureFilterPlayerView = null;
        }
        if (mWyyGLSurfaceView != null) {
            mWyyGLSurfaceView.destroyRender();
            mWyyGLSurfaceView = null;
        }
        if (mWyyGLSurfaceViewNew != null) {
            mWyyGLSurfaceViewNew.destroyRender();
            mWyyGLSurfaceViewNew = null;
        }
    }


    @Override
    public void onDestroy() {
        if (mCameraPreView != null) {
            mCameraPreView.onDestroy();
        }
        if (mGLTextureVideoPlayerView != null) {
            mGLTextureVideoPlayerView.destroyRender();
        }
        if (mGLTextureFilterPlayerView != null) {
            mGLTextureFilterPlayerView.destroyRender();
            mGLTextureFilterPlayerView = null;
        }
        super.onDestroy();
    }


}
