//  Author : wangyongyao https://github.com/wangyongyao1989
// Created by MMM on 2024/9/6.
//

#ifndef MYYFFMPEG_OpenglesSurfaceViewVideoRender_H
#define MYYFFMPEG_OpenglesSurfaceViewVideoRender_H

#endif //MYYFFMPEG_OpenglesSurfaceViewVideoRender_H

#include <cstdint>
#include <memory>
#include <android/native_window.h>
#include <android/asset_manager.h>
#include "OpenGLShader.h"
#include <EGL/egl.h>
#include <GLES3/gl3.h>


struct surface_video_frame {
    size_t width;
    size_t height;
    size_t stride_y;
    size_t stride_uv;
    uint8_t *y;
    uint8_t *u;
    uint8_t *v;
};

// Vertices for a full screen quad.
static const float kVerticekSurface[8] = {
        -1.f, 1.f,
        -1.f, -1.f,
        1.f, 1.f,
        1.f, -1.f
};

// Texture coordinates for mapping entire texture.
static const float kTextureCoordSurface[8] = {
        0, 0,
        0, 1,
        1, 0,
        1, 1
};

class OpenglesSurfaceViewVideoRender {

public:
    OpenglesSurfaceViewVideoRender();

    ~OpenglesSurfaceViewVideoRender();

    void
    init(ANativeWindow *window, AAssetManager *assetManager, size_t width, size_t height);

    void render();

    void updateFrame(const surface_video_frame &frame);

    void release();

    void draw(uint8_t *buffer, size_t length, size_t width, size_t height, float rotation);

    void setParameters(uint32_t params);

    uint32_t getParameters();

    int createProgram();

    bool setSharderPath(const char *vertexPath, const char *fragmentPath);

    bool setSharderStringPath(string vertexPath, string fragmentPath);

    bool isProgramChanged = true;

    void delete_program(GLuint &program);

    GLuint m_program = 0;


private:

    bool createTextures();

    bool updateTextures();

    void deleteTextures();


    GLuint useProgram();

    void printGLString(const char *name, GLenum s);

    void checkGlError(const char *op);

    GLuint m_vertexShader = 0;
    GLuint m_pixelShader = 0;

    std::unique_ptr<uint8_t[]> m_pDataY = nullptr;

    uint8_t *m_pDataU = nullptr;
    uint8_t *m_pDataV = nullptr;

    __unused  size_t m_length = 0;
    size_t m_sizeY = 0;
    size_t m_sizeU = 0;
    size_t m_sizeV = 0;

    GLuint m_textureIdY = 0;
    GLuint m_textureIdU = 0;
    GLuint m_textureIdV = 0;

    GLuint m_vertexPos = 0;
    GLuint m_textureLoc = 0;
    GLint m_textureYLoc = 0;
    GLint m_textureULoc = 0;
    GLint m_textureVLoc = 0;
    GLint m_textureSize = 0;

    size_t m_width = 0;
    size_t m_height = 0;
    size_t m_backingWidth = 0;
    size_t m_backingHeight = 0;
    uint32_t m_params = 0;
    float m_rotation = 0;

    bool isDirty;

    OpenGLShader *openGlShader = nullptr;

    EGLDisplay display = nullptr;
    EGLSurface winsurface = nullptr;


};