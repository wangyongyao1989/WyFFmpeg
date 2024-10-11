package com.wangyongyao.glplay.view;

import android.content.Context;
import android.graphics.Point;
import android.opengl.GLSurfaceView;
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

import java.lang.ref.WeakReference;


/**
 * author : wangyongyao https://github.com/wangyongyao1989
 * Create Time : 2024/9/30 20:08
 * Descibe : MyyFFmpeg com.wangyongyao.glplay.view
 */
public class GLSurfaceViewManger extends SurfaceView implements SurfaceHolder.Callback, GLCamera2Listener {
    private static String TAG = GLSurfaceViewManger.class.getSimpleName();
    private OpenGLPlayCallJni mJniCall;
    private Context mContext;

    private int mWidth;
    private int mHeight;
    private Camera2Helper2 camera2Helper;
    private SurfaceHolder mHolder;

    private final WeakReference<GLSurfaceViewManger> mThisWeakRef =
            new WeakReference<GLSurfaceViewManger>(this);

    private WeakReference<GLSurfaceViewManger> mGLSurfaceViewWeakRef;


    public GLSurfaceViewManger(Context context, OpenGLPlayCallJni jniCall) {
        super(context);
        Log.e(TAG, "GLSurfaceViewManger");
        mContext = context;
        mJniCall = jniCall;
        init();
    }

    public GLSurfaceViewManger(Context context, AttributeSet attrs) {
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
        String fragPath = OpenGLPlayFileUtils.getModelFilePath(mContext, "texture_video_play_frament.glsl");
        String vertexPath = OpenGLPlayFileUtils.getModelFilePath(mContext, "texture_video_play_vert.glsl");
        mJniCall.glSurfaceViewCreate(0, vertexPath, fragPath);
    }


    private void stopCameraPreview() {
        if (camera2Helper != null) {
            camera2Helper.stop();
        }
    }


    @Override
    public void surfaceCreated(@NonNull SurfaceHolder holder) {
        Log.e(TAG, "surfaceCreated");

    }

    @Override
    public void surfaceChanged(@NonNull SurfaceHolder holder, int format, int width, int height) {
        Surface surface = holder.getSurface();
        Log.e(TAG, "onSurfaceChanged width:" + width + ",height" + height
                + "===surface:" + surface.toString());
//        Log.e(TAG, "surfaceChanged: "+Thread.currentThread().getName());
        if (mJniCall != null) {
            mJniCall.glSurfaceViewInit(surface, null, width, height);
        }
        mWidth = width;
        mHeight = height;
        startCameraPreview(width, height);
    }

    @Override
    public void surfaceDestroyed(@NonNull SurfaceHolder holder) {

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
//        Log.e(TAG, "onPreviewFrame: "+Thread.currentThread().getName());

        if (mJniCall != null) {
            mJniCall.glSurfaceViewDraw(yuvData, width, height, 90);
            mJniCall.glSurfaceViewRender();
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
        mJniCall.glSurfaceViewDestroy();
        stopCameraPreview();
    }


    class GLThread extends Thread {
        GLThread(WeakReference<GLSurfaceViewManger> glSurfaceViewWeakRef) {
            super();
            mWidth = 0;
            mHeight = 0;
            mGLSurfaceViewWeakRef = glSurfaceViewWeakRef;
        }

        @Override
        public void run() {
            super.run();
        }
    }


}
