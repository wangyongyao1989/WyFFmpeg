package com.wangyongyao.glplay.view;

import android.content.Context;
import android.graphics.Point;
import android.util.AttributeSet;
import android.util.Log;
import android.util.Size;
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
 * Create Time : 2024/11/6 16:24
 * Descibe : MyyFFmpeg com.wangyongyao.glplay.view
 */
public class GLDrawTextSurfaceView extends SurfaceView implements SurfaceHolder.Callback, GLCamera2Listener {
    private static String TAG = GLDrawTextSurfaceView.class.getSimpleName();
    private OpenGLPlayCallJni mJniCall;
    private Context mContext;

    private int mWidth;
    private int mHeight;
    private Camera2Helper2 camera2Helper;
    private SurfaceHolder mHolder;
    private Surface mSurface;


    public GLDrawTextSurfaceView(Context context, OpenGLPlayCallJni jniCall) {
        super(context);
        mContext = context;
        mJniCall = jniCall;
        init();
    }

    public GLDrawTextSurfaceView(Context context, AttributeSet attrs) {
        super(context, attrs);
        mContext = context;
        init();
    }


    @Override
    protected void onAttachedToWindow() {
        super.onAttachedToWindow();

    }

    private void init() {
        //获取SurfaceHolder对象
        mHolder = getHolder();
        //注册SurfaceHolder的回调方法
        mHolder.addCallback(this);

        String fragPath = OpenGLPlayFileUtils.getModelFilePath(mContext
                , "draw_text_video_play_frament.glsl");
        String vertexPath = OpenGLPlayFileUtils.getModelFilePath(mContext
                , "draw_text_video_play_vert.glsl");
        mJniCall.glDrawTextSurfaceInit(0, vertexPath, fragPath);
        String picPath = OpenGLPlayFileUtils.getModelFilePath(mContext
                , "yao.jpg");
        mJniCall.glDrawTextPicPath(picPath);

    }


    private void stopCameraPreview() {
        if (camera2Helper != null) {
            camera2Helper.stop();
        }
    }


    @Override
    public void surfaceCreated(@NonNull SurfaceHolder holder) {
        Log.e(TAG, "surfaceCreated");
        mSurface = holder.getSurface();
        if (mJniCall != null) {
            mJniCall.glDrawTextSurfaceCreated(mSurface, null);
        }
    }

    @Override
    public void surfaceChanged(@NonNull SurfaceHolder holder, int format, int width, int height) {
        Log.e(TAG, "onSurfaceChanged width:" + width + ",height" + height
                + "===surface:" + mSurface.toString());
        if (mJniCall != null) {
            mJniCall.glDrawTextSurfaceChanged(width, height);
        }
        mWidth = width;
        mHeight = height;
        startCameraPreview(width, height);
    }

    @Override
    public void surfaceDestroyed(@NonNull SurfaceHolder holder) {
        if (mJniCall != null) {
            mJniCall.glDrawTextSurfaceDestroy();
        }
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
//        Log.e(TAG, "onPreviewFrame" );
        if (mJniCall != null) {
            mJniCall.glDrawTextSurfaceDraw(yuvData, width, height, 90);
            mJniCall.glDrawTextSurfaceRender();
        }
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
        mJniCall.glDrawTextSurfaceDestroy();
        stopCameraPreview();
    }

    public void startRecord(String path) {
        mJniCall.glDrawTextSurfaceStartRecord(path);
    }

    public void stopRecord() {
        mJniCall.glDrawTextSurfaceStopRecord();
    }


}