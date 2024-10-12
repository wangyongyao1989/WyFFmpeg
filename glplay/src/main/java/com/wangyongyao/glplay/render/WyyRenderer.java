package com.wangyongyao.glplay.render;

import android.view.Surface;

import com.wangyongyao.glplay.OpenGLPlayCallJni;

/**
 * author : wangyongyao https://github.com/wangyongyao1989
 * Create Time : 2024/10/11
 * Descibe : MyyFFmpeg com.wangyongyao.glplay.render
 */
public class WyyRenderer {

    private OpenGLPlayCallJni mJniCall;

    public WyyRenderer(OpenGLPlayCallJni jniCall) {
        this.mJniCall = jniCall;
    }

    public void init() {
        mJniCall.wyyRendererInit();
    }

    public void release() {
        mJniCall.wyyRendererSurfaceRelease();
    }

    public void surfaceCreated(Surface surface) {
        mJniCall.wyyRendererSurfaceCreated(surface);
    }

    public void surfaceChanged(int width, int height) {
        mJniCall.wyyRendererSurfaceChanged(width, height);
    }

    public void surfaceDestroyed() {
        mJniCall.wyyRendererSurfaceDestroyed();
    }


}
