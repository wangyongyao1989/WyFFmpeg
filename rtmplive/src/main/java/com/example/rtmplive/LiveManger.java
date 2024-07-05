package com.example.rtmplive;

import android.app.Activity;
import android.content.Context;
import android.graphics.Point;
import android.graphics.SurfaceTexture;
import android.util.Size;
import android.view.TextureView;
import androidx.annotation.NonNull;

import com.example.rtmplive.camera.Camera2Helper;
import com.example.rtmplive.camera.Camera2Listener;

public class LiveManger implements TextureView.SurfaceTextureListener, Camera2Listener {

    private final TextureView mTextureView;
    private Context mContext;
    private int rotation = 0;
    private Camera2Helper camera2Helper;
    private RtmpLivePusher mRtmpLivePusher;


    public LiveManger(Context context, TextureView textureView) {
        mContext = context;
        mTextureView = textureView;
        mTextureView.setSurfaceTextureListener(this);
        initPusher();
    }

    private void initPusher() {
        mRtmpLivePusher = new RtmpLivePusher();
    }

    public void startRtmpPush(String path) {
        mRtmpLivePusher.startRtmpPush(path);
    }

    public void stopRtmpPush() {
        mRtmpLivePusher.stopRtmpPush();
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
                .previewViewSize(new Point(640, 480))
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

    }

    @Override
    public void onCameraOpened(Size previewSize, int displayOrientation) {

    }

    @Override
    public void onPreviewFrame(byte[] yuvData) {

    }

    @Override
    public void onCameraClosed() {

    }

    @Override
    public void onCameraError(Exception e) {

    }

}
