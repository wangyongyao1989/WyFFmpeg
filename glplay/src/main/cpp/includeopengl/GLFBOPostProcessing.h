//  Author : wangyongyao https://github.com/wangyongyao1989


#ifndef MYYFFMPEG_GLFBOPostProcessing_H
#define MYYFFMPEG_GLFBOPostProcessing_H

#endif //MYYFFMPEG_GLFBOPostProcessing_H

#include <cstdint>
#include <memory>
#include <android/native_window.h>
#include <android/asset_manager.h>
#include <EGL/egl.h>
#include <GLES3/gl3.h>
#include <vector>
#include <stb_image.h>
#include <jni.h>
#include "EglCore.h"
#include "WindowSurface.h"
#include "Looper.h"
#include "VideoEncoderCore.h"
#include "TextureMovieEncoder2.h"


#include "OpenGLCamera3D.h"
#include "OpenGLShader.h"


using namespace std;
using namespace glm;

enum {
    MSG_PS_SurfaceCreated,
    MSG_PS_SurfaceChanged,
    MSG_PS_DrawFrame,
    MSG_PS_SurfaceDestroyed,
};

struct ps_video_frame {
    size_t width;
    size_t height;
    size_t stride_y;
    size_t stride_uv;
    uint8_t *y;
    uint8_t *u;
    uint8_t *v;
};

// Vertices for a full screen quad.
const float FboPsVerticek[12] = {
        -1.f, 1.f, 0,
        -1.f, -1.f, 0,
        1.f, 1.f, 0,
        1.f, -1.f, 0
};

// Texture coordinates for mapping entire texture.
const float FboPsTextureCoord[12] = {
        0, 0, 0,
        0, 1, 0,
        1, 0, 0,
        1, 1, 0,
};


const float PostProcessingVertices[] = {
        // positions          // texture Coords
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,

        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
        -0.5f, 0.5f, 0.5f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,

        -0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
        -0.5f, 0.5f, 0.5f, 1.0f, 0.0f,

        0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 1.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,

        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
        -0.5f, 0.5f, 0.5f, 0.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f
};

const float PostProcessingPlaneVertices[] = {
        // positions          // texture Coords (note we set these higher than 1
        // (together with GL_REPEAT as texture wrapping mode). this will cause the floor texture to repeat)
        5.0f, -0.5f, 5.0f, 2.0f, 0.0f,
        -5.0f, -0.5f, 5.0f, 0.0f, 0.0f,
        -5.0f, -0.5f, -5.0f, 0.0f, 2.0f,

        5.0f, -0.5f, 5.0f, 2.0f, 0.0f,
        -5.0f, -0.5f, -5.0f, 0.0f, 2.0f,
        5.0f, -0.5f, -5.0f, 2.0f, 2.0f
};

// vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
const float PostProcessingQuadVertices[] = {
        // positions   // texCoords
        -1.0f, 1.0f, 0.0f, 1.0f,
        -1.0f, -1.0f, 0.0f, 0.0f,
        1.0f, -1.0f, 1.0f, 0.0f,

        -1.0f, 1.0f, 0.0f, 1.0f,
        1.0f, -1.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 1.0f, 1.0f
};

const float PostProcessingQuadVertices1[] = {
        // positions
        -1.0f, 1.0f, 0.0f,
        -1.0f, -1.0f, 0.0f,
        1.0f, -1.0f, 0.0f,

        -1.0f, 1.0f, 0.0f,
        1.0f, -1.0f, 0.0f,
        1.0f, 1.0f, 0.0f,
};

const float PostProcessingQuadTextCoord[] = {
         // texCoords
        0.0f, 1.0f,0.0f,
        0.0f, 0.0f,0.0f,
        1.0f, 0.0f,0.0f,

        0.0f, 1.0f,0.0f,
        1.0f, 0.0f,0.0f,
        1.0f, 1.0f,0.0f
};

static const float EGLFboPSTextVerticek1[12] = {
        0.3f, -0.3f, 0,
        0.3f, -0.8f, 0,
        0.8f, -0.3f, 0,
        0.8f, -0.8f, 0
};

// Texture coordinates for mapping entire texture.
static const float EGLFboPSTextTextureCoord[12] = {
        0, 0, 0,
        0, 1, 0,
        1, 0, 0,
        1, 1, 0,
};


