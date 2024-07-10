package com.example.myyffmpeg;

import android.util.Log;
import android.view.Surface;

public class FFPlayCallJni {

    private static final String TAG = FFPlayCallJni.class.getSimpleName();

    static {
        System.loadLibrary("myyffmpeg");
    }

    public void initCallBackListener() {
        native_callback();
    }

    public void seekToPosition(float progress) {
        native_seek_to_position(progress);
    }

    public String getFFmpegVersion() {
        return native_getFFmpegVersion();
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

    public void initPlay(String videoUrl, Surface surface) {
        native_Play_init(videoUrl, surface);
    }

    public void unInitPlay() {
        native_Play_Uninit();
    }

    public void playVideo() {
        native_Play_Video();
    }

    public void stopVideo() {
        native_Stop_Video();
    }

    public void pauseVideo() {
        native_Pause_Video();
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


    private native void native_MP4_AVI(String inputUrl, String outputUrl);

    private native void native_Water_mark(String inputUrl, String inputPng, String outputUrl);

    private native void native_Play_Audio(String audio);

    private native void native_Stop_Audio();

    private native void native_Play_init(String videoUrl, Surface surface);

    private native void native_Play_Uninit();

    private native void native_Play_Video();

    private native void native_Stop_Video();

    private native void native_Pause_Video();
    private native void native_seek_to_position(float position);


    private native void native_callback();


    private native String native_getFFmpegVersion();

    private void CppEventCallback(int msgType, float msgValue) {
        Log.e(TAG,"msgType:"+msgType+"====msgValue:"+msgValue);

    }

    private  void CppPlayStatusCallback(String status) {
        Log.e(TAG,"status:"+status);

    }

}
