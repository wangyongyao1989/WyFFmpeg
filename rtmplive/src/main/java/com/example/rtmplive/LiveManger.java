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

import com.example.rtmplive.camera.RtmpCamera2Helper;
import com.example.rtmplive.camera.RtmpCamera2Listener;

import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

public class LiveManger implements TextureView.SurfaceTextureListener, RtmpCamera2Listener {

    private static final String TAG = LiveManger.class.getSimpleName();

    private final TextureView mTextureView;
    private Context mContext;
    private int rotation = 0;
    private RtmpCamera2Helper mRtmpCamera2Helper;
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
        if (mAudioRecord != null) {
            executor.submit(new AudioTask());
        }
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
        // CHANNEL_IN_STEREO 是"声道掩码"，值为 12，不是声道数。
        // 以前把掩码直接当声道数传给 faac，生成的 AudioSpecificConfig 里
        // channelConfiguration=12（非法），本机 mediamtx 收到 publish 后立刻断开：
        // "unable to parse MPEG4 audio config: unsupported channel configuration (12)"。
        numChannels = (channelConfig & AudioFormat.CHANNEL_IN_STEREO) != 0 ? 2 : 1;
        // 先配置 faac 再取 inputSamples：编码器还没打开时 native 返回 -1，
        // 原实现先拿它去算 bufferSizeInBytes，等于用了个无意义的值
        mRtmpLivePusher.setAudioCodecInfo(sampleRate, numChannels);
        inputSamples = mRtmpLivePusher.getInputSamplesFromNative();

        int minBufferSize = AudioRecord.getMinBufferSize(sampleRate,
                channelConfig, audioFormat) * 2;
        // faac 一帧要 inputSamples 个 16bit 采样，字节数是采样数的 2 倍
        int frameBytes = Math.max(inputSamples, 0) * 2;
        int bufferSizeInBytes = Math.max(minBufferSize, frameBytes);
        if (ActivityCompat.checkSelfPermission(mContext
                , Manifest.permission.RECORD_AUDIO) != PackageManager.PERMISSION_GRANTED) {
            // 没有录音权限时只丢音频、视频照常推流。原实现直接 return，
            // 导致 mAudioRecord 为 null，AudioTask 一上线就 NPE 并被单线程池静默吞掉
            Log.e(TAG, "RECORD_AUDIO permission denied: push video only");
            return;
        }
        mAudioRecord = new AudioRecord(MediaRecorder.AudioSource.MIC, sampleRate,
                channelConfig, audioFormat, bufferSizeInBytes);
        if (mAudioRecord.getState() != AudioRecord.STATE_INITIALIZED) {
            Log.e(TAG, "AudioRecord init failed: push video only");
            mAudioRecord.release();
            mAudioRecord = null;
        }
    }

    private void startCameraPreview() {
        if (mContext instanceof Activity) {
            rotation = ((Activity) mContext).getWindowManager().getDefaultDisplay().getRotation();
        }
        mRtmpCamera2Helper = new RtmpCamera2Helper.Builder()
                .cameraListener(this)
                .specificCameraId(RtmpCamera2Helper.CAMERA_ID_BACK)
                .context(mContext.getApplicationContext())
                .previewOn(mTextureView)
                .previewViewSize(new Point(width, height))
                .rotation(rotation)
                .build();
        mRtmpCamera2Helper.start();
    }


    public void release() {
        if (mRtmpCamera2Helper != null) {
            mRtmpCamera2Helper.stop();
            mRtmpCamera2Helper.release();
            mRtmpCamera2Helper = null;
        }
    }

    private void stopCameraPreview() {
        if (mRtmpCamera2Helper != null) {
            mRtmpCamera2Helper.stop();
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
    public void onPreviewFrame(byte[] yuvData, int width, int height) {
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
        mRtmpCamera2Helper.updatePreviewDegree(degree);
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
            // 一次要喂满 faac 的一帧：inputSamples 是采样数，16bit PCM 的字节数是它的 2 倍。
            // 原来按 inputSamples 申请字节缓冲，每次只喂进半帧，音画时长会越推越偏。
            byte[] bytes = new byte[inputSamples * 2];
            while (isLiving) {
                int len = mAudioRecord.read(bytes, 0, bytes.length);
                // native 侧按整帧长度读取，半截数据不能投喂，否则会读到上一帧的残留
                if (len == bytes.length) {
                    mRtmpLivePusher.pushAudioData(bytes);
                }
            }
            mAudioRecord.stop();
        }
    }

}
