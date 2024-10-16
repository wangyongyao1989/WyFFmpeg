package com.wangyongyao.glplay.view;

import android.content.Context;
import android.graphics.Point;
import android.graphics.SurfaceTexture;
import android.opengl.GLSurfaceView;
import android.util.AttributeSet;
import android.util.Log;
import android.util.Size;


import com.wangyongyao.glplay.OpenGLPlayCallJni;
import com.wangyongyao.glplay.camera.GLCamera2Helper1;
import com.wangyongyao.glplay.camera.GLCamera2Listener;
import com.wangyongyao.glplay.utils.OpenGLPlayFileUtils;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

public class GLGLCameraView extends GLSurfaceView implements GLSurfaceView.Renderer
        , SurfaceTexture.OnFrameAvailableListener, GLCamera2Listener {


    private static String TAG = GLGLCameraView.class.getSimpleName();
    private OpenGLPlayCallJni mJniCall;
    private Context mContext;

    private SurfaceTexture surfaceTexture;
    private int mTextureId = 0;

    private GLCamera2Helper1 camera2Helper;
    private int mWidth;
    private int mHeight;


    public GLGLCameraView(Context context, OpenGLPlayCallJni jniCall) {
        super(context);
        mContext = context;
        mJniCall = jniCall;
        init();
    }

    public GLGLCameraView(Context context, AttributeSet attrs) {
        super(context, attrs);
        mContext = context;
        init();
    }

    private void init() {
        getHolder().addCallback(this);
        setEGLContextClientVersion(3);
        setEGLConfigChooser(8, 8, 8, 8, 16, 0);
        String fragPath = OpenGLPlayFileUtils.getModelFilePath(mContext, "camera_pre_fragment.glsl");
        String vertexPath = OpenGLPlayFileUtils.getModelFilePath(mContext, "camera_pre_vertex.glsl");
        String picSrc1 = OpenGLPlayFileUtils.getModelFilePath(mContext, "wall.jpg");

        if (mJniCall != null) {
            mJniCall.setCamerPreGLSLPath(fragPath, vertexPath);
            mJniCall.setCamerPreGLSLPic(picSrc1);
        }
        setRenderer(this);


    }

    private void startCameraPreview(SurfaceTexture surfaceTexture, int width, int height) {
        if (camera2Helper == null) {
            camera2Helper = new GLCamera2Helper1.Builder()
                    .cameraListener(this)
//                    .specificCameraId(Camera2Helper.CAMERA_ID_BACK)
                    .context(mContext)
                    .setSurfaceTexture(surfaceTexture)
                    .previewViewSize(new Point(width, height))
                    .build();
        }
        camera2Helper.start();
    }

    private void stopCameraPreview() {
        if (camera2Helper != null) {
            camera2Helper.stop();
        }
    }


    public void onDrawFrame(GL10 gl) {
        surfaceTexture.updateTexImage();
        float[] mtx = new float[16];
        surfaceTexture.getTransformMatrix(mtx);
        if (mJniCall != null)
            mJniCall.camerPreOpenGLRenderFrame(mtx);
    }

    public void onSurfaceChanged(GL10 gl, int width, int height) {
        Log.e(TAG, "onSurfaceChanged width:" + width + ",height" + height);
        if (mJniCall != null)
            mJniCall.setCamerPreWH(width, height);
        mWidth = width;
        mHeight = height;
        startCameraPreview(surfaceTexture, mWidth, mHeight);
    }


    @Override
    public void onSurfaceCreated(GL10 gl10, EGLConfig eglConfig) {
        Log.e(TAG, "onSurfaceCreated:");
        if (mJniCall != null) {
            mTextureId = mJniCall.initCamerPreOpenGl();
        }
        surfaceTexture = new SurfaceTexture(mTextureId);
        surfaceTexture.setOnFrameAvailableListener(this);
        Log.e(TAG, "onSurfaceCreated mTextureId:" + mTextureId);
        if (mJniCall != null)
            mJniCall.setVideoTexture(mTextureId);
        startCameraPreview(surfaceTexture, mWidth, mHeight);
    }

    @Override
    public void onFrameAvailable(SurfaceTexture surfaceTexture) {
        requestRender();
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
