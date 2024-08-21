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

    /**
     * GLCamerPre
     */
    public void setCamerPreGLSLPath(String fragString, String vertexString) {
        native_camera_pre_set_glsl_path(fragString, vertexString);
    }
    public boolean initCamerPreOpenGl() {
        return native_camera_pre_init_opengl();
    }
    public boolean setCamerPreWH(int w, int h) {
        return native_camera_pre_set_wh_opengl(w, h);
    }
    public void camerPreOpenGLRenderFrame() {
        native_camera_pre_render_frame();
    }

    private native void native_camera_pre_set_glsl_path(String fragPath, String vertexPath);
    private native boolean native_camera_pre_init_opengl();
    private native boolean native_camera_pre_set_wh_opengl(int width, int height);
    private native void native_camera_pre_render_frame();


    /**
     * 聚光手电筒
     */
    public void setFlashLightGLSLPath(String fragString, String vertexString, String picSrc1, String picSrc2) {
        native_flash_light_set_glsl_path(fragString, vertexString, picSrc1, picSrc2);
    }
    public void setFlashLightColorGLSLPath(String fragString, String vertexString) {
        native_flash_light_color_set_glsl_path(fragString, vertexString);
    }
    public boolean initFlashLightOpenGl(int w, int h) {
        return native_flash_light_init_opengl(w, h);
    }
    public void flashLightOpenGLRenderFrame() {
        native_flash_light_render_frame();
    }
    public void flashLightMoveXY(float dx, float dy, int action) {
        native_flash_light_move_xy(dx, dy, action);
    }
    public void flashLightOnScale(float scaleFactor, float focusX, float focusY, int action) {
        native_flash_light_on_scale(scaleFactor, focusX, focusY, action);
    }

    private native void native_flash_light_set_glsl_path(String fragPath, String vertexPath
            , String picSrc1, String picSrc2);
    private native void native_flash_light_color_set_glsl_path(String fragPath, String vertexPath);
    private native boolean native_flash_light_init_opengl(int width, int height);
    private native void native_flash_light_render_frame();
    private native void native_flash_light_move_xy(float dx, float dy, int action);
    private native void native_flash_light_on_scale(float scaleFactor, float focusX, float focusY, int action);

}
