package com.example.myyffmpeg.view;

import android.content.Context;
import android.graphics.Point;
import android.graphics.SurfaceTexture;
import android.opengl.GLSurfaceView;
import android.util.AttributeSet;
import android.util.Log;
import android.util.Size;
import android.view.Surface;

import com.example.myyffmpeg.FFPlayCallJni;
import com.example.myyffmpeg.utils.FileUtils;
import com.example.rtmplive.camera.Camera2Helper;
import com.example.rtmplive.camera.Camera2Helper1;
import com.example.rtmplive.camera.Camera2Listener;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

/**
 *
 */
public class GLTextureCPlusVideoPlayerView extends GLSurfaceView implements GLSurfaceView.Renderer
        , SurfaceTexture.OnFrameAvailableListener, Camera2Listener {


    private static String TAG = GLTextureCPlusVideoPlayerView.class.getSimpleName();
    private FFPlayCallJni mJniCall;
    private Context mContext;

    private SurfaceTexture surfaceTexture;

    private Camera2Helper1 camera2Helper;
    private int mWidth;
    private int mHeight;


    public GLTextureCPlusVideoPlayerView(Context context, FFPlayCallJni jniCall) {
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
        String fragPath = FileUtils.getModelFilePath(mContext, "texture_video_play_frament.glsl");
        String vertexPath = FileUtils.getModelFilePath(mContext, "texture_video_play_vert.glsl");
        String picSrc1 = FileUtils.getModelFilePath(mContext, "wall.jpg");

        if (mJniCall != null) {
            mJniCall.setTextureVieoPlayGLSLPath(fragPath, vertexPath);
        }
        setRenderer(this);
    }

    private void startCameraPreview(SurfaceTexture surfaceTexture, int width, int height) {
        if (camera2Helper == null) {
            camera2Helper = new Camera2Helper1.Builder()
                    .cameraListener(this)
                    .specificCameraId(Camera2Helper.CAMERA_ID_BACK)
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
        if (mJniCall != null) {
            mJniCall.textureVieoPlayRender();
        }
    }

    public void onSurfaceChanged(GL10 gl, int width, int height) {
        Log.e(TAG, "onSurfaceChanged width:" + width + ",height" + height);
        if (mJniCall != null) {
            mJniCall.setTextureVieoPlayWH(width, height);
        }
        mWidth = width;
        mHeight = height;
        startCameraPreview(surfaceTexture, mWidth, mHeight);
    }


    @Override
    public void onSurfaceCreated(GL10 gl10, EGLConfig eglConfig) {
        Log.e(TAG, "onSurfaceCreated:");
        if (mJniCall != null) {
            mJniCall.textureVieoPlayInit(null, null);
        }
        surfaceTexture = new SurfaceTexture(0);
        surfaceTexture.setOnFrameAvailableListener(this);
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
        if (mJniCall != null) {
            mJniCall.textureVieoPlayDraw(yuvData, width, height);
        }
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
        if (mJniCall != null) {
            mJniCall.textureVieoPlayDestroy();
        }
    }

}
