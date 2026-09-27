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
    // 两个录制按钮各自一个状态位。以前共用一个 isRecording：
    // 正在录 SelfGL 时去点"录制 水印"，会因为 isRecording 已为 true 而走 stopRecord() 分支，
    // 对一个从没开始录制的视图调停止，同时 SelfGL 还在录、它的按钮文案却再也对不上状态。
    private boolean isRecordingSelfGL;
    private boolean isRecordingWaterMark;
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
                if (isRecordingSelfGL) {
                    mWyyGLSurfaceViewNew.stopRecord();
                    isRecordingSelfGL = false;
                    mBinding.btnSurfaceNewRecord.setText("录制 SelfGL");
                } else {
                    @SuppressLint("SimpleDateFormat") SimpleDateFormat formatter
                            = new SimpleDateFormat("yy_MM_dd_HH_mm_ss");
                    Date curDate = new Date(System.currentTimeMillis());//获取当前时间
                    String str = formatter.format(curDate);
                    String videoDir = DirectoryPath.createVideoDir(getActivity());
                    String videoName = new File(videoDir, str + ".mp4").getAbsolutePath();
                    mWyyGLSurfaceViewNew.startRecord(videoName);
                    isRecordingSelfGL = true;
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
                if (isRecordingWaterMark) {
                    mDrawTextSurfaceView.stopRecord();
                    isRecordingWaterMark = false;
                    mBinding.btnDrawTextRecord.setText("录制 水印");
                } else {
                    @SuppressLint("SimpleDateFormat") SimpleDateFormat formatter
                            = new SimpleDateFormat("yy_MM_dd_HH_mm_ss");
                    Date curDate = new Date(System.currentTimeMillis());//获取当前时间
                    String str = formatter.format(curDate);
                    String videoDir = DirectoryPath.createVideoDir(getActivity());
                    String videoName = new File(videoDir, str + ".mp4").getAbsolutePath();
                    mDrawTextSurfaceView.startRecord(videoName);
                    isRecordingWaterMark = true;
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
        // 摄像头预览也要一并释放：原先切到其它模式时 mCameraPreView 没被销毁，
        // CameraDevice 会一直被占用（真机 dumpsys media.camera 可见 CONNECT 无 DISCONNECT）
        if (mCameraPreView != null) {
            mCameraPreView.onDestroy();
            mCameraPreView = null;
        }
        if (mDrawTextSurfaceView != null) {
            // 先停录像再销毁渲染器：录制中直接切模式（例如"录制 水印"进行中点 CUSTOM GL）
            // 会让 MediaCodec 编码器对着一个已经销毁的 GL 上下文继续取帧
            if (isRecordingWaterMark) {
                mDrawTextSurfaceView.stopRecord();
                isRecordingWaterMark = false;
                if (mBinding != null) {
                    mBinding.btnDrawTextRecord.setText("录制 水印");
                }
            }
            mDrawTextSurfaceView.destroyRender();
            mDrawTextSurfaceView = null;
        }
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
            if (isRecordingSelfGL) {
                mWyyGLSurfaceViewNew.stopRecord();
                isRecordingSelfGL = false;
                if (mBinding != null) {
                    mBinding.btnSurfaceNewRecord.setText("录制 SelfGL");
                }
            }
            mWyyGLSurfaceViewNew.destroyRender();
            mWyyGLSurfaceViewNew = null;
        }
    }

    /**
     * MainActivity 用 hide()/show() 切换 Fragment，离开页面时 onDestroy/onDestroyView
     * 都不会触发，必须在 onHiddenChanged 里释放摄像头与 GL 渲染器。
     */
    @Override
    public void onHiddenChanged(boolean hidden) {
        super.onHiddenChanged(hidden);
        if (hidden && mBinding != null) {
            // onDestroyGLView() 里已经把"录制中先停录"的逻辑做了，这里直接复用它，
            // 不再单独判 isRecording（原来两个录制按钮共用一个标志位，状态经常对不上）
            onDestroyGLView();
            mBinding.glShow.removeAllViews();
        }
    }

    @Override
    public void onDestroy() {
        onDestroyGLView();
        super.onDestroy();
    }


}
