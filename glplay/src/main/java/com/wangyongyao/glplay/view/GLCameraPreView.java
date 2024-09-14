package com.wangyongyao.glplay.view;

import android.content.Context;
import android.graphics.Point;
import android.graphics.SurfaceTexture;
import android.util.AttributeSet;
import android.util.Log;
import android.util.Size;
import android.view.TextureView;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;

import com.wangyongyao.glplay.camera.GLCamera2Helper;
import com.wangyongyao.glplay.camera.GLCamera2Listener;


/**
 * author : wangyongyao https://github.com/wangyongyao1989
 * Create Time : 2024/8/20 18:43
 * Descibe : MyyFFmpeg com.example.rtmplive.camera
 */

public class GLCameraPreView extends TextureView implements TextureView.SurfaceTextureListener
        , GLCamera2Listener {

    private static final String TAG = GLCameraPreView.class.getSimpleName();

    private Context mContext;
    private int width = 640;
    private int height = 480;
    private GLCamera2Helper mGLCamera2Helper;
    private int rotation = 90;


    public GLCameraPreView(@NonNull Context context) {
        super(context);
        mContext = context;
        init();
    }

    public GLCameraPreView(@NonNull Context context, @Nullable AttributeSet attrs) {
        super(context, attrs);
        mContext = context;
        init();
    }

    public GLCameraPreView(@NonNull Context context, @Nullable AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
        mContext = context;
        init();
    }

    public GLCameraPreView(@NonNull Context context, @Nullable AttributeSet attrs, int defStyleAttr, int defStyleRes) {
        super(context, attrs, defStyleAttr, defStyleRes);
        mContext = context;
        init();
    }

    private void init() {
        setSurfaceTextureListener(this);
    }

    private void startCameraPreview() {
        if (mGLCamera2Helper == null) {
            mGLCamera2Helper = new GLCamera2Helper.Builder()
                    .cameraListener(this)
                    .specificCameraId(GLCamera2Helper.CAMERA_ID_BACK)
                    .context(mContext)
                    .previewOn(this)
                    .previewViewSize(new Point(width, height))
                    .rotation(rotation)
                    .build();
        }

        mGLCamera2Helper.start();
    }

    private void stopCameraPreview() {
        if (mGLCamera2Helper != null) {
            mGLCamera2Helper.stop();
        }
    }

    public void onDestroy() {
        if (mGLCamera2Helper != null) {
            mGLCamera2Helper.stop();
            mGLCamera2Helper.release();
            mGLCamera2Helper = null;
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
