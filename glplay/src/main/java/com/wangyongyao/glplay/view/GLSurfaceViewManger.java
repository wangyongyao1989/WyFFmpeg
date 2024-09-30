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
 * Create Time : 2024/9/30 20:08
 * Descibe : MyyFFmpeg com.wangyongyao.glplay.view
 */
public class GLSurfaceViewManger extends SurfaceView implements SurfaceHolder.Callback, GLCamera2Listener, Runnable {
    private static String TAG = GLSurfaceViewManger.class.getSimpleName();
    private OpenGLPlayCallJni mJniCall;
    private Context mContext;

    private int mWidth;
    private int mHeight;
    private Camera2Helper2 camera2Helper;
    private SurfaceView mSurfaceView;
    private boolean mIsDrawing;
    private Thread mThread;
    private SurfaceHolder mHolder;

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

    private void draw() {
//        Log.e(TAG, " draw======:");

    }


    @Override
    public void surfaceCreated(@NonNull SurfaceHolder holder) {
        Log.e(TAG, "surfaceCreated");
        //设置标志位为true，表示子线程可以开始运行
        mIsDrawing = true;
        //创建并启动子线程
        mThread = new Thread(this);
        mThread.start();

    }

    @Override
    public void surfaceChanged(@NonNull SurfaceHolder holder, int format, int width, int height) {
        Surface surface = holder.getSurface();
        Log.e(TAG, "onSurfaceChanged width:" + width + ",height" + height);
        if (mJniCall != null) {
            mJniCall.glSurfaceViewInit(surface, null, width, height);
        }
        mWidth = width;
        mHeight = height;
        startCameraPreview(width, height);
    }

    @Override
    public void surfaceDestroyed(@NonNull SurfaceHolder holder) {
        //设置标志位为false，表示子线程可以停止运行
        mIsDrawing = false;
        try {
            //等待子线程结束，并释放子线程对象
            mThread.join();
            mThread = null;
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
    }

    @Override
    public void run() {
        //使用一个循环来不断地刷新页面
        while (mIsDrawing) {
            //获取当前时间，用于计算绘制时间
            long start = System.currentTimeMillis();
            //调用draw方法进行绘制操作
            draw();
            //获取结束时间，用于计算绘制时间
            long end = System.currentTimeMillis();
            //如果绘制时间小于16ms，则延时一段时间，保证每秒60帧的刷新率
            if (end - start < 16) {
                try {
                    Thread.sleep(16 - (end - start));
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }
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
        mJniCall.glSurfaceViewDraw(yuvData, width, height, 90);
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


}
