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

    public void glSurfaceViewNewStartRecord(String recordPath) {
        native_surfaceview_new_video_start_record(recordPath);
    }

    public void glSurfaceViewNewStopRecord() {
        native_surfaceview_new_video_stop_record();
    }


    private native void native_surfaceview_new_video_init(int type, String vertexPath, String fragPath);

    private native void native_surfaceview_new_video_created(Surface surface, AssetManager assetManager);

    private native void native_surfaceview_new_video_changed(int width, int height);

    private native void native_surfaceview_new_video_render();

    private native void native_surfaceview_new_video_draw(byte[] data, int width, int height, int rotation);

    private native void native_surfaceview_new_video_destroy();

    private native void native_surfaceview_new_video_start_record(String recordPath);

    private native void native_surfaceview_new_video_stop_record();


    /*********************** GL视频中绘制文本并录制********************/
    public void glDrawTextSurfaceInit(int type, String vertexPath, String fragPath, String fragPath1) {
        native_draw_text_surface_init(type, vertexPath, fragPath, fragPath1);
    }

    public void glTextSharderPath(String vertexPath, String fragPath, String freeTypePath) {
        native_text_sharder_path(vertexPath, fragPath, freeTypePath);
    }

    public void glDrawTextPicPath(String vertexPath) {
        native_draw_text_pic_path(vertexPath);
    }

    public void glDrawTextSurfaceCreated(Surface surface, AssetManager assetManager) {
        native_draw_text_surface_created(surface, assetManager);
    }

    public void glDrawTextSurfaceChanged(int width, int height) {
        native_draw_text_surface_changed(width, height);
    }

    public void glDrawTextSurfaceRender() {
        native_draw_text_surface_render();
    }

    public void glDrawTextSurfaceDraw(byte[] data, int width, int height, int rotation) {
        native_draw_text_surface_draw(data, width, height, rotation);
    }

    public void glDrawTextSurfaceDestroy() {
        native_draw_text_surface_destroy();
    }

    public void glDrawTextSurfaceStartRecord(String recordPath) {
        native_draw_text_surface_start_record(recordPath);
    }

    public void glDrawTextSurfaceStopRecord() {
        native_draw_text_surface_stop_record();
    }


    private native void native_draw_text_surface_init(int type, String vertexPath, String fragPath, String fragPath1);

    private native void native_text_sharder_path(String vertexPath, String fragPath, String freeTypePath);

    private native void native_draw_text_pic_path(String picPath);

    private native void native_draw_text_surface_created(Surface surface, AssetManager assetManager);

    private native void native_draw_text_surface_changed(int width, int height);

    private native void native_draw_text_surface_render();

    private native void native_draw_text_surface_draw(byte[] data, int width, int height, int rotation);

    private native void native_draw_text_surface_destroy();

    private native void native_draw_text_surface_start_record(String recordPath);

    private native void native_draw_text_surface_stop_record();

    /*********************** GL 使用帧缓冲FBO在视频中绘制文本并录制********************/

    public void glFboInit(int type, String vertexPath, String fragPath, String fragPath1) {
        native_fbo_surface_init(type, vertexPath, fragPath, fragPath1);
    }

    public void glFboTextPath(String vertexPath, String fragPath, String freeTypePath) {
        native_fbo_sharder_path(vertexPath, fragPath, freeTypePath);
    }

    public void glFboPicPath(String vertexPath) {
        native_fbo_pic_path(vertexPath);
    }


    public void glFBOPostProcessingPath(String vScreenPath, String fScreenPath) {
        native_fbo_screen_path(vScreenPath, fScreenPath);
    }


    public void glFboSurfaceCreated(Surface surface, AssetManager assetManager) {
        native_fbo_surface_created(surface, assetManager);
    }

    public void glFboSurfaceChanged(int width, int height) {
        native_fbo_surface_changed(width, height);
    }

    public void glFboSurfaceRender() {
        native_fbo_surface_render();
    }

    public void glFboSurfaceDraw(byte[] data, int width, int height, int rotation) {
        native_fbo_surface_draw(data, width, height, rotation);
    }

    public void glFboSurfaceDestroy() {
        native_fbo_surface_destroy();
    }

    public void glFboSurfaceStartRecord(String recordPath) {
        native_fbo_surface_start_record(recordPath);
    }

    public void glFboSurfaceStopRecord() {
        native_fbo_surface_stop_record();
    }


    private native void native_fbo_surface_init(int type, String vertexPath, String fragPath, String fragPath1);

    private native void native_fbo_sharder_path(String vertexPath, String fragPath, String freeTypePath);

    private native void native_fbo_pic_path(String picPath);

    private native void native_fbo_screen_path(String vScreenPath, String fScreenPath);


    private native void native_fbo_surface_created(Surface surface, AssetManager assetManager);

    private native void native_fbo_surface_changed(int width, int height);

    private native void native_fbo_surface_render();

    private native void native_fbo_surface_draw(byte[] data, int width, int height, int rotation);

    private native void native_fbo_surface_destroy();

    private native void native_fbo_surface_start_record(String recordPath);

    private native void native_fbo_surface_stop_record();

    /**
     * 帧缓冲FBO——后期处理
     */
    public void setFBOPostProcessingGLSLPath(String fragString, String vertexString
            , String fragScreenString, String vertexScreenString
            , String picSrc1, String picSrc2
            , String fragGrayScalePath
            , String fragWeightedGrayPath
            , String fragNuclearEffectPath
    ) {
        native_fbo_post_processing_set_glsl_path(fragString, vertexString
                , fragScreenString, vertexScreenString
                , picSrc1, picSrc2
                , fragGrayScalePath
                , fragWeightedGrayPath
                , fragNuclearEffectPath
        );
    }

    public void glPSSurfaceCreated(Surface surface, AssetManager assetManager) {
        native_fbo_ps_surface_created(surface, assetManager);
    }

    public boolean initFBOPostProcessing(int w, int h) {
        return native_fbo_post_processing_init_opengl(w, h);
    }

    public void glFBOPostProcessingRenderFrame() {
        native_fbo_post_processing_render_frame();
    }

    public void glFboPostProcessingSurfaceDraw(byte[] data, int width, int height, int rotation) {
        native_fbo_ps_surface_draw(data, width, height, rotation);
    }

    public void glFBOPostProcessingSetParameters(int params) {
        native_fbo_post_processing_set_parameters(params);
    }

    public int glFBOPostProcessingGetParameters() {
        return native_fbo_post_processing_get_parameters();
    }

    private native void native_fbo_post_processing_set_glsl_path(String fragPath, String vertexPath
            , String fragScreenString, String vertexScreenString
            , String picSrc1, String picSrc2
            , String fragGrayScalePath
            , String fragWeightedGrayPath
            , String fragNuclearEffectPath

    );

    private native boolean native_fbo_post_processing_init_opengl(int width, int height);

    private native void native_fbo_ps_surface_created(Surface surface, AssetManager assetManager);

    private native void native_fbo_post_processing_render_frame();

    private native void native_fbo_ps_surface_draw(byte[] data, int width, int height, int rotation);


    private native void native_fbo_post_processing_set_parameters(int params);

    private native int native_fbo_post_processing_get_parameters();

}
