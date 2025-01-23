package com.wangyongyao.glplay.view;

import android.content.Context;
import android.graphics.Point;
import android.opengl.GLSurfaceView;
import android.util.AttributeSet;
import android.util.Log;
import android.util.Size;
import android.view.GestureDetector;
import android.view.MotionEvent;
import android.view.ScaleGestureDetector;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;


import androidx.annotation.NonNull;

import com.wangyongyao.glplay.OpenGLPlayCallJni;
import com.wangyongyao.glplay.camera.Camera2Helper2;
import com.wangyongyao.glplay.camera.GLCamera2Listener;
import com.wangyongyao.glplay.utils.OpenGLPlayFileUtils;


/**
 * author : wangyongyao https://github.com/wangyongyao1989
 * Create Time : 2025/1/16
 * Descibe : AndroidLearnOpenGL com.wangyongyao.views
 */
public class GLFBOPostProcessingView extends SurfaceView implements SurfaceHolder.Callback , GLCamera2Listener {
    private static String TAG = GLFBOPostProcessingView.class.getSimpleName();

    private GestureDetector gestureDetector;
    private ScaleGestureDetector scaleGestureDetector;

    private OpenGLPlayCallJni mJniCall;
    private Context mContext;
    private boolean isScaleGesture;

    private float downX;
    private float downY;
    private int mWidth;
    private int mHeight;

    private SurfaceHolder mHolder;
    private Surface mSurface;
    private Camera2Helper2 camera2Helper;


    public GLFBOPostProcessingView(Context context, OpenGLPlayCallJni jniCall) {
        super(context);
        mContext = context;
        mJniCall = jniCall;
        init();
    }

    public GLFBOPostProcessingView(Context context, AttributeSet attrs) {
        super(context, attrs);
        mContext = context;
        init();
    }

    private void init() {
        //获取SurfaceHolder对象
        mHolder = getHolder();
        //注册SurfaceHolder的回调方法
        mHolder.addCallback(this);

        String fragPath = OpenGLPlayFileUtils.getModelFilePath(mContext, "fbo_fragment.glsl");
        String vertexPath = OpenGLPlayFileUtils.getModelFilePath(mContext, "fbo_vertex.glsl");


        String picSrc1 = OpenGLPlayFileUtils.getModelFilePath(mContext
                , "diffuse_map_container2.png");
        String picSrc2 = OpenGLPlayFileUtils.getModelFilePath(mContext
                , "metal.png");

        String fragScreenPath = OpenGLPlayFileUtils.getModelFilePath(mContext
                , "fbo_post_opposition_fragment.glsl");
        String vertexScreenPath = OpenGLPlayFileUtils.getModelFilePath(mContext
                , "fbo_screen_vertex.glsl");

        String fragGrayScalePath = OpenGLPlayFileUtils.getModelFilePath(mContext
                , "fbo_post_gray_scale_fragment.glsl");
        String fragWeightedGrayPath = OpenGLPlayFileUtils.getModelFilePath(mContext
                , "fbo_post_weighted_gray_fragment.glsl");
        String fragNuclearEffectPath = OpenGLPlayFileUtils.getModelFilePath(mContext
                , "fbo_post_nuclear_effect_fragment.glsl");

        if (mJniCall != null) {
            mJniCall.setFBOPostProcessingGLSLPath(fragPath, vertexPath
                    , fragScreenPath, vertexScreenPath
                    , picSrc1, picSrc2
                    , fragGrayScalePath
                    , fragWeightedGrayPath
                    , fragNuclearEffectPath
            );
        }


        gestureDetector = new GestureDetector(getContext(), new GestureDetector.SimpleOnGestureListener());
        scaleGestureDetector = new ScaleGestureDetector(getContext()
                , new ScaleGestureDetector.OnScaleGestureListener() {
            @Override
            public boolean onScale(ScaleGestureDetector detector) {
                // 处理缩放事件
                float scaleFactor = detector.getScaleFactor();
                mJniCall.glFBOPostProcessingOnScale(scaleFactor, detector.getFocusX()
                        , detector.getFocusY(), 2);
                return true;
            }

            @Override
            public boolean onScaleBegin(ScaleGestureDetector detector) {
                // 开始缩放事件
//                Log.e(TAG, "onScaleBegin: " + detector);
                mJniCall.glFBOPostProcessingOnScale(detector.getScaleFactor(), detector.getFocusX()
                        , detector.getFocusY(), 1);
                return true;
            }

            @Override
            public void onScaleEnd(ScaleGestureDetector detector) {
                // 结束缩放事件
//                Log.e(TAG, "onScaleEnd: " + detector);
                mJniCall.glFBOPostProcessingOnScale(detector.getScaleFactor(), detector.getFocusX()
                        , detector.getFocusY(), 3);
                isScaleGesture = false;
            }
        });


    }

    public void setFBOPostProcessingType(int type) {
        int typeVaule = type % 4;
        if (mJniCall != null) {
            mJniCall.glFBOPostProcessingSetParameters(typeVaule);
        }
    }

    public int getFBOPostProcessingType() {
        int type = 0;
        if (mJniCall != null) {
            type = mJniCall.glFBOPostProcessingGetParameters();
        }
        return type;
    }

    @Override
    public void surfaceCreated(@NonNull SurfaceHolder holder) {
        Log.e(TAG, "surfaceCreated");
        mSurface = holder.getSurface();
        if (mJniCall != null) {
            mJniCall.glPSSurfaceCreated(mSurface, null);
        }
    }

    @Override
    public void surfaceChanged(@NonNull SurfaceHolder holder, int format, int width, int height) {
        Log.e(TAG, "onSurfaceChanged width:" + width + ",height" + height
                + "===surface:" + mSurface.toString());
        if (mJniCall != null) {
            mJniCall.initFBOPostProcessing(width, height);
        }
        startCameraPreview(width, height);
    }

    @Override
    public void surfaceDestroyed(@NonNull SurfaceHolder holder) {
        if (mJniCall != null) {
            mJniCall.glFboSurfaceDestroy();
        }
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
                mJniCall.glFBOPostProcessingMoveXY(0, 0, 1);
            }
            break;
            case MotionEvent.ACTION_MOVE: {
//                Log.e(TAG, "onTouchEvent: " + event.getAction());
                float dx = event.getX() - downX;
                float dy = event.getY() - downY;
//                Log.e(TAG, "ACTION_MOVE:dx= "
//                        + dx + "==dy:" + dy);
                mJniCall.glFBOPostProcessingMoveXY(dx, dy, 2);
            }
            break;
            case MotionEvent.ACTION_UP: {
//                Log.e(TAG, "onTouchEvent: " + event.getAction());
                downX = 0;
                downY = 0;
                mJniCall.glFBOPostProcessingMoveXY(0, 0, 3);
            }
            break;
        }


        return true;
    }

    @Override
    public void onCameraOpened(Size previewSize, int displayOrientation) {

    }

    @Override
    public void onPreviewFrame(byte[] yuvData, int width, int height) {
        if (mJniCall != null) {
            mJniCall.glFBOPostProcessingRenderFrame();
        }
    }

    @Override
    public void onCameraClosed() {

    }

    @Override
    public void onCameraError(Exception e) {

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
}
