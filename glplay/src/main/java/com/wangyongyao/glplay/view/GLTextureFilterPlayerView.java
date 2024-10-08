package com.wangyongyao.glplay.view;

import android.content.Context;
import android.graphics.Point;
import android.opengl.GLSurfaceView;
import android.util.AttributeSet;
import android.util.Log;
import android.util.Size;

import com.wangyongyao.glplay.OpenGLPlayCallJni;
import com.wangyongyao.glplay.camera.Camera2Helper2;
import com.wangyongyao.glplay.camera.GLCamera2Listener;
import com.wangyongyao.glplay.utils.OpenGLPlayFileUtils;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

/**
 *
 */
public class GLTextureFilterPlayerView extends GLSurfaceView implements GLSurfaceView.Renderer
        , GLCamera2Listener {
    private static String TAG = GLTextureFilterPlayerView.class.getSimpleName();
    private OpenGLPlayCallJni mJniCall;
    private Context mContext;

    private int mWidth;
    private int mHeight;
    private Camera2Helper2 camera2Helper;

    public GLTextureFilterPlayerView(Context context, OpenGLPlayCallJni jniCall) {
        super(context);
        mContext = context;
        mJniCall = jniCall;
        init();
    }

    public GLTextureFilterPlayerView(Context context, AttributeSet attrs) {
        super(context, attrs);
        mContext = context;
        init();
    }

    private void init() {
        getHolder().addCallback(this);
        setEGLContextClientVersion(3);
        setEGLConfigChooser(8, 8, 8, 8, 16, 0);
        String fragPath = OpenGLPlayFileUtils
                .getModelFilePath(mContext, "texture_video_play_frament.glsl");
        String fragPath1 = OpenGLPlayFileUtils
                .getModelFilePath(mContext, "texture_filter1_play_frament.glsl");
        String fragPath2 = OpenGLPlayFileUtils
                .getModelFilePath(mContext, "texture_filter2_play_frament.glsl");
        String fragPath3 = OpenGLPlayFileUtils
                .getModelFilePath(mContext, "texture_filter3_play_frament.glsl");
        String fragPath4 = OpenGLPlayFileUtils
                .getModelFilePath(mContext, "texture_filter4_play_frament.glsl");
        String fragPath5 = OpenGLPlayFileUtils
                .getModelFilePath(mContext, "texture_filter5_play_frament.glsl");
        String fragPath6 = OpenGLPlayFileUtils
                .getModelFilePath(mContext, "texture_filter6_play_frament.glsl");
        String fragPath7 = OpenGLPlayFileUtils
                .getModelFilePath(mContext, "texture_filter7_play_frament.glsl");
        String fragPath8 = OpenGLPlayFileUtils
                .getModelFilePath(mContext, "texture_filter8_play_frament.glsl");
        String fragPath9 = OpenGLPlayFileUtils
                .getModelFilePath(mContext, "texture_filter9_play_frament.glsl");
        String fragPath10 = OpenGLPlayFileUtils
                .getModelFilePath(mContext, "texture_filter10_play_frament.glsl");
        String fragPath11 = OpenGLPlayFileUtils
                .getModelFilePath(mContext, "texture_filter11_play_frament.glsl");
        String fragPath12 = OpenGLPlayFileUtils
                .getModelFilePath(mContext, "texture_filter12_play_frament.glsl");


        String vertexPath = OpenGLPlayFileUtils
                .getModelFilePath(mContext, "texture_video_play_vert.glsl");
        mJniCall.glTextureFilterPlayerCreate(0, vertexPath
                , fragPath
                , fragPath1
                , fragPath2
                , fragPath3
                , fragPath4
                , fragPath5
                , fragPath6
                , fragPath7
                , fragPath8
                , fragPath9
                , fragPath10
                , fragPath11
                , fragPath12

        );

        setRenderer(this);
        setRenderMode(GLSurfaceView.RENDERMODE_WHEN_DIRTY);

    }

    public void setFilterType(int type) {
        int typeVaule = type % 13;
        if (mJniCall != null) {
            mJniCall.glTextureFilterPlayerSetParameters(typeVaule);
        }
    }

    public int getFilterType() {
        int type = 0;
        if (mJniCall != null) {
           type = mJniCall.glTextureFilterPlayerGetParameters();
        }
        return type;
    }
    private void stopCameraPreview() {
        if (camera2Helper != null) {
            camera2Helper.stop();
        }
    }


    public void onDrawFrame(GL10 gl) {
        if (mJniCall != null) {
            mJniCall.glTextureFilterPlayerRender();
        }

    }

    public void onSurfaceChanged(GL10 gl, int width, int height) {
        Log.e(TAG, "onSurfaceChanged width:" + width + ",height" + height);
        if (mJniCall != null) {
            mJniCall.glTextureFilterPlayerInit(null, null, width, height);
        }
        mWidth = width;
        mHeight = height;
        startCameraPreview(width, height);

    }


    @Override
    public void onSurfaceCreated(GL10 gl10, EGLConfig eglConfig) {
        Log.e(TAG, "onSurfaceCreated:");


    }

    private void startCameraPreview(int width, int height) {
        if (camera2Helper == null) {
            camera2Helper = new Camera2Helper2.Builder()
                    .cameraListener(this)
                    .specificCameraId(Camera2Helper2.CAMERA_ID_BACK)
                    .context(mContext)
                    .previewViewSize(new Point(width, height))
                    .rotation(90)
                    .build();
        }

        camera2Helper.start();
    }


    @Override
    public void onPreviewFrame(byte[] yuvData, int width, int height) {
        mJniCall.glTextureFilterPlayerDraw(yuvData, width, height, 90);
        requestRender();
    }

    @Override
    public void onCameraOpened(Size previewSize, int displayOrientation) {
        Log.e(TAG, "onCameraOpened previewSize:" + previewSize.toString()
                + "==displayOrientation:" + displayOrientation);
    }

    @Override
    public void onCameraClosed() {
        Log.e(TAG, "onCameraClosed:");

    }

    @Override
    public void onCameraError(Exception e) {
        Log.e(TAG, "onCameraError:" + e.toString());

    }

    public void destroyRender() {
        mJniCall.glTextureFilterPlayerDestroy();
        stopCameraPreview();
    }

}
