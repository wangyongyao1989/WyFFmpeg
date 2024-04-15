package com.example.myyffmpeg;

public class CallJni {

    static {
        System.loadLibrary("myyffmpeg");
    }

    public String getFFmpegVersion() {
        return native_getFFmpegVersion();
    }

    public void initFFmpeg() {
        native_initFFmpeg();
    }

    public String callStringForJNI() {
        return stringFromJNI();
    }

    public String callIntForJNI() {
        return intFromJNI(55) +"岁";
    }

    public void setVideoTransURL(String inputUrl) {
        setInputUrl(inputUrl);
    }

    private native String stringFromJNI();
    private native int intFromJNI(int age);

    private native void setInputUrl(String inputUrl);

    private native long native_initFFmpeg();

    private native String native_getFFmpegVersion();

}
