//  Author : wangyongyao https://github.com/wangyongyao1989
// Created by MMM on 2024/10/11.
// 来源于google开源项目 https://github.com/google/grafika WindowSurface.java类的改造。


#include "../includeopengl/WindowSurface.h"
#include <assert.h>

WindowSurface::WindowSurface(EglCore *eglCore, ANativeWindow *window, bool releaseSurface)
        : EglSurfaceBase(eglCore) {
    mSurface = window;
    createWindowSurface(mSurface);
    mReleaseSurface = releaseSurface;
}

WindowSurface::WindowSurface(EglCore *eglCore, ANativeWindow *window)
        : EglSurfaceBase(eglCore) {
    createWindowSurface(window);
    mSurface = window;
}

void WindowSurface::release() {
    releaseEglSurface();
    if (mSurface != NULL) {
        ANativeWindow_release(mSurface);
        mSurface = NULL;
    }

}

void WindowSurface::recreate(EglCore *eglCore) {
    assert(mSurface != NULL);
    if (mSurface == NULL) {
        LOGE("not yet implemented ANativeWindow");
        return;
    }
    mEglCore = eglCore;
    createWindowSurface(mSurface);
}