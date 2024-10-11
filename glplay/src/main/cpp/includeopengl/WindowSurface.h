//  Author : wangyongyao https://github.com/wangyongyao1989
// Created by MMM on 2024/10/11.
// 来源于google开源项目 https://github.com/google/grafika 中WindowSurface.java类的改造。


#ifndef MYYFFMPEG_WINDOWSURFACE_H
#define MYYFFMPEG_WINDOWSURFACE_H

#include "EglSurfaceBase.h"

class WindowSurface : public EglSurfaceBase {
public:
    WindowSurface(EglCore *eglCore, ANativeWindow *window, bool releaseSurface);

    WindowSurface(EglCore *eglCore, ANativeWindow *window);

    // 释放资源
    void release();

    // 重新创建
    void recreate(EglCore *eglCore);

private:
    ANativeWindow *mSurface;
    bool mReleaseSurface;
};


#endif //MYYFFMPEG_WINDOWSURFACE_H
