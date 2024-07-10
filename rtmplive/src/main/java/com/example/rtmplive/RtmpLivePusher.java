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

    public void stopRtmpPush() {
        native_rtmp_stop_push();
    }

    public void releaseRtmp() {
        native_rtmp_release();
    }

    public void pauseRtmp() {
        native_rtmp_pause();
    }

    private native void native_rtmp_init();

    private native void native_rtmp_start_push(String path);

    private native void native_rtmp_stop_push();

    private native void native_rtmp_release();

    private native void native_rtmp_pause();

    private native void native_init_callback();


    private void rtmpStatusMessage(String status, float msgCode) {
        Log.e(TAG, "status:" + status + "====msgCode:" + msgCode);

    }


}
