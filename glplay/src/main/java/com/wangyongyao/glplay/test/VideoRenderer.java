package com.wangyongyao.glplay.test;

import android.content.res.AssetManager;
import android.view.Surface;

import com.wangyongyao.glplay.OpenGLPlayCallJni;

/**
 * Created by oleg on 11/2/17.
 */

public class VideoRenderer {
    protected enum Type {
        GL_YUV420(0), VK_YUV420(1), GL_YUV420_FILTER(2);

        private final int mValue;

        Type(int value) {
            mValue = value;
        }

        public int getValue() {
            return mValue;
        }
    }

    private OpenGLPlayCallJni mOpenGLPlayCallJni;

    public VideoRenderer() {
        mOpenGLPlayCallJni = new OpenGLPlayCallJni();
    }

    private long mNativeContext; // using by native

    protected void create(int type) {

    }


    protected void destroy() {
        mOpenGLPlayCallJni.gldestroy();
    }

    protected void init(Surface surface, AssetManager assetManager, int width, int height) {
        mOpenGLPlayCallJni.glinit(surface, assetManager, width, height);
    }

    protected void render() {
        mOpenGLPlayCallJni.glrender();
    }

    protected void draw(byte[] data, int width, int height, int rotation) {
        mOpenGLPlayCallJni.gldraw(data, width, height, rotation);
    }

    protected void setParameters(int params) {
        mOpenGLPlayCallJni.glsetParameters(params);
    }

    protected int getParameters() {
        return mOpenGLPlayCallJni.glgetParameters();
    }

    public  void drawVideoFrame(byte[] data, int width, int height, int rotation) {

    }

    public void destroyRenderer() {
        destroy();
    }

}
