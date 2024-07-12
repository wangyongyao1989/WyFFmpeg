package com.example.rtmplive;

import android.app.Activity;
import android.content.Context;
import android.graphics.Point;
import android.graphics.SurfaceTexture;
import android.util.Log;
import android.util.Size;
import android.view.TextureView;

import androidx.annotation.NonNull;
import androidx.lifecycle.LifecycleOwner;

import com.example.rtmplive.camera.Camera2Helper;
import com.example.rtmplive.camera.Camera2Listener;

public class LiveManger implements TextureView.SurfaceTextureListener, Camera2Listener {

    private static final String TAG = LiveManger.class.getSimpleName();

    private final TextureView mTextureView;
    private Context mContext;
    private int rotation = 0;
    private Camera2Helper camera2Helper;
    private RtmpLivePusher mRtmpLivePusher;
    private LifecycleOwner mLifecycleOwner;
    private int width = 640;
    private int height = 480;
    private int videoBitRate = 800000;// kb/s
    private int videoFrameRate = 10;
    private boolean isLiving;


    public LiveManger(LifecycleOwner lifecycleOwner, Context context
            , TextureView textureView) {
        mContext = context;
        mLifecycleOwner = lifecycleOwner;
        mTextureView = textureView;
        mTextureView.setSurfaceTextureListener(this);
        initPusher();
    }

    private void initPusher() {
        mRtmpLivePusher = new RtmpLivePusher();
    }

    public void startRtmpPush(String path) {
        mRtmpLivePusher.startRtmpPush(path);
        isLiving = true;
    }

    public void stopRtmpPush() {
        mRtmpLivePusher.stopRtmpPush();
        isLiving = false;

    }

    public void releaseRtmp() {
        mRtmpLivePusher.releaseRtmp();
    }

    public void pauseRtmp() {
        mRtmpLivePusher.pauseRtmp();
    }

    private void startCameraPreview() {
        if (mContext instanceof Activity) {
            rotation = ((Activity) mContext).getWindowManager().getDefaultDisplay().getRotation();
        }
        camera2Helper = new Camera2Helper.Builder()
                .cameraListener(this)
                .specificCameraId(Camera2Helper.CAMERA_ID_BACK)
                .context(mContext.getApplicationContext())
                .previewOn(mTextureView)
                .previewViewSize(new Point(width, height))
                .rotation(rotation)
                .build();
        camera2Helper.start();
    }


    public void release() {
        if (camera2Helper != null) {
            camera2Helper.stop();
            camera2Helper.release();
            camera2Helper = null;
        }
    }

    private void stopCameraPreview() {
        if (camera2Helper != null) {
            camera2Helper.stop();
        }
    }


    @Override
    public void onSurfaceTextureAvailable(@NonNull SurfaceTexture surfaceTexture, int i, int i1) {
        startCameraPreview();
    }


    @Override
    public void onSurfaceTextureSizeChanged(@NonNull SurfaceTexture surfaceTexture, int i, int i1) {

    }

    @Override
    public boolean onSurfaceTextureDestroyed(@NonNull SurfaceTexture surfaceTexture) {
        stopCameraPreview();
        return false;
    }

    @Override
    public void onSurfaceTextureUpdated(@NonNull SurfaceTexture surfaceTexture) {
//        Log.e(TAG,"onSurfaceTextureUpdated");

    }

    @Override
    public void onCameraOpened(Size previewSize, int displayOrientation) {
        Log.e(TAG, "onCameraOpened");

    }

    @Override
    public void onPreviewFrame(byte[] yuvData) {
//        Log.e(TAG, "onPreviewFrame:"+yuvData.length);
        if (yuvData != null && isLiving && mRtmpLivePusher != null) {
            byte[] bytes = new byte[]{0x1,0x2,0x3,0x4,0x5};
            mRtmpLivePusher.pushVideoData(yuvData);
        }
    }

    @Override
    public void onCameraClosed() {
        Log.e(TAG, "onCameraClosed");

    }

    @Override
    public void onCameraError(Exception e) {

    }


}
