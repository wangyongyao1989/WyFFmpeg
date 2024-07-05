package com.example.rtmplive;


public class RtmpLivePusher {

    private static final String TAG = RtmpLivePusher.class.getSimpleName();

    static {
        System.loadLibrary("rtmplive");
    }

    public RtmpLivePusher() {
        initRtmp();
    }

    private void initRtmp() {
        native_rtmp_init();
    }

    public void startRtmpPush(String path) {
        native_rtmp_start_push(path);
    }

    public void stopRtmpPush() {

    }

    private native void native_rtmp_init();

    public native void native_rtmp_start_push(String path);

    public native void native_rtmp_stop_push();

}
