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
    private Button mBtnGlBack;
    private FFViewModel mFfViewModel;
    private FrameLayout mGlShow;
    private OpenGLPlayCallJni mFFPlayCallJni;
    private GLCameraPreView mCameraPreView;
    private Button mBtnCameraPre;
    private Button mBtnGlCamera1;
    private Button mBtnGlCamera2;
    private GLTextureCPlusVideoPlayerView mGLTextureVideoPlayerView;
    private GLTextureFilterPlayerView mGLTextureFilterPlayerView;

    private Button mBtnGlFilter;
    private int type;
    private Button mBtnGlFilterC;
    private Button mBtnSurface;
    private WyyGLSurfaceView mWyyGLSurfaceView;
    private Button mBtnSurfaceNew;
    private WyyGLSurfaceViewNew mWyyGLSurfaceViewNew;
    private Button mBtnSurfaceNewRecord;
    private boolean isRecording;
    private Button mBtnDrawTextSurface;
    private Button mBtnDrawTextRecord;
    private GLDrawTextSurfaceView mDrawTextSurfaceView;


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
        mBtnGlFilter = mBinding.btnGlFilter;
        mBtnGlFilterC = mBinding.btnGlFilterC;
        mBtnSurface = mBinding.btnSurface;
        mBtnSurfaceNew = mBinding.btnSurfaceNew;
        mBtnSurfaceNewRecord = mBinding.btnSurfaceNewRecord;
        mBtnDrawTextSurface = mBinding.btnDrawTextSurface;
        mBtnDrawTextRecord = mBinding.btnDrawTextRecord;
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
            onDestroyGLView();
            mGlShow.removeAllViews();
            if (mCameraPreView == null) {
                mCameraPreView = new GLCameraPreView(getContext());
            }
            mGlShow.addView(mCameraPreView);
        });

        mBtnGlCamera1.setOnClickListener(view -> {
            onDestroyGLView();
            mGlShow.removeAllViews();
            GLFlashLightView glFlashLight
                    = new GLFlashLightView(getActivity(), mFFPlayCallJni);
            mGlShow.addView(glFlashLight);
        });

        mBtnGlCamera2.setOnClickListener(view -> {
            onDestroyGLView();
            mGlShow.removeAllViews();
            mGLTextureVideoPlayerView = new GLTextureCPlusVideoPlayerView(getActivity()
                    , mFFPlayCallJni);
            mGlShow.addView(mGLTextureVideoPlayerView);
        });

        mBtnGlFilter.setOnClickListener(view -> {
            onDestroyGLView();
            mGlShow.removeAllViews();
            if (mGLTextureFilterPlayerView == null) {
                mGLTextureFilterPlayerView = new GLTextureFilterPlayerView(getActivity()
                        , mFFPlayCallJni);
            }
            mGlShow.addView(mGLTextureFilterPlayerView);
        });

        mBtnGlFilterC.setOnClickListener(view -> {
            type++;
            if (mGLTextureFilterPlayerView != null)
                mGLTextureFilterPlayerView.setFilterType(type);
            switchFilter();
        });

        mBtnSurface.setOnClickListener(view -> {
            onDestroyGLView();
            mGlShow.removeAllViews();
            if (mWyyGLSurfaceView == null) {
                mWyyGLSurfaceView = new WyyGLSurfaceView(getActivity(), mFFPlayCallJni);
            }
            mGlShow.addView(mWyyGLSurfaceView);
        });

        mBtnSurfaceNew.setOnClickListener(view -> {
            onDestroyGLView();
            mGlShow.removeAllViews();
            if (mWyyGLSurfaceViewNew == null) {
                mWyyGLSurfaceViewNew = new WyyGLSurfaceViewNew(getActivity(), mFFPlayCallJni);
            }
            mGlShow.addView(mWyyGLSurfaceViewNew);
        });

        mBtnSurfaceNewRecord.setOnClickListener(v -> {
            if (mWyyGLSurfaceViewNew != null) {
                if (isRecording) {
                    mWyyGLSurfaceViewNew.stopRecord();
                    isRecording = false;
                    mBtnSurfaceNewRecord.setText("start recording");
                } else {
                    @SuppressLint("SimpleDateFormat") SimpleDateFormat formatter
                            = new SimpleDateFormat("yy_MM_dd_HH_mm_ss");
                    Date curDate = new Date(System.currentTimeMillis());//获取当前时间
                    String str = formatter.format(curDate);
                    String videoDir = DirectoryPath.createVideoDir(getActivity());
                    String videoName = videoDir + File.pathSeparator + str + ".mp4";
                    mWyyGLSurfaceViewNew.startRecord(videoName);
                    isRecording = true;
                    mBtnSurfaceNewRecord.setText("stop recording");
                }
            } else {
                Toast.makeText(getActivity(), "自定GLSurfaceViewNew没开启"
                        , Toast.LENGTH_SHORT).show();
            }
        });

        mBtnDrawTextSurface.setOnClickListener(view -> {
            onDestroyGLView();
            mGlShow.removeAllViews();
            if (mDrawTextSurfaceView == null) {
                mDrawTextSurfaceView = new GLDrawTextSurfaceView(getActivity(), mFFPlayCallJni);
            }
            mGlShow.addView(mDrawTextSurfaceView);
        });

        mBtnDrawTextRecord.setOnClickListener(v -> {
            if (mDrawTextSurfaceView != null) {
                if (isRecording) {
                    mDrawTextSurfaceView.stopRecord();
                    isRecording = false;
                    mBtnDrawTextRecord.setText("GL绘制文本开始录制");
                } else {
                    @SuppressLint("SimpleDateFormat") SimpleDateFormat formatter
                            = new SimpleDateFormat("yy_MM_dd_HH_mm_ss");
                    Date curDate = new Date(System.currentTimeMillis());//获取当前时间
                    String str = formatter.format(curDate);
                    String videoDir = DirectoryPath.createVideoDir(getActivity());
                    String videoName = videoDir + File.pathSeparator + str + ".mp4";
                    mDrawTextSurfaceView.startRecord(videoName);
                    isRecording = true;
                    mBtnDrawTextRecord.setText("GL绘制文本停止录制");
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
                mBtnGlFilterC.setText("GL滤镜切换");
            }
            break;
            case 1: {
                mBtnGlFilterC.setText("模糊滤镜");
            }
            break;
            case 2: {
                mBtnGlFilterC.setText("鱼眼滤镜");
            }
            break;
            case 3: {
                mBtnGlFilterC.setText("旋流过滤器");
            }
            break;
            case 4: {
                mBtnGlFilterC.setText("放大镜滤光片");
            }
            break;
            case 5: {
                mBtnGlFilterC.setText("利希滕斯坦式过滤器");
            }
            break;
            case 6: {
                mBtnGlFilterC.setText("三角形马赛克滤镜");
            }
            break;
            case 7: {
                mBtnGlFilterC.setText("像素过滤器");
            }
            break;
            case 8: {
                mBtnGlFilterC.setText("交叉缝合过滤器");
            }
            break;
            case 9: {
                mBtnGlFilterC.setText("Toonify过滤器");
            }
            break;
            case 10: {
                mBtnGlFilterC.setText("捕食者热视觉滤镜");
            }
            break;
            case 11: {
                mBtnGlFilterC.setText("压花过滤器");
            }
            break;
            case 12: {
                mBtnGlFilterC.setText("边缘检测滤波器");
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
