package com.example.rtmplive;


import android.util.Log;

public class RtmpLivePusher {

    private static final String TAG = RtmpLivePusher.class.getSimpleName();

    static {
        System.loadLibrary("rtmplive");
    }

    public RtmpLivePusher() {
        initRtmp();
        native_init_callback();
    }

    private void initRtmp() {
        native_rtmp_init();
    }

    public void startRtmpPush(String path) {
        native_rtmp_start_push(path);
    }

    public void pushVideoData(byte[] data) {
        native_push_video_data(data);
    }

    public void setVideoCodecInfo(int width, int height, int fps, int bitrate) {
        native_video_codec_info(width, height, fps, bitrate);
    }

    public void stopRtmpPush() {
        native_rtmp_stop_push();
    }

    public void releaseRtmp() {
        native_rtmp_release();
    }

    public void pauseRtmp() {
        native_rtmp_pause();
    }

    public void setAudioCodecInfo(int sampleRateInHz, int channels) {
        native_setAudioCodecInfo(sampleRateInHz, channels);
    }

    public int getInputSamplesFromNative() {
        return native_getInputSamples();
    }

    public void pushAudioData(byte[] audioData) {
        native_push_audio_data(audioData);
    }

    private native void native_rtmp_init();

    private native void native_rtmp_start_push(String path);

    private native void native_rtmp_stop_push();

    private native void native_push_video_data(byte[] data);

    private native void native_rtmp_release();

    private native void native_rtmp_pause();

    private native void native_init_callback();

    private native void native_video_codec_info(int width, int height, int fps, int bitrate);

    private native void native_setAudioCodecInfo(int sampleRateInHz, int channels);

    private native int native_getInputSamples();

    private native void native_push_audio_data(byte[] audioData);

    private void rtmpStatusMessage(String status, float msgCode) {
        Log.e(TAG, "status:" + status + "====msgCode:" + msgCode);

    }


}
