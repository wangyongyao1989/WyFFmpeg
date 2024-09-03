package com.wangyongyao.glplay.view;

import android.content.Context;
import android.opengl.GLSurfaceView;
import android.util.AttributeSet;
import android.util.Log;


import com.wangyongyao.glplay.OpenGLPlayCallJni;
import com.wangyongyao.glplay.camera.CameraHelper;
import com.wangyongyao.glplay.utils.OpenGLPlayFileUtils;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

/**
 *
 */
public class GLTextureCPlusVideoPlayerView extends GLSurfaceView implements GLSurfaceView.Renderer {


    private static String TAG = GLTextureCPlusVideoPlayerView.class.getSimpleName();
    private OpenGLPlayCallJni mJniCall;
    private Context mContext;

//    private SurfaceTexture surfaceTexture;

    private int mWidth;
    private int mHeight;
    private CameraHelper mCameraHelper;


    public GLTextureCPlusVideoPlayerView(Context context, OpenGLPlayCallJni jniCall) {
        super(context);
        mContext = context;
        mJniCall = jniCall;
        init();
    }

    public GLTextureCPlusVideoPlayerView(Context context, AttributeSet attrs) {
        super(context, attrs);
        mContext = context;
        init();
    }

    private void init() {
        getHolder().addCallback(this);
        setEGLContextClientVersion(3);
        setEGLConfigChooser(8, 8, 8, 8, 16, 0);
        String fragPath = OpenGLPlayFileUtils.getModelFilePath(mContext, "texture_video_play_frament.glsl");
        String vertexPath = OpenGLPlayFileUtils.getModelFilePath(mContext, "texture_video_play_vert.glsl");
        String picSrc1 = OpenGLPlayFileUtils.getModelFilePath(mContext, "wall.jpg");

        if (mJniCall != null) {
            mJniCall.setTextureVieoPlayGLSLPath(fragPath, vertexPath);
        }
        setRenderer(this);
    }



    private void stopCameraPreview() {

    }


    public void onDrawFrame(GL10 gl) {
        if (mJniCall != null) {
            mJniCall.textureVieoPlayRender();
        }
        requestRender();
    }

    public void onSurfaceChanged(GL10 gl, int width, int height) {
        Log.e(TAG, "onSurfaceChanged width:" + width + ",height" + height);
        if (mJniCall != null) {
            mJniCall.setTextureVieoPlayWH(width, height);
        }
        mWidth = width;
        mHeight = height;
//        startCameraPreview(mWidth, mHeight);
//        mCameraHelper.initialize(width, height);
    }


    @Override
    public void onSurfaceCreated(GL10 gl10, EGLConfig eglConfig) {
        Log.e(TAG, "onSurfaceCreated:");
        if (mJniCall != null) {
            mJniCall.textureVieoPlayInit(null, null);
        }
//        startCameraPreview(mWidth, mHeight);
//        mCameraHelper = new CameraHelper(getContext());
    }

//
//    @Override
//    public void onPreviewFrame(byte[] yuvData, int width, int height) {
////        Log.e(TAG, "onPreviewFrame:" + yuvData.length);
//        if (mJniCall != null && yuvData != null && yuvData.length > 0) {
//            mJniCall.textureVieoPlayDraw(yuvData, width, height);
//        }
//    }


    public void destroyRender() {
        if (mJniCall != null) {
            mJniCall.textureVieoPlayDestroy();
        }
        stopCameraPreview();
    }

}
