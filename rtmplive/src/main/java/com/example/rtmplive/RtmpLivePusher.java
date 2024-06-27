package com.example.rtmplive;


import android.util.Log;

public class RtmpLivePusher {

    private static final String TAG = RtmpLivePusher.class.getSimpleName();

    static {
        System.loadLibrary("rtmplive");
    }

    private native void native_rtmp_init();


}
