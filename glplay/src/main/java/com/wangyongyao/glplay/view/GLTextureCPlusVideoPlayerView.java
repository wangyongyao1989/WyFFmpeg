package com.wangyongyao.glplay.view;

import android.content.Context;
import android.opengl.GLSurfaceView;
import android.util.AttributeSet;
import android.util.Log;


import com.wangyongyao.glplay.OpenGLPlayCallJni;
import com.wangyongyao.glplay.camerahelper.camerahelper.CameraDataHelper;
import com.wangyongyao.glplay.camerahelper.camerahelper.CameraDataListener;
import com.wangyongyao.glplay.utils.OpenGLPlayFileUtils;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

/**
 *
 */
public class GLTextureCPlusVideoPlayerView extends GLSurfaceView implements GLSurfaceView.Renderer
        , CameraDataListener {


    private static String TAG = GLTextureCPlusVideoPlayerView.class.getSimpleName();
    private OpenGLPlayCallJni mJniCall;
    private Context mContext;


    private int mWidth;
    private int mHeight;
    private CameraDataHelper mCameraHelper;


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
        mCameraHelper = new CameraDataHelper(getContext(), this);
        mCameraHelper.startCamera();

//        if (mJniCall != null) {
//            mJniCall.setTextureVieoPlayGLSLPath(fragPath, vertexPath);
//        }

        mJniCall.glcreate(0);

        setRenderer(this);
        setRenderMode(GLSurfaceView.RENDERMODE_WHEN_DIRTY);

    }


    private void stopCameraPreview() {
        mCameraHelper.destroy();
    }


    public void onDrawFrame(GL10 gl) {
//        if (mJniCall != null) {
//            mJniCall.textureVieoPlayRender();
//        }
        if (mJniCall != null) {
            mJniCall.glrender();
        }

    }

    public void onSurfaceChanged(GL10 gl, int width, int height) {
        Log.e(TAG, "onSurfaceChanged width:" + width + ",height" + height);
//        if (mJniCall != null) {
//            mJniCall.setTextureVieoPlayWH(width, height);
//        }
        if (mJniCall != null) {
            mJniCall.glinit(null, null, width, height);
        }
        mWidth = width;
        mHeight = height;
        mCameraHelper.initialize(width, height);
    }


    @Override
    public void onSurfaceCreated(GL10 gl10, EGLConfig eglConfig) {
        Log.e(TAG, "onSurfaceCreated:");
//        if (mJniCall != null) {
//            mJniCall.textureVieoPlayInit(null, null);
//        }

    }


    @Override
    public void onPreviewFrame(byte[] yuvData, int width, int height) {
//        if (mJniCall != null && yuvData != null && yuvData.length > 0) {
//            mJniCall.textureVieoPlayDraw(yuvData, width, height);
//        }
        mJniCall.gldraw(yuvData, width, height, 90);
        requestRender();
    }


    public void destroyRender() {
//        if (mJniCall != null) {
//            mJniCall.textureVieoPlayDestroy();
//        }
        mJniCall.gldestroy();
        stopCameraPreview();
    }

}
