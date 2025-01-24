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
public class GLFBOPostProcessingView extends SurfaceView implements SurfaceHolder.Callback, GLCamera2Listener {
    private static String TAG = GLFBOPostProcessingView.class.getSimpleName();


    private OpenGLPlayCallJni mJniCall;
    private Context mContext;

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

        String vertexScreenPath = OpenGLPlayFileUtils.getModelFilePath(mContext
                , "fbo_screen_vertex.glsl");
        String fragScreenPath = OpenGLPlayFileUtils.getModelFilePath(mContext
                , "fbo_screen_fragment.glsl");
        String fragOppositionPath = OpenGLPlayFileUtils.getModelFilePath(mContext
                , "fbo_post_opposition_fragment.glsl");
        String fragGrayScalePath = OpenGLPlayFileUtils.getModelFilePath(mContext
                , "fbo_post_gray_scale_fragment.glsl");
        String fragWeightedGrayPath = OpenGLPlayFileUtils.getModelFilePath(mContext
                , "fbo_post_weighted_gray_fragment.glsl");
        String fragNuclearEffectPath = OpenGLPlayFileUtils.getModelFilePath(mContext
                , "fbo_post_nuclear_effect_fragment.glsl");

        String fYUVPath = OpenGLPlayFileUtils.getModelFilePath(mContext
                , "fbo_video_play_frament.glsl");
        String vYUVPath = OpenGLPlayFileUtils.getModelFilePath(mContext
                , "fbo_video_play_vert.glsl");
        String picPath = OpenGLPlayFileUtils.getModelFilePath(mContext
                , "yao.jpg");
        String picVertexPath = OpenGLPlayFileUtils.getModelFilePath(mContext
                , "draw_text_video_play_vert.glsl");
        String picFragPath = OpenGLPlayFileUtils.getModelFilePath(mContext
                , "draw_pic_frament.glsl");

        if (mJniCall != null) {
            mJniCall.setFBOPostProcessingGLSLPath(
                     fragScreenPath, vertexScreenPath
                    , fragOppositionPath
                    , fragGrayScalePath
                    , fragWeightedGrayPath
                    , fragNuclearEffectPath
                    , vYUVPath
                    , fYUVPath
                    , picPath
                    , picVertexPath
                    , picFragPath
            );
        }


    }

    public void setFBOPostProcessingType(int type) {
        int typeVaule = type % 5;
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
    public void onCameraOpened(Size previewSize, int displayOrientation) {

    }

    @Override
    public void onPreviewFrame(byte[] yuvData, int width, int height) {
        if (mJniCall != null) {
            mJniCall.glFboPostProcessingSurfaceDraw(yuvData, width, height, 90);
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
