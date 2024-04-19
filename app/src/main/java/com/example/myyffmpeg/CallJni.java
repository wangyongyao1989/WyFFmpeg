package com.example.myyffmpeg;

import android.view.Surface;

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

    public void mp4CAvi(String inputUrl, String outputUrl) {
        native_MP4_AVI(inputUrl, outputUrl);
    }

    public void waterMark(String inputUrl, String intputPng, String outputUrl) {
        native_Water_mark(inputUrl, intputPng, outputUrl);
    }

    public void playAudio(String audioUrl) {
//        callIntForJNI();
        native_Play_Audio(audioUrl);
//        setInputUrl(audioUrl);
    }

    public void stopAudio() {
        native_Stop_Audio();
    }

    public void playVideo(String videoUrl, Surface surface) {
        native_Play_Video(videoUrl, surface);
    }

    public void stopVideo() {
        stopVideo();
    }

    public String callStringForJNI() {
        return stringFromJNI();
    }

    public String callIntForJNI() {
        return intFromJNI(55) + "岁";
    }

    public void setVideoTransURL(String inputUrl) {
        setInputUrl(inputUrl);
    }

    private native String stringFromJNI();

    private native int intFromJNI(int age);

    private native void setInputUrl(String inputUrl);

    private native long native_initFFmpeg();

    private native void native_MP4_AVI(String inputUrl, String outputUrl);

    private native void native_Water_mark(String inputUrl, String inputPng, String outputUrl);

    private native void native_Play_Audio(String audio);

    private native void native_Stop_Audio();

    private native void native_Play_Video(String audio, Surface surface);

    private native void native_Stop_Video();

    private native String native_getFFmpegVersion();

}
