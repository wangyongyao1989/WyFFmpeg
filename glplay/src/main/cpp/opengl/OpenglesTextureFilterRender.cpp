//  Author : wangyongyao https://github.com/wangyongyao1989
// Created by MMM on 2024/9/14.
//

#include "../includeopengl/OpenglesTextureFilterRender.h"

void OpenglesTextureFilterRender::render() {
    OpenglesTexureVideoRender::render();
}


OpenglesTextureFilterRender::OpenglesTextureFilterRender() {

}

OpenglesTextureFilterRender::~OpenglesTextureFilterRender() {

}


void OpenglesTextureFilterRender::setParameters(uint32_t params) {
    OpenglesTexureVideoRender::setParameters(params);
}

uint32_t OpenglesTextureFilterRender::getParameters() {
    return OpenglesTexureVideoRender::getParameters();
}

bool OpenglesTextureFilterRender::setSharderStringPath(string vertexPath, string fragmentPath) {

    return OpenglesTexureVideoRender::setSharderStringPath(vertexPath, fragmentPath);
}

bool OpenglesTextureFilterRender::setSharderStringPathes(string vertexPath, vector<string> fragmentPathes) {
    m_fragmentShader = fragmentPathes;
    return OpenglesTexureVideoRender::setSharderStringPath(vertexPath, m_fragmentShader.front());
}
