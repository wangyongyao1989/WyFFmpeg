package com.example.rtmplive;

import android.Manifest;
import android.app.Activity;
import android.content.Context;
import android.content.pm.PackageManager;
import android.graphics.Point;
import android.graphics.SurfaceTexture;
import android.media.AudioFormat;
import android.media.AudioRecord;
import android.media.MediaRecorder;
import android.util.Log;
import android.util.Size;
import android.view.TextureView;

import androidx.annotation.NonNull;
import androidx.core.app.ActivityCompat;
import androidx.lifecycle.LifecycleOwner;

import com.example.rtmplive.camera.Camera2Helper;
import com.example.rtmplive.camera.Camera2Listener;

import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

public class LiveManger implements TextureView.SurfaceTextureListener, Camera2Listener {

    private static final String TAG = LiveManger.class.getSimpleName();

    private final TextureView mTextureView;
    private Context mContext;
    private int rotation = 0;
    private Camera2Helper camera2Helper;
    private RtmpLivePusher mRtmpLivePusher;
    private LifecycleOwner mLifecycleOwner;
    private int width = 640;
    private int height = 480;
    private int videoBitRate = 800000;// kb/s
    private int videoFrameRate = 10;
    private boolean isLiving;
    private Size previewSize;
    private int rotateDegree = 90;

    private ExecutorService executor;
    private int sampleRate = 44100;
    private int channelConfig = AudioFormat.CHANNEL_IN_STEREO;
    private int audioFormat = AudioFormat.ENCODING_PCM_16BIT;
    private int numChannels = 2;
    private int inputSamples;

    private AudioRecord mAudioRecord;

    public LiveManger(LifecycleOwner lifecycleOwner, Context context
            , TextureView textureView) {
        mContext = context;
        mLifecycleOwner = lifecycleOwner;
        mTextureView = textureView;
        mTextureView.setSurfaceTextureListener(this);
        initPusher();
    }

    private void initPusher() {
        mRtmpLivePusher = new RtmpLivePusher();
        executor = Executors.newSingleThreadExecutor();
        initAudio();
    }

    public void startRtmpPush(String path) {
        isLiving = true;
        mRtmpLivePusher.startRtmpPush(path);
        executor.submit(new AudioTask());
    }

    public void stopRtmpPush() {
        mRtmpLivePusher.stopRtmpPush();
        isLiving = false;

    }

    public void releaseRtmp() {
        mRtmpLivePusher.releaseRtmp();
    }

    public void pauseRtmp() {
        mRtmpLivePusher.pauseRtmp();
    }

    private void initAudio() {
        int minBufferSize = AudioRecord.getMinBufferSize(sampleRate,
                channelConfig, audioFormat) * 2;

        int bufferSizeInBytes = Math.max(minBufferSize, mRtmpLivePusher.getInputSamplesFromNative());
        if (ActivityCompat.checkSelfPermission(mContext
                , Manifest.permission.RECORD_AUDIO) != PackageManager.PERMISSION_GRANTED) {
            // TODO: Consider calling
            //    ActivityCompat#requestPermissions
            // here to request the missing permissions, and then overriding
            //   public void onRequestPermissionsResult(int requestCode, String[] permissions,
            //                                          int[] grantResults)
            // to handle the case where the user grants the permission. See the documentation
            // for ActivityCompat#requestPermissions for more details.
            return;
        }
        mRtmpLivePusher.setAudioCodecInfo(sampleRate, channelConfig);
        inputSamples = mRtmpLivePusher.getInputSamplesFromNative();
        mAudioRecord = new AudioRecord(MediaRecorder.AudioSource.MIC, sampleRate,
                channelConfig, audioFormat, bufferSizeInBytes);
    }

    private void startCameraPreview() {
        if (mContext instanceof Activity) {
            rotation = ((Activity) mContext).getWindowManager().getDefaultDisplay().getRotation();
        }
        camera2Helper = new Camera2Helper.Builder()
                .cameraListener(this)
                .specificCameraId(Camera2Helper.CAMERA_ID_BACK)
                .context(mContext.getApplicationContext())
                .previewOn(mTextureView)
                .previewViewSize(new Point(width, height))
                .rotation(rotation)
                .build();
        camera2Helper.start();
    }


    public void release() {
        if (camera2Helper != null) {
            camera2Helper.stop();
            camera2Helper.release();
            camera2Helper = null;
        }
    }

    private void stopCameraPreview() {
        if (camera2Helper != null) {
            camera2Helper.stop();
        }
    }


    @Override
    public void onSurfaceTextureAvailable(@NonNull SurfaceTexture surfaceTexture, int i, int i1) {
        startCameraPreview();
    }


    @Override
    public void onSurfaceTextureSizeChanged(@NonNull SurfaceTexture surfaceTexture, int i, int i1) {

    }

    @Override
    public boolean onSurfaceTextureDestroyed(@NonNull SurfaceTexture surfaceTexture) {
        stopCameraPreview();
        return false;
    }

    @Override
    public void onSurfaceTextureUpdated(@NonNull SurfaceTexture surfaceTexture) {
//        Log.e(TAG,"onSurfaceTextureUpdated");

    }

    @Override
    public void onCameraOpened(Size previewSize, int displayOrientation) {
        Log.e(TAG, "onCameraOpened");
        this.previewSize = previewSize;
        updateVideoCodecInfo(rotateDegree);
    }

    @Override
    public void onPreviewFrame(byte[] yuvData) {
//        Log.e(TAG, "onPreviewFrame:"+yuvData.length);
        if (yuvData != null && isLiving && mRtmpLivePusher != null) {
            byte[] bytes = new byte[]{0x1, 0x2, 0x3, 0x4, 0x5};
            mRtmpLivePusher.pushVideoData(yuvData);
        }
    }

    @Override
    public void onCameraClosed() {
        Log.e(TAG, "onCameraClosed");

    }

    @Override
    public void onCameraError(Exception e) {

    }


    private void updateVideoCodecInfo(int degree) {
        camera2Helper.updatePreviewDegree(degree);
        if (mRtmpLivePusher != null) {
            int width = previewSize.getWidth();
            int height = previewSize.getHeight();
            if (degree == 90 || degree == 270) {
                int temp = width;
                width = height;
                height = temp;
            }
            mRtmpLivePusher.setVideoCodecInfo(width, height, videoFrameRate, videoBitRate);
        }
    }

    class AudioTask implements Runnable {

        @Override
        public void run() {
            mAudioRecord.startRecording();
            byte[] bytes = new byte[inputSamples];
            while (isLiving) {
                int len = mAudioRecord.read(bytes, 0, bytes.length);
                if (len > 0) {
                    Log.e(TAG, "mAudioRecord len: " + len);
                    mRtmpLivePusher.pushAudioData(bytes);
                }
            }
            mAudioRecord.stop();
        }
    }

}