class GLFBOPostProcessing : public Looper {
private:
    unsigned int cubeTexture;
    unsigned int floorTexture;
    unsigned char *data1 = nullptr;
    unsigned char *data2 = nullptr;
    int screenW, screenH;
    int width1, height1, nrChannels1;
    int width2, height2, nrChannels2;

    unsigned char *picData = nullptr;
    int picWidth, picHeight, picChannels;
    unsigned int yaoTexture;

    unsigned int cubeVAO;
    unsigned int cubeVBO;

    unsigned int planeVAO;
    unsigned int planeVBO;

    unsigned int quadVAO;
    unsigned int quadVBO;

    unsigned int framebuffer;
    unsigned int rbo;
    unsigned int textureColorbuffer;


    string colorVertexCode;
    string colorFragmentCode;

    // camera
    OpenGLCamera3D mCamera;
    OpenGLShader *fBOShader;
    OpenGLShader *screenShader;
    OpenGLShader *picGLShader = nullptr;


    GLuint screenProgram;
    string m_vertexStringPath;
    vector<string> m_fragmentStringPathes;

    EglCore *m_EglCore = nullptr;
    WindowSurface *m_WindowSurface = nullptr;
    EGLSurface winsurface = nullptr;

    ANativeWindow *m_ANWindow = nullptr;

    void delete_program(GLuint &program);

public:

    GLFBOPostProcessing();

    ~GLFBOPostProcessing();

    bool setupGraphics(int w, int h);

    void renderFrame();

    bool setSharderPath(const char *vertexPath, const char *fragmentPath);

    bool setSharderScreenPathes(string vertexScreenPath, vector<string> fragmentScreenPathes);

    void setPicPath(const char *pic1, const char *pic2, const char *pic3);


    void printGLString(const char *name, GLenum s);

    void checkGlError(const char *op);

    int loadTexture(unsigned char *data, int width, int height, GLenum format);

    void createPostProcessingProgram();

    void setParameters(uint32_t i);

    jint getParameters();

    size_t m_filter = 0;

    size_t m_prevFilter = 0;

    void handleMessage(LooperMessage *msg);

    void OnSurfaceCreated();

    void surfaceCreated(ANativeWindow *window, AAssetManager *assetManager);

    void surfaceChanged(size_t width, size_t height);

    void render();

    void release();

    void draw(uint8_t *buffer, size_t length, size_t width, size_t height, float rotation);

    void updateFrame(const ps_video_frame &frame);

    size_t m_sizeY = 0;
    size_t m_sizeU = 0;
    size_t m_sizeV = 0;

    std::unique_ptr<uint8_t[]> m_pDataY = nullptr;

    uint8_t *m_pDataU = nullptr;
    uint8_t *m_pDataV = nullptr;

    __unused  size_t m_length = 0;

    size_t m_width = 0;
    size_t m_height = 0;
    bool isDirty;

    bool createYUVTextures();

    bool updateYUVTextures();

    int createYUVProgram();

    void deleteYUVTextures();

    GLuint useYUVProgram();

    OpenGLShader *yuvGLShader = nullptr;

    GLuint m_textureIdY = 0;
    GLuint m_textureIdU = 0;
    GLuint m_textureIdV = 0;

    GLuint m_yuv_program = 0;
    GLuint m_yuv_vertexPos = 0;
    GLint m_textureYLoc = 0;
    GLint m_textureULoc = 0;
    GLint m_textureVLoc = 0;
    GLuint m_yuv_textureCoordLoc = 0;

    bool setYUVSharderPath(const char *vertexPath, const char *fragmentPath);

    void bindPicTexture();

    void deletePicTextures();

    int createPicProgram();

    void creatPicTexture();

    void usePicProgram();

    bool setPicSharderPath(const char *vertexPath, const char *fragmentPath);

    void useFBOProgram();

    GLuint m_pic_program = 0;
    GLuint m_pic_vertexPos = 0;
    GLuint m_pic_textureCoordLoc = 0;
    GLuint m_texturePicLoc = 0;

    GLuint m_screen_vertexPos = 0;
    GLuint m_screen_textureCoordLoc = 0;
    GLuint m_textureScreenLoc = 0;
};