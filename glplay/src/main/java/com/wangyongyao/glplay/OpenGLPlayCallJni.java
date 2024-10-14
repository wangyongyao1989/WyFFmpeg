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


    /*********************** OpenGL 聚光手电筒*******************/
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

    /*********************** OpenGL Texture显示视频********************/
    public void glTextureVideoPlayCreate(int type, String vertexPath, String fragPath) {
        native_texture_video_play_create(type, vertexPath, fragPath);
    }

    public void glTextureVideoPlayDestroy() {
        native_texture_video_play_destroy();
    }

    public void glTextureVideoPlayInit(Surface surface, AssetManager assetManager, int width, int height) {
        native_texture_video_play_init(surface, assetManager, width, height);
    }

    public void glTextureVideoPlayRender() {
        native_texture_video_play_render();
    }

    public void glTextureVideoPlayDraw(byte[] data, int width, int height, int rotation) {
        native_texture_video_play_draw(data, width, height, rotation);
    }

    public void glTextureVideoPlaySetParameters(int params) {
        native_texture_video_play_set_parameters(params);
    }

    public int glTextureVideoPlayGetParameters() {
        return native_texture_video_play_get_parameters();
    }

    private native void native_texture_video_play_create(int type, String vertexPath, String fragPath);

    private native void native_texture_video_play_destroy();

    private native void native_texture_video_play_init(Surface surface, AssetManager assetManager, int width, int height);

    private native void native_texture_video_play_render();

    private native void native_texture_video_play_draw(byte[] data, int width, int height, int rotation);

    private native void native_texture_video_play_set_parameters(int params);

    private native int native_texture_video_play_get_parameters();


    /*********************** OpenGL Texture显示滤镜视频********************/
    public void glTextureFilterPlayerCreate(int type, String vertexPath
            , String fragPath
            , String fragPath1
            , String fragPath2
            , String fragPath3
            , String fragPath4
            , String fragPath5
            , String fragPath6
            , String fragPath7
            , String fragPath8
            , String fragPath9
            , String fragPath10
            , String fragPath11
            , String fragPath12


    ) {
        native_texture_filter_player_create(type, vertexPath
                , fragPath
                , fragPath1
                , fragPath2
                , fragPath3
                , fragPath4
                , fragPath5
                , fragPath6
                , fragPath7
                , fragPath8
                , fragPath9
                , fragPath10
                , fragPath11
                , fragPath12

        );
    }

    public void glTextureFilterPlayerDestroy() {
        native_texture_filter_player_destroy();
    }

    public void glTextureFilterPlayerInit(Surface surface, AssetManager assetManager, int width, int height) {
        native_texture_filter_player_init(surface, assetManager, width, height);
    }

    public void glTextureFilterPlayerRender() {
        native_texture_filter_player_render();
    }

    public void glTextureFilterPlayerDraw(byte[] data, int width, int height, int rotation) {
        native_texture_filter_player_draw(data, width, height, rotation);
    }

    public void glTextureFilterPlayerSetParameters(int params) {
        native_texture_filter_player_set_parameters(params);
    }

    public int glTextureFilterPlayerGetParameters() {
        return native_texture_filter_player_get_parameters();
    }

    private native void native_texture_filter_player_create(int type, String vertexPath
            , String fragPath
            , String fragPath1
            , String fragPath2
            , String fragPath3
            , String fragPath4
            , String fragPath5
            , String fragPath6
            , String fragPath7
            , String fragPath8
            , String fragPath9
            , String fragPath10
            , String fragPath11
            , String fragPath12

    );

    private native void native_texture_filter_player_destroy();

    private native void native_texture_filter_player_init(Surface surface, AssetManager assetManager, int width, int height);

    private native void native_texture_filter_player_render();

    private native void native_texture_filter_player_draw(byte[] data, int width, int height, int rotation);

    private native void native_texture_filter_player_set_parameters(int params);

    private native int native_texture_filter_player_get_parameters();


    /*********************** OpenGL SurfaceView 显示视频********************/
    public void glSurfaceViewCreate(int type, String vertexPath, String fragPath) {
        native_surfaceview_video_create(type, vertexPath, fragPath);
    }

    public void glSurfaceViewDestroy() {
        native_surfaceview_video_destroy();
    }

    public void glSurfaceViewInit(Surface surface, AssetManager assetManager, int width, int height) {
        native_surfaceview_video_init(surface, assetManager, width, height);
    }

    public void glSurfaceViewRender() {
        native_surfaceview_video_render();
    }

    public void glSurfaceViewDraw(byte[] data, int width, int height, int rotation) {
        native_surfaceview_video_draw(data, width, height, rotation);
    }

    public void glSurfaceViewSetParameters(int params) {
        native_surfaceview_video_set_parameters(params);
    }

    public int glSurfaceViewGetParameters() {
        return native_surfaceview_video_get_parameters();
    }

    private native void native_surfaceview_video_create(int type, String vertexPath, String fragPath);

    private native void native_surfaceview_video_destroy();

    private native void native_surfaceview_video_init(Surface surface, AssetManager assetManager, int width, int height);

    private native void native_surfaceview_video_render();

    private native void native_surfaceview_video_draw(byte[] data, int width, int height, int rotation);

    private native void native_surfaceview_video_set_parameters(int params);

    private native int native_surfaceview_video_get_parameters();



    /*********************** OpenGL SurfaceViewNew 显示视频********************/
    public void glSurfaceViewNewInit(int type, String vertexPath, String fragPath) {
        native_surfaceview_new_video_init(type, vertexPath, fragPath);
    }

    public void glSurfaceViewNewCreated(Surface surface, AssetManager assetManager) {
        native_surfaceview_new_video_created(surface, assetManager);
    }

    public void glSurfaceViewNewChanged(int width, int height) {
        native_surfaceview_new_video_changed(width, height);
    }

    public void glSurfaceViewNewRender() {
        native_surfaceview_new_video_render();
    }

    public void glSurfaceViewNewDraw(byte[] data, int width, int height, int rotation) {
        native_surfaceview_new_video_draw(data, width, height, rotation);
    }

    public void glSurfaceViewNewDestroy() {
        native_surfaceview_new_video_destroy();
    }


    private native void native_surfaceview_new_video_init(int type, String vertexPath, String fragPath);

    private native void native_surfaceview_new_video_created(Surface surface, AssetManager assetManager);

    private native void native_surfaceview_new_video_changed(int width, int height);

    private native void native_surfaceview_new_video_render();

    private native void native_surfaceview_new_video_draw(byte[] data, int width, int height, int rotation);

    private native void native_surfaceview_new_video_destroy();


}
