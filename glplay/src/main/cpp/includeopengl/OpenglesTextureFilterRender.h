//  Author : wangyongyao https://github.com/wangyongyao1989
// Created by MMM on 2024/9/14.
//

#ifndef MYYFFMPEG_OPENGLESTEXTUREFILTERRENDER_H
#define MYYFFMPEG_OPENGLESTEXTUREFILTERRENDER_H

#include "OpenglesTexureVideoRender.h"

class OpenglesTextureFilterRender : public OpenglesTexureVideoRender {
public:

    OpenglesTextureFilterRender();

    ~OpenglesTextureFilterRender();

    void render();

    void setParameters(uint32_t params);

    uint32_t getParameters();

private:
    size_t m_filter = 0;
    size_t m_prevFilter = 0;

    std::vector<const char *> m_fragmentShader;
};


#endif //MYYFFMPEG_OPENGLESTEXTUREFILTERRENDER_H
