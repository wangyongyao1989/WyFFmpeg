//  Author : wangyongyao https://github.com/wangyongyao1989
// Created by MMM on 2024/9/14.
//

#include "../includeopengl/OpenglesTextureFilterRender.h"

void OpenglesTextureFilterRender::render() {
    if (m_filter != m_prevFilter) {
        m_prevFilter = m_filter;
        if (m_filter >= 0 && m_filter < m_fragmentStringPathes.size()) {
            isProgramChanged = true;
            delete_program(m_program);
            LOGI("render---m_filter：%d", m_filter);
            setSharderStringPath(m_vertexStringPath
                                 , m_fragmentStringPathes.at(m_filter));
            createProgram();
        }
    }

    OpenglesTexureVideoRender::render();
}


OpenglesTextureFilterRender::OpenglesTextureFilterRender() {

}

OpenglesTextureFilterRender::~OpenglesTextureFilterRender() {

}


void OpenglesTextureFilterRender::setParameters(uint32_t params) {
    m_filter = params;
}

uint32_t OpenglesTextureFilterRender::getParameters() {
    return m_filter;
}

bool OpenglesTextureFilterRender::setSharderStringPath(string vertexPath, string fragmentPath) {

    return OpenglesTexureVideoRender::setSharderStringPath(vertexPath, fragmentPath);
}

bool OpenglesTextureFilterRender::setSharderStringPathes(string vertexPath,
                                                         vector<string> fragmentPathes) {
    m_fragmentStringPathes = fragmentPathes;
    m_vertexStringPath = vertexPath;
    return OpenglesTexureVideoRender::setSharderStringPath(vertexPath,
                                                           m_fragmentStringPathes.front());
}
