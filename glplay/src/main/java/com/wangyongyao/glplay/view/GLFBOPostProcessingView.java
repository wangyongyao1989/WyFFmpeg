package com.wangyongyao.glplay.view;

import android.content.Context;
import android.opengl.GLSurfaceView;
import android.util.AttributeSet;
import android.util.Log;
import android.view.GestureDetector;
import android.view.MotionEvent;
import android.view.ScaleGestureDetector;


import com.wangyongyao.glplay.OpenGLPlayCallJni;
import com.wangyongyao.glplay.utils.OpenGLPlayFileUtils;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

/**
 * author : wangyongyao https://github.com/wangyongyao1989
 * Create Time : 2025/1/16
 * Descibe : AndroidLearnOpenGL com.wangyongyao.views
 */
public class GLFBOPostProcessingView extends GLSurfaceView implements GLSurfaceView.Renderer {
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
        getHolder().addCallback(this);
        setEGLContextClientVersion(3);
        setEGLConfigChooser(8, 8, 8, 8, 16, 0);
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

        setRenderer(this);

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


    public void onDrawFrame(GL10 gl) {
        if (mJniCall != null) {
            mJniCall.glFBOPostProcessingRenderFrame();
        }

    }

    public void onSurfaceChanged(GL10 gl, int width, int height) {
        Log.e(TAG, "onSurfaceChanged width:" + width + ",height" + height);
        if (mJniCall != null) {
            mJniCall.initFBOPostProcessing(width, height);
        }
        mWidth = width;
        mHeight = height;
    }


    @Override
    public void onSurfaceCreated(GL10 gl10, EGLConfig eglConfig) {
        Log.e(TAG, "onSurfaceCreated:");


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

}
