package com.wangyongyao.glplay.test;

import android.opengl.GLSurfaceView;

import com.wangyongyao.glplay.OpenGLPlayCallJni;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

public class GLVideoRenderer  implements GLSurfaceView.Renderer {

    private static final String TAG = GLVideoRenderer.class.getSimpleName();
    private GLSurfaceView mGLSurface;
    private OpenGLPlayCallJni mOpenGLPlayCallJni;

    public GLVideoRenderer() {
//        create(Type.GL_YUV420_FILTER.getValue());
        mOpenGLPlayCallJni = new OpenGLPlayCallJni();
        mOpenGLPlayCallJni.glTextureVideoPlayCreate(0,null,null);

    }

    public void init(GLSurfaceView glSurface) {
        mGLSurface = glSurface;
        // Create an OpenGL ES 2 context.
        mGLSurface.setEGLContextClientVersion(2);
        mGLSurface.setRenderer(this);
        mGLSurface.setRenderMode(GLSurfaceView.RENDERMODE_WHEN_DIRTY);
    }

    public void destroyRender() {
        mOpenGLPlayCallJni.glTextureVideoPlayDestroy();
    }

    public void drawVideoFrame(byte[] data, int width, int height, int rotation) {
        mOpenGLPlayCallJni.glTextureVideoPlayDraw(data, width, height, rotation);
        requestRender();
    }

    public void setVideoParameters(int params) {
//        setParameters(params);
        mOpenGLPlayCallJni.glTextureVideoPlaySetParameters(params);
    }

    public int getVideoParameters() {
//        return getParameters();
        return mOpenGLPlayCallJni.glTextureVideoPlayGetParameters();

    }

    @Override
    public void onDrawFrame(GL10 gl) {
//        render();
         mOpenGLPlayCallJni.glTextureVideoPlayRender();

    }

    @Override
    public void onSurfaceChanged(GL10 gl, int width, int height) {
//        init(null, null, width, height);
         mOpenGLPlayCallJni.glTextureVideoPlayInit(null, null, width, height);

    }

    @Override
    public void onSurfaceCreated(GL10 gl, EGLConfig config) {

    }

    private void requestRender() {
        if (mGLSurface != null) {
            mGLSurface.requestRender();
        }
    }

    public void destroyRenderer() {
        mOpenGLPlayCallJni.glTextureVideoPlayDestroy();
    }
}
