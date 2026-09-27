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
        0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,

        0.0f, 1.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        1.0f, 1.0f, 0.0f
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

public:

    GLFBOPostProcessing();

    ~GLFBOPostProcessing();

    bool setSharderScreenPathes(string vertexScreenPath, vector<string> fragmentScreenPathes);

    void setPicPath(const char *pic);

    bool setupGraphics(int w, int h);

    bool setPicSharderPath(const char *vertexPath, const char *fragmentPath);

    bool setYUVSharderPath(const char *vertexPath, const char *fragmentPath);

    void surfaceChanged(size_t width, size_t height);

    void surfaceCreated(ANativeWindow *window, AAssetManager *assetManager);

    void draw(uint8_t *buffer, size_t length, size_t width, size_t height, float rotation);


    void setParameters(uint32_t i);

    jint getParameters();

    void render();

    void release();


private:
    /**
     * 纹理单元分配表（同一 pass 内 sampler 与 glBindTexture 必须落在同一单元）：
     *   0 / 1 / 2 —— YUV 三通道（YUV pass 的 s_textureY/U/V）
     *   3         —— 水印图片（s_texturePic）
     *   4         —— FBO 后期处理结果（screenTexture）
     * 原先三者都依赖"上一次 glActiveTexture 停在哪个单元"，pass 之间互相覆盖绑定，
     * 造成 GL_INVALID_OPERATION(0x502) 与水印丢失。
     */
    static const GLint kPicTextureUnit = 3;
    static const GLint kScreenTextureUnit = 4;

    int screenW, screenH;
    unsigned int framebuffer;
    unsigned int rbo;
    unsigned int fboTexture;

    // camera
    OpenGLCamera3D mCamera;

    string m_vertexStringPath;
    vector<string> m_fragmentStringPathes;

    EglCore *m_EglCore = nullptr;
    WindowSurface *m_WindowSurface = nullptr;
    EGLSurface winsurface = nullptr;

    ANativeWindow *m_ANWindow = nullptr;

    void OnSurfaceCreated();

    void OnSurfaceDestroyed();

    void handleMessage(LooperMessage *msg);

    void renderFrame();

    //YUV
    size_t m_sizeY = 0;
    size_t m_sizeU = 0;
    size_t m_sizeV = 0;
    std::unique_ptr<uint8_t[]> m_pDataY = nullptr;
    uint8_t *m_pDataU = nullptr;
    uint8_t *m_pDataV = nullptr;
    __unused  size_t m_length = 0;
    size_t m_width = 0;
    size_t m_height = 0;
    bool isDirty = false;

    GLuint m_textureIdY = 0;
    GLuint m_textureIdU = 0;
    GLuint m_textureIdV = 0;

    GLuint m_yuv_program = 0;
    GLuint m_yuv_vertexPos = 0;
    GLint m_textureYLoc = 0;
    GLint m_textureULoc = 0;
    GLint m_textureVLoc = 0;
    GLuint m_yuv_textureCoordLoc = 0;
    OpenGLShader *yuvGLShader = nullptr;


    GLuint useYUVProgram();

    int createYUVProgram();

    void deleteYUVTextures();

    bool createYUVTextures();

    bool updateYUVTextures();

    void updateFrame(const ps_video_frame &frame);

    void delete_program(GLuint &program);

    //图片纹理
    unsigned char *picData = nullptr;
    int picWidth, picHeight, picChannels;
    OpenGLShader *picGLShader = nullptr;
    GLuint m_pic_program = 0;
    GLuint m_pic_vertexPos = 0;
    GLuint m_pic_textureCoordLoc = 0;
    // sampler uniform 的位置必须用 glGetUniformLocation 取，且类型用 GLint
    // （取不到时是 -1；若存成 GLuint 会变成 4294967295，传给 glUniform1i 只是静默失效）
    GLint m_texturePicLoc = -1;
    /** 图片纹理对象名。原先复用 m_texturePicLoc（uniform location），
     *  creatPicTexture 的 glGenTextures 会把 uniform 位置覆盖成纹理名，
     *  导致 glUniform1i(s_texturePic, <纹理名>) 采样到错误的纹理单元 */
    GLuint m_picTextureName = 0;

    void usePicProgram();

    void bindPicTexture();

    void deletePicTextures();


    //帧缓冲FBO
    size_t m_filter = 0;
    size_t m_prevFilter = 0;
    GLuint m_screen_vertexPos = 0;
    GLuint m_screen_textureCoordLoc = 0;
    GLint m_textureScreenLoc = -1;
    OpenGLShader *screenShader;
    GLuint screenProgram;
    void createFBOProgram();

    int createPicProgram();

    void creatPicTexture();

    void useFBOProgram();

    void creatFBOTexture();

    //打印
    void printGLString(const char *name, GLenum s);

    void checkGlError(const char *op);
};