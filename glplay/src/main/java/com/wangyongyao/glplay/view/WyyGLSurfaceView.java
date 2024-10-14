package com.wangyongyao.glplay.view;

import android.content.Context;
import android.graphics.Point;
import android.os.Handler;
import android.os.Looper;
import android.os.Message;
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
public class WyyGLSurfaceView extends SurfaceView implements SurfaceHolder.Callback, GLCamera2Listener {
    private static String TAG = WyyGLSurfaceView.class.getSimpleName();
    private OpenGLPlayCallJni mJniCall;
    private Context mContext;

    private int mWidth;
    private int mHeight;
    private Camera2Helper2 camera2Helper;
    private SurfaceHolder mHolder;

    private final WeakReference<WyyGLSurfaceView> mThisWeakRef =
            new WeakReference<WyyGLSurfaceView>(this);

    private MyGLRendererThread mMyGLRendererThread;
    private Surface mSurface;


    public WyyGLSurfaceView(Context context, OpenGLPlayCallJni jniCall) {
        super(context);
        Log.e(TAG, "GLSurfaceViewManger");
        mContext = context;
        mJniCall = jniCall;
        init();
    }

    public WyyGLSurfaceView(Context context, AttributeSet attrs) {
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

        mMyGLRendererThread = new MyGLRendererThread(mThisWeakRef);
        mMyGLRendererThread.start();

    }


    private void stopCameraPreview() {
        if (camera2Helper != null) {
            camera2Helper.stop();
        }
    }


    @Override
    public void surfaceCreated(@NonNull SurfaceHolder holder) {
        Log.e(TAG, "surfaceCreated");
        mMyGLRendererThread.getHandler().sendSurfaceCreated();
    }

    @Override
    public void surfaceChanged(@NonNull SurfaceHolder holder, int format, int width, int height) {
        mSurface = holder.getSurface();
        Log.e(TAG, "onSurfaceChanged width:" + width + ",height" + height
                + "===surface:" + mSurface.toString());
//        Log.e(TAG, "surfaceChanged: "+Thread.currentThread().getName());
        mMyGLRendererThread.getHandler().sendSurfaceChanged(format, width, height);

        mWidth = width;
        mHeight = height;
        startCameraPreview(width, height);
    }

    @Override
    public void surfaceDestroyed(@NonNull SurfaceHolder holder) {
        mMyGLRendererThread.getHandler().sendShutdown();
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
        }
        mMyGLRendererThread.getHandler().sendDoFrame(1000);
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


    class MyGLRendererThread extends Thread {

        private volatile RenderHandler mRenderHandler;
        private WeakReference<WyyGLSurfaceView> mGLSurfaceViewWeakRef;

        MyGLRendererThread(WeakReference<WyyGLSurfaceView> glSurfaceViewWeakRef) {
            super();
            mWidth = 0;
            mHeight = 0;
            mGLSurfaceViewWeakRef = glSurfaceViewWeakRef;
        }

        @Override
        public void run() {
            Looper.prepare();
            mRenderHandler = new RenderHandler(this);

            Looper.loop();

        }

        public RenderHandler getHandler() {
            return mRenderHandler;
        }


        public void surfaceCreated() {
            Log.e(TAG, "onSurfaceChanged: " + Thread.currentThread().getName());
            String fragPath = OpenGLPlayFileUtils.getModelFilePath(mContext
                    , "texture_video_play_frament.glsl");
            String vertexPath = OpenGLPlayFileUtils.getModelFilePath(mContext
                    , "texture_video_play_vert.glsl");
            mJniCall.glSurfaceViewCreate(0, vertexPath, fragPath);
        }

        public void surfaceChanged(int width, int height) {
            if (mJniCall != null) {
                mJniCall.glSurfaceViewInit(mSurface, null, width, height);
            }
        }


        public void doFrame(long timestamp) {
            if (mJniCall != null)
                mJniCall.glSurfaceViewRender();

        }


        public void setRecordingEnabled(boolean b) {

        }

        public void setRecordMethod(int arg1) {

        }

        public void shutdown() {
            if (mJniCall != null) {
                mJniCall.glSurfaceViewDestroy();
            }
            Looper.myLooper().quit();

        }
    }

    //
    private static class RenderHandler extends Handler {
        private static final int MSG_SURFACE_CREATED = 0;
        private static final int MSG_SURFACE_CHANGED = 1;
        private static final int MSG_DO_FRAME = 2;
        private static final int MSG_RECORDING_ENABLED = 3;
        private static final int MSG_RECORD_METHOD = 4;
        private static final int MSG_SHUTDOWN = 5;
        private WeakReference<MyGLRendererThread> mWeakRenderThread;

        public RenderHandler(MyGLRendererThread rt) {
            mWeakRenderThread = new WeakReference<MyGLRendererThread>(rt);
        }

        public void sendSurfaceCreated() {
            sendMessage(obtainMessage(RenderHandler.MSG_SURFACE_CREATED));
        }

        public void sendSurfaceChanged(@SuppressWarnings("unused") int format,
                                       int width, int height) {
            // ignore format
            sendMessage(obtainMessage(RenderHandler.MSG_SURFACE_CHANGED, width, height));
        }

        public void sendDoFrame(long frameTimeNanos) {
            sendMessage(obtainMessage(RenderHandler.MSG_DO_FRAME,
                    (int) (frameTimeNanos >> 32), (int) frameTimeNanos));
        }

        public void setRecordingEnabled(boolean enabled) {
            sendMessage(obtainMessage(MSG_RECORDING_ENABLED, enabled ? 1 : 0, 0));
        }

        public void setRecordMethod(int recordMethod) {
            sendMessage(obtainMessage(MSG_RECORD_METHOD, recordMethod, 0));
        }

        public void sendShutdown() {
            sendMessage(obtainMessage(RenderHandler.MSG_SHUTDOWN));
        }

        @Override  // runs on RenderThread
        public void handleMessage(Message msg) {
            int what = msg.what;
            //Log.d(TAG, "RenderHandler [" + this + "]: what=" + what);

            MyGLRendererThread renderThread = mWeakRenderThread.get();
            if (renderThread == null) {
                Log.w(TAG, "RenderHandler.handleMessage: weak ref is null");
                return;
            }

            switch (what) {
                case MSG_SURFACE_CREATED:
                    renderThread.surfaceCreated();
                    break;
                case MSG_SURFACE_CHANGED:
                    renderThread.surfaceChanged(msg.arg1, msg.arg2);
                    break;
                case MSG_DO_FRAME:
                    long timestamp = (((long) msg.arg1) << 32) |
                            (((long) msg.arg2) & 0xffffffffL);
                    renderThread.doFrame(timestamp);
                    break;
                case MSG_RECORDING_ENABLED:
                    renderThread.setRecordingEnabled(msg.arg1 != 0);
                    break;
                case MSG_RECORD_METHOD:
                    renderThread.setRecordMethod(msg.arg1);
                    break;
                case MSG_SHUTDOWN:
                    renderThread.shutdown();
                    break;
                default:
                    throw new RuntimeException("unknown message " + what);
            }
        }

    }


}
