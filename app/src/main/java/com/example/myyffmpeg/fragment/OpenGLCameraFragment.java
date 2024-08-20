package com.example.myyffmpeg.fragment;

import android.graphics.Point;
import android.graphics.SurfaceTexture;
import android.os.Bundle;
import android.util.Log;
import android.util.Size;
import android.view.LayoutInflater;
import android.view.TextureView;
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
import com.example.myyffmpeg.view.GLFlashLightView;
import com.example.rtmplive.camera.Camera2Helper;
import com.example.rtmplive.camera.Camera2Listener;

public class OpenGLCameraFragment extends BaseFragment implements
        TextureView.SurfaceTextureListener, Camera2Listener {

    private static final String TAG = OpenGLCameraFragment.class.getSimpleName();
    private FragmentOpenglCameraLayoutBinding mBinding;
    private Button mBtnGlBack;
    private Button mBtnGlCamera1;
    private FFViewModel mFfViewModel;
    private FrameLayout mGlShow;
    private FFPlayCallJni mFFPlayCallJni;
    private TextureView mTextureView;
    private int width = 640;
    private int height = 480;
    private Camera2Helper camera2Helper;
    private int rotation = 90;

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
        mBtnGlCamera1 = mBinding.btnGlCamera1;
        mGlShow = mBinding.glShow;
        mTextureView = mBinding.textureView;

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
        mTextureView.setSurfaceTextureListener(this);

        mBtnGlBack.setOnClickListener(view -> {
            mFfViewModel.getSwitchFragment().postValue(FFViewModel.FRAGMENT_STATUS.MAIN);
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
        if (camera2Helper != null) {
            camera2Helper.stop();
            camera2Helper.release();
            camera2Helper = null;
        }
        super.onDestroy();
    }

    @Override
    public void onSurfaceTextureAvailable(@NonNull SurfaceTexture surface, int width, int height) {
        Log.e(TAG, "onSurfaceTextureAvailable surface:" + surface);
        startCameraPreview();
    }

    @Override
    public void onSurfaceTextureSizeChanged(@NonNull SurfaceTexture surface, int width, int height) {

    }

    @Override
    public boolean onSurfaceTextureDestroyed(@NonNull SurfaceTexture surface) {
        stopCameraPreview();
        return false;
    }

    @Override
    public void onSurfaceTextureUpdated(@NonNull SurfaceTexture surface) {

    }

    private void startCameraPreview() {
        rotation = (getActivity()).getWindowManager().getDefaultDisplay().getRotation();
        camera2Helper = new Camera2Helper.Builder()
                .cameraListener(this)
                .specificCameraId(Camera2Helper.CAMERA_ID_BACK)
                .context(getActivity())
                .previewOn(mTextureView)
                .previewViewSize(new Point(width, height))
                .rotation(rotation)
                .build();
        camera2Helper.start();
    }

    private void stopCameraPreview() {
        if (camera2Helper != null) {
            camera2Helper.stop();
        }
    }

    @Override
    public void onCameraOpened(Size previewSize, int displayOrientation) {
        Log.e(TAG, "onCameraOpened previewSize:" + previewSize.toString()
                + "==displayOrientation:" + displayOrientation);

    }

    @Override
    public void onPreviewFrame(byte[] yuvData) {
//        Log.e(TAG, "onPreviewFrame:" + yuvData.length);

    }

    @Override
    public void onCameraClosed() {
        Log.e(TAG, "onCameraClosed:");

    }

    @Override
    public void onCameraError(Exception e) {
        Log.e(TAG, "onCameraError:" + e.toString());

    }


}
