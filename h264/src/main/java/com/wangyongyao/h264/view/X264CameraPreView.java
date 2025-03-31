package com.wangyongyao.h264.view;

import android.content.Context;
import android.graphics.Point;
import android.graphics.SurfaceTexture;
import android.util.AttributeSet;
import android.util.Log;
import android.util.Size;
import android.view.TextureView;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;

import com.wangyongyao.h264.camera.X264Camera2Helper;
import com.wangyongyao.h264.camera.X264Camera2Listener;

/**
 * author : wangyongyao https://github.com/wangyongyao1989
 * Create Time : 2025/3/31 16:35
 * Descibe : MyyFFmpeg com.wangyongyao.h264.view
 */
public class X264CameraPreView extends TextureView implements TextureView.SurfaceTextureListener
        , X264Camera2Listener {

    private static final String TAG = X264CameraPreView.class.getSimpleName();

    private Context mContext;
    private int width = 640;
    private int height = 480;
    private X264Camera2Helper mX264Camera2Helper;
    private int rotation = 90;


    public X264CameraPreView(@NonNull Context context) {
        super(context);
        mContext = context;
        init();
    }

    public X264CameraPreView(@NonNull Context context, @Nullable AttributeSet attrs) {
        super(context, attrs);
        mContext = context;
        init();
    }

    public X264CameraPreView(@NonNull Context context, @Nullable AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
        mContext = context;
        init();
    }

    public X264CameraPreView(@NonNull Context context, @Nullable AttributeSet attrs, int defStyleAttr, int defStyleRes) {
        super(context, attrs, defStyleAttr, defStyleRes);
        mContext = context;
        init();
    }

    private void init() {
        setSurfaceTextureListener(this);
    }

    private void startCameraPreview() {
        if (mX264Camera2Helper == null) {
            mX264Camera2Helper = new X264Camera2Helper.Builder()
                    .cameraListener(this)
                    .specificCameraId(X264Camera2Helper.CAMERA_ID_BACK)
                    .context(mContext)
                    .previewOn(this)
                    .previewViewSize(new Point(width, height))
                    .rotation(rotation)
                    .build();
        }

        mX264Camera2Helper.start();
    }

    private void stopCameraPreview() {
        if (mX264Camera2Helper != null) {
            mX264Camera2Helper.stop();
        }
    }

    public void onDestroy() {
        if (mX264Camera2Helper != null) {
            mX264Camera2Helper.stop();
            mX264Camera2Helper.release();
            mX264Camera2Helper = null;
        }
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


    @Override
    public void onCameraOpened(Size previewSize, int displayOrientation) {
        Log.e(TAG, "onCameraOpened previewSize:" + previewSize.toString()
                + "==displayOrientation:" + displayOrientation);

    }

    @Override
    public void onPreviewFrame(byte[] yuvData, int width, int height) {
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
