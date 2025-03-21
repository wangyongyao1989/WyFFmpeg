//  Author : wangyongyao https://github.com/wangyongyao1989
// Created by MMM on 2024/9/6.
//

#ifndef MYYFFMPEG_EGLSurfaceViewVideoRender_H
#define MYYFFMPEG_EGLSurfaceViewVideoRender_H

#endif //MYYFFMPEG_EGLSurfaceViewVideoRender_H

#include <cstdint>
#include <memory>
#include <android/native_window.h>
#include <android/asset_manager.h>
#include "OpenGLShader.h"
#include <EGL/egl.h>
#include <GLES3/gl3.h>
#include "EglCore.h"
#include "WindowSurface.h"
#include "Looper.h"
#include "VideoEncoderCore.h"
#include "TextureMovieEncoder2.h"

enum {
    MSG_SurfaceCreated,
    MSG_SurfaceChanged,
    MSG_DrawFrame,
    MSG_SurfaceDestroyed,
};


struct egl_surface_video_frame {
    size_t width;
    size_t height;
    size_t stride_y;
    size_t stride_uv;
    uint8_t *y;
    uint8_t *u;
    uint8_t *v;
};

// Vertices for a full screen quad.
static const float EGLVerticek[8] = {
        -1.f, 1.f,
        -1.f, -1.f,
        1.f, 1.f,
        1.f, -1.f
};

// Texture coordinates for mapping entire texture.
static const float EGLTextureCoord[8] = {
        0, 0,
        0, 1,
        1, 0,
        1, 1
};

static const size_t BIT_RATE = 4000000;   // 4Mbps
static const size_t VIDEO_WIDTH = 1280;
static const size_t VIDEO_HEIGHT = 720;

class EGLSurfaceViewVideoRender : public Looper {

public:
    EGLSurfaceViewVideoRender();

    ~EGLSurfaceViewVideoRender();

    void surfaceCreated(ANativeWindow *window, AAssetManager *assetManager);

    void surfaceChanged(size_t width, size_t height);

    void render();

    void updateFrame(const egl_surface_video_frame &frame);

    void release();

    void draw(uint8_t *buffer, size_t length, size_t width, size_t height, float rotation);

    void setParameters(uint32_t params);

    uint32_t getParameters();

    bool setSharderPath(const char *vertexPath, const char *fragmentPath);

    bool setSharderStringPath(string vertexPath, string fragmentPath);

    void startEncoder(const char *recordPath);

    void stopEncoder();

private:

    void handleMessage(LooperMessage *msg);

    void OnSurfaceCreated();
    void OnSurfaceChanged(int w, int h);
    void OnDrawFrame();
    void OnSurfaceDestroyed();

    bool CreateFrameBufferObj();

    bool createTextures();

    bool updateTextures();

    void deleteTextures();

    int createProgram();

    GLuint useProgram();

    void printGLString(const char *name, GLenum s);

    void checkGlError(const char *op);


    bool isProgramChanged = true;

    void delete_program(GLuint &program);

    GLuint m_program = 0;

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

    EglCore *m_EglCore = nullptr;
    WindowSurface *m_WindowSurface = nullptr;

    ANativeWindow *m_ANWindow = nullptr;

    VideoEncoderCore *m_VideoEncoderCore = nullptr;

    TextureMovieEncoder2 *m_TextureMovieEncoder2 = nullptr;
    WindowSurface *m_InputWindowSurface = nullptr;

    size_t offX;
    size_t offY;
    size_t off_right;
    size_t off_bottom;

};