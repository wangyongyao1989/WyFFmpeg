package com.example.myyffmpeg.view;

import android.content.Context;
import android.graphics.Point;
import android.graphics.SurfaceTexture;
import android.opengl.GLSurfaceView;
import android.util.AttributeSet;
import android.util.Log;
import android.util.Size;
import android.view.GestureDetector;
import android.view.MotionEvent;
import android.view.ScaleGestureDetector;

import com.example.myyffmpeg.FFPlayCallJni;
import com.example.myyffmpeg.utils.FileUtils;
import com.example.rtmplive.camera.Camera2Helper;
import com.example.rtmplive.camera.Camera2Helper1;
import com.example.rtmplive.camera.Camera2Listener;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

public class GLCameraView extends GLSurfaceView implements GLSurfaceView.Renderer
        , SurfaceTexture.OnFrameAvailableListener, Camera2Listener {

    private GestureDetector gestureDetector;
    private ScaleGestureDetector scaleGestureDetector;

    private static String TAG = GLCameraView.class.getSimpleName();
    private FFPlayCallJni mJniCall;
    private Context mContext;
    private boolean isScaleGesture;

    private float downX;
    private float downY;

    private SurfaceTexture surfaceTexture;
    private int mTextureId = 0;

    private Camera2Helper1 camera2Helper;
    private int mWidth;
    private int mHeight;


    public GLCameraView(Context context, FFPlayCallJni jniCall) {
        super(context);
        mContext = context;
        mJniCall = jniCall;
        init();
    }

    public GLCameraView(Context context, AttributeSet attrs) {
        super(context, attrs);
        mContext = context;
        init();
    }

    private void init() {
        getHolder().addCallback(this);
        setEGLContextClientVersion(3);
        setEGLConfigChooser(8, 8, 8, 8, 16, 0);
        String fragPath = FileUtils.getModelFilePath(mContext, "camera_pre_fragment.glsl");
        String vertexPath = FileUtils.getModelFilePath(mContext, "camera_pre_vertex.glsl");


        if (mJniCall != null) {
            mJniCall.setCamerPreGLSLPath(fragPath, vertexPath);
        }
        setRenderer(this);

        gestureDetector = new GestureDetector(getContext(), new GestureDetector.SimpleOnGestureListener());
        scaleGestureDetector = new ScaleGestureDetector(getContext()
                , new ScaleGestureDetector.OnScaleGestureListener() {
            @Override
            public boolean onScale(ScaleGestureDetector detector) {
                // 处理缩放事件
                float scaleFactor = detector.getScaleFactor();
//                Log.e(TAG, "onScale scaleFactor: " + scaleFactor
//                        + "==getFocusX:" + detector.getFocusX()
//                        + "===getFocusY" + detector.getFocusY());
                mJniCall.camerPreOnScale(scaleFactor, detector.getFocusX()
                        , detector.getFocusY(), 2);
                return true;
            }

            @Override
            public boolean onScaleBegin(ScaleGestureDetector detector) {
                // 开始缩放事件
//                Log.e(TAG, "onScaleBegin: " + detector);
                mJniCall.camerPreOnScale(detector.getScaleFactor(), detector.getFocusX()
                        , detector.getFocusY(), 1);
                return true;
            }

            @Override
            public void onScaleEnd(ScaleGestureDetector detector) {
                // 结束缩放事件
//                Log.e(TAG, "onScaleEnd: " + detector);
                mJniCall.camerPreOnScale(detector.getScaleFactor(), detector.getFocusX()
                        , detector.getFocusY(), 3);
                        isScaleGesture = false;
            }
        });
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
//        Log.e(TAG, "onDrawFrame:");

        if (mJniCall != null)
            mJniCall.camerPreOpenGLRenderFrame();
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
        if (mJniCall != null)
            mJniCall.initCamerPreOpenGl();
        surfaceTexture = new SurfaceTexture(mTextureId);
        surfaceTexture.setOnFrameAvailableListener(this);
    }

    @Override
    public void onFrameAvailable(SurfaceTexture surfaceTexture) {
        Log.e(TAG, "onFrameAvailable:");
        requestRender();
    }


    @Override
    public boolean onTouchEvent(MotionEvent event) {
        if (isScaleGesture) {
            gestureDetector.onTouchEvent(event);
            scaleGestureDetector.onTouchEvent(event);
            return true;
        }
        switch (event.getAction()) {
            case MotionEvent.ACTION_POINTER_2_DOWN: {
                isScaleGesture = true;
            }
            break;
            case MotionEvent.ACTION_POINTER_2_UP: {
                isScaleGesture = false;
            }
            break;
            case MotionEvent.ACTION_DOWN: {
//                Log.e(TAG, "onTouchEvent: " + event.getAction());
                downX = event.getX();
                downY = event.getY();
                mJniCall.camerPreMoveXY(0, 0, 1);
            }
            break;
            case MotionEvent.ACTION_MOVE: {
//                Log.e(TAG, "onTouchEvent: " + event.getAction());
                float dx = event.getX() - downX;
                float dy = event.getY() - downY;
//                Log.e(TAG, "ACTION_MOVE:dx= "
//                        + dx + "==dy:" + dy);
                mJniCall.camerPreMoveXY(dx, dy, 2);
            }
            break;
            case MotionEvent.ACTION_UP: {
//                Log.e(TAG, "onTouchEvent: " + event.getAction());
                downX = 0;
                downY = 0;
                mJniCall.camerPreMoveXY(0, 0, 3);
            }
            break;
        }


        return true;
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
