//  Author : wangyongyao https://github.com/wangyongyao1989
// Created by MMM on 2024/11/6.
//

#ifndef MYYFFMPEG_GLFboDrawTextVideoRender_H
#define MYYFFMPEG_GLFboDrawTextVideoRender_H

#ifndef MYYFFMPEG_GLFboDrawTextVideoRender_H
#define MYYFFMPEG_GLFboDrawTextVideoRender_H

#endif //MYYFFMPEG_GLFboDrawTextVideoRender_H

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
#include <stb_image.h>

#include <map>
// FreeType
#include "../includeopengl/freetype_2_9_1/ft2build.h"
#include "../includeopengl/freetype_2_9_1/freetype/ftglyph.h"
#include FT_FREETYPE_H


/// Holds all state information relevant to a character as loaded using FreeType
struct FboCharacter {
    GLuint TextureID;   // ID handle of the glyph texture
    glm::ivec2 Size;    // Size of glyph
    glm::ivec2 Bearing;  // Offset from baseline to left/top of glyph
    GLuint Advance;    // Horizontal offset to advance to next glyph
};


enum {
    MSG_FBO_SurfaceCreated,
    MSG_FBO_SurfaceChanged,
    MSG_FBO_DrawFrame,
    MSG_FBO_SurfaceDestroyed,
};


struct fbo_video_frame {
    size_t width;
    size_t height;
    size_t stride_y;
    size_t stride_uv;
    uint8_t *y;
    uint8_t *u;
    uint8_t *v;
};

// Vertices for a full screen quad.
static const float EGLFboTextVerticek[12] = {
        -1.f, 1.f, 0,
        -1.f, -1.f, 0,
        1.f, 1.f, 0,
        1.f, -1.f, 0
};

static const float EGLFboTextVerticek1[12] = {
        0.3f, -0.3f, 0,
        0.3f, -0.8f, 0,
        0.8f, -0.3f, 0,
        0.8f, -0.8f, 0
};

// Texture coordinates for mapping entire texture.
static const float EGLFboTextTextureCoord[12] = {
        0, 0, 0,
        0, 1, 0,
        1, 0, 0,
        1, 1, 0,
};

static const float EGLFboPicTextureCoord[12] = {
        0, 0, 0,
        0, 1, 0,
        1, 0, 0,
        1, 1, 0,
};

//const float quadVertices[] = {
//        // positions   // texCoords
//        -1.0f, 1.0f, 0.0f,
//        -1.0f, -1.0f, 0.0f,
//        1.0f, -1.0f, 0.0f,
//
//        -1.0f, 1.0f, 0.0f,
//        1.0f, -1.0f, 0.0f,
//        1.0f, 1.0f, 0.0F
//};
//
//const float quadTextureCoord[] = {
//        // positions   // texCoords
//        0.0f, 1.0f, 0,
//        0.0f, 0.0f, 0,
//        1.0f, 0.0f, 0,
//
//        0.0f, 1.0f, 0,
//        1.0f, 0.0f, 0,
//        1.0f, 1.0f, 0
//};


// vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
const float quadVertices[] = {
        // positions   // texCoords
        -1.0f, 1.0f, 0.0f, 1.0f,
        -1.0f, -1.0f, 0.0f, 0.0f,
        1.0f, -1.0f, 1.0f, 0.0f,

        -1.0f, 1.0f, 0.0f, 1.0f,
        1.0f, -1.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 1.0f, 1.0f
};

static const size_t BIT_RATE_FBO = 4000000;   // 4Mbps
static const size_t VIDEO_WIDTH_FBO = 1280;
static const size_t VIDEO_HEIGHT_FBO = 720;

class GLFboDrawTextVideoRender : public Looper {

public:
    GLFboDrawTextVideoRender();

    ~GLFboDrawTextVideoRender();

    void surfaceCreated(ANativeWindow *window, AAssetManager *assetManager);

    void surfaceChanged(size_t width, size_t height);

    void render();

    void updateFrame(const fbo_video_frame &frame);

    void release();

    void draw(uint8_t *buffer, size_t length, size_t width, size_t height, float rotation);

    void setParameters(uint32_t params);

    uint32_t getParameters();

    bool
    setSharderPath(const char *vertexPath, const char *fragmentPath, const char *fragmentPath1);

    bool
    setTextSharderPath(const char *vertexPath, const char *fragmentPath, const char *freeTypePath);

    bool setScreenSharderPath(const char *vertexPath, const char *fragmentPath);

    bool setPicTextPath(const char *picPath);

    bool setSharderStringPath(string vertexPath, string fragmentPath);

    void startEncoder(const char *recordPath);

    void stopEncoder();

private:

    void handleMessage(LooperMessage *msg);

    void OnSurfaceCreated();

    void OnSurfaceChanged(int w, int h);

    void OnDrawFrame();

    void OnSurfaceDestroyed();

    bool createYUVTextures();

    void creatPicTexture();     //创建图片纹理

    void bindTextVertexData();

    void RenderText(std::string text, GLfloat x, GLfloat y, GLfloat scale,
                    glm::vec3 color, glm::vec2 viewport);

    void LoadFacesByASCII(const char *path);

    bool updateYUVTextures();

    void bindPicTexture();

    void deleteYUVTextures();

    void deletePicTextures();

    int createYUVProgram();

    int createPicProgram();

    int createTextProgram();

    void createPostProcessingProgram();

    void bindPostProcessingVertexData();

    GLuint useYUVProgram();

    void usePicProgram();


    void printGLString(const char *name, GLenum s);

    void checkGlError(const char *op);


    void delete_program(GLuint &program);

    GLuint m_yuv_program = 0;
    GLuint m_pic_program = 0;
    GLuint m_text_program = 0;

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

    GLuint m_yuv_vertexPos = 0;
    GLuint m_pic_vertexPos = 0;
    GLuint m_fbo_vertexPos = 0;

    GLuint m_yuv_textureCoordLoc = 0;
    GLuint m_pic_textureCoordLoc = 0;
    GLuint m_fbo_textureCoordLoc = 0;

    GLint m_textureYLoc = 0;
    GLint m_textureULoc = 0;
    GLint m_textureVLoc = 0;
    GLuint m_texturePicLoc = 0;
    GLuint m_textureFboLoc = 0;

    size_t m_width = 0;
    size_t m_height = 0;
    size_t m_backingWidth = 0;
    size_t m_backingHeight = 0;
    uint32_t m_params = 0;
    float m_rotation = 0;

    bool isDirty;

    OpenGLShader *yuvGLShader = nullptr;
    OpenGLShader *picGLShader = nullptr;
    OpenGLShader *textGLShader = nullptr;
    OpenGLShader *screenShader = nullptr;


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

    int picChannels;
    int picWidth, picHeight;
    unsigned char *picData;

    map<GLchar, FboCharacter> Characters;
    GLuint VAO = GL_NONE;
    GLuint VBO = GL_NONE;

    string freeTypePathString;

    unsigned int quadVAO;
    unsigned int quadVBO;

    unsigned int framebuffer;
    unsigned int rbo;
    unsigned int textureColorbuffer;

    GLuint screenProgram;


};


#endif //MYYFFMPEG_GLFboDrawTextVideoRender_H
