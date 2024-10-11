//  Author : wangyongyao https://github.com/wangyongyao1989
// Created by MMM on 2024/10/11.
// 来源于google开源项目 https://github.com/google/grafika OffscreenSurface.java类的改造。


#ifndef MYYFFMPEG_OFFSCREENSURFACE_H
#define MYYFFMPEG_OFFSCREENSURFACE_H
#include "EglSurfaceBase.h"

class OffscreenSurface : public EglSurfaceBase {
public:
    OffscreenSurface(EglCore *eglCore, int width, int height);
    void release();
};



#endif //MYYFFMPEG_OFFSCREENSURFACE_H
