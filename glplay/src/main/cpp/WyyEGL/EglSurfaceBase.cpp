//  Author : wangyongyao https://github.com/wangyongyao1989
// Created by MMM on 2024/10/11.
// 来源于google开源项目https://github.com/google/grafika 中的 EglSurfaceBase.java 类的改造。


#include "../includeopengl/EglSurfaceBase.h"

#include <assert.h>
#include <GLES2/gl2.h>


EglSurfaceBase::EglSurfaceBase(EglCore *eglCore) : mEglCore(eglCore) {
    mEglSurface = EGL_NO_SURFACE;
}

/**
 * 创建显示的Surface
 * @param nativeWindow
 */
void EglSurfaceBase::createWindowSurface(ANativeWindow *nativeWindow) {
    assert(mEglSurface == EGL_NO_SURFACE);
    if (mEglSurface != EGL_NO_SURFACE) {
        LOGI("surface already created\n");
        return;
    }
    mEglSurface = mEglCore->createWindowSurface(nativeWindow);
}

/**
 * 创建离屏surface
 * @param width
 * @param height
 */
void EglSurfaceBase::createOffscreenSurface(int width, int height) {
    assert(mEglSurface == EGL_NO_SURFACE);
    if (mEglSurface != EGL_NO_SURFACE) {
        LOGE("surface already created\n");
        return;
    }
    mEglSurface = mEglCore->createOffscreenSurface(width, height);
    mWidth = width;
    mHeight = height;
}

/**
 * 获取宽度
 * @return
 */
int EglSurfaceBase::getWidth() {
    if (mWidth < 0) {
        return mEglCore->querySurface(mEglSurface, EGL_WIDTH);
    } else {
        return mWidth;
    }
}

/**
 * 获取高度
 * @return
 */
int EglSurfaceBase::getHeight() {
    if (mHeight < 0) {
        return mEglCore->querySurface(mEglSurface, EGL_HEIGHT);
    } else {
        return mHeight;
    }
}

/**
 * 释放EGLSurface
 */
void EglSurfaceBase::releaseEglSurface() {
    mEglCore->releaseSurface(mEglSurface);
    mEglSurface = EGL_NO_SURFACE;
    mWidth = mHeight = -1;
}

/**
 * 切换到当前EGLContext
 */
void EglSurfaceBase::makeCurrent() {
    mEglCore->makeCurrent(mEglSurface);
}

/**
 * 交换到前台显示
 * @return
 */
bool EglSurfaceBase::swapBuffers() {
    bool result = mEglCore->swapBuffers(mEglSurface);
    if (!result) {
        LOGD("WARNING: swapBuffers() failed");
    }
    return result;
}

/**
 * 设置当前时间戳
 * @param nsecs
 */
void EglSurfaceBase::setPresentationTime(long nsecs) {
    mEglCore->setPresentationTime(mEglSurface, nsecs);
}

/**
 * 获取当前像素
 * @return
 */
char* EglSurfaceBase::getCurrentFrame() {
    char *pixels = NULL;
    glReadPixels(0, 0, getWidth(), getHeight(), GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    return pixels;
}