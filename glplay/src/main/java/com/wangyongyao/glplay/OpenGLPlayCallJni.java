package com.wangyongyao.glplay;

import android.content.res.AssetManager;
import android.view.Surface;

/**
 * author : wangyongyao https://github.com/wangyongyao1989
 * Create Time : 2024/9/3 14:17
 * Descibe : MyyFFmpeg com.wangyongyao.openglplay
 */
public class OpenGLPlayCallJni {

    static {
        System.loadLibrary("glplay");
    }

    /**
     * OpenGL纹理方式显示视频
     */
    public void textureVieoPlayInit(Surface surface, AssetManager assetManager) {
        native_texture_video_play_init(surface, assetManager);
    }

    public boolean setTextureVieoPlayWH(int w, int h) {
        return native_texture_video_play_wh(w, h);
    }

    public void setTextureVieoPlayGLSLPath(String fragString, String vertexString) {
        native_texture_video_play_set_glsl_path(fragString, vertexString);
    }

    public void textureVieoPlayRender() {
        native_texture_video_play_render();
    }

    public void textureVieoPlayDraw(byte[] data, int width
            , int height) {
        native_texture_video_play_draw(data, width, height);
    }

    public void textureVieoPlayDestroy() {
        native_texture_video_play_destroy();
    }

    //    public native void create(int type);
    public native void native_texture_video_play_init(Surface surface, AssetManager assetManager);

    private native boolean native_texture_video_play_wh(int width, int height);

    private native void native_texture_video_play_set_glsl_path(String fragPath, String vertexPath);

    public native void native_texture_video_play_render();

    public native void native_texture_video_play_draw(byte[] data, int width
            , int height);

    public native void native_texture_video_play_destroy();


    /**
     * GLCamerPre
     */
    public void setCamerPreGLSLPath(String fragString, String vertexString) {
        native_camera_pre_set_glsl_path(fragString, vertexString);
    }

    public void setCamerPreGLSLPic(String picSrc1) {
        native_camera_pre_set_glsl_pic(picSrc1);
    }

    public int initCamerPreOpenGl() {
        return native_camera_pre_init_opengl();
    }

    public boolean setCamerPreWH(int w, int h) {
        return native_camera_pre_set_wh_opengl(w, h);
    }

    public void setVideoTexture(int videoTexture) {
        native_camera_pre_set_video_texture(videoTexture);
    }

    public void camerPreOpenGLRenderFrame(float[] mtx) {
        native_camera_pre_render_frame(mtx);
    }

    private native void native_camera_pre_set_glsl_path(String fragPath, String vertexPath);

    private native void native_camera_pre_set_glsl_pic(String picSrc1);

    private native int native_camera_pre_init_opengl();

    private native boolean native_camera_pre_set_wh_opengl(int width, int height);

    private native void native_camera_pre_set_video_texture(int videoTexture);

    private native void native_camera_pre_render_frame(float[] mtx);


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


    private long mNativeContext; // using by native

    public static void glcreate(int type) {
        create(type);
    }

    public static void gldestroy() {
        destroy();
    }

    public static void glinit(Surface surface, AssetManager assetManager, int width, int height) {
        init(surface, assetManager, width, height);
    }

    public static void glrender() {
        render();
    }

    public static void gldraw(byte[] data, int width, int height, int rotation) {
        draw(data, width, height, rotation);
    }

    public static void glsetParameters(int params) {
        setParameters(params);
    }

    public static int glgetParameters() {
        return getParameters();
    }

    protected static native void create(int type);

    protected static native void destroy();

    protected static native void init(Surface surface, AssetManager assetManager, int width, int height);

    protected static native void render();

    protected static native void draw(byte[] data, int width, int height, int rotation);

    protected static native void setParameters(int params);

    protected static native int getParameters();
}
