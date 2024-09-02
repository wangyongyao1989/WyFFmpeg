
#ifndef ANDROIDLEARNOPENGL_OPENGLESFOUNDATION_H
#define ANDROIDLEARNOPENGL_OPENGLESFOUNDATION_H
#endif //ANDROIDLEARNOPENGL_OPENGLESFOUNDATION_H

#include <stb_image.h>
#include <android/native_window.h>
#include <android/asset_manager.h>
#include "OpenGLCamera3D.h"
#include "OpenGLShader.h"
#include "GLCommonUtils.h"

using namespace std;
using namespace glm;


const float VIDEO_PLAY_TEXTURE[] = {
        0.5f, 0.5f, // 右下
        0.5f, 0.0f, // 右上
        0.0f, 0.5f, // 左下
        0.0f, 0.0f // 左上左上
};

const float VIDEO_PLAY_VERTEX[] = {
        // positions             // color
        0.5f, -0.5f, // 右下
        0.5f, 0.5f, // 右上
        -0.5f, -0.5f, // 左下
        -0.5f, 0.5f // 左上
};

// Vertices for a full screen quad.
const float kVertices[8] = {
        -1.f, 1.f,
        -1.f, -1.f,
        1.f, 1.f,
        1.f, -1.f
};

// Texture coordinates for mapping entire texture.
const float kTextureCoords[8] = {
        0, 0,
        0, 1,
        1, 0,
        1, 1
};

struct video_frame {
    size_t width;
    size_t height;
    size_t stride_y;
    size_t stride_uv;
    uint8_t *y;
    uint8_t *u;
    uint8_t *v;
};

class OpenglesTextureVideoPlay {

private:

    int screenW, screenH;
    GLuint program = 0;
    GLuint aTexCoord;
    string colorVertexCode;
    string colorFragmentCode;

    // camera
    OpenGLCamera3D mCamera;
    OpenGLShader *openGlShader;

    int createProgram();

    bool createTextures();

    bool updateTextures();

    void deleteTextures();

    GLuint useProgram();

    void updateFrame(const video_frame &frame);

    bool isDirty;
    bool isProgramChanged;

    size_t m_width;
    size_t m_height;
    size_t m_backingWidth;
    size_t m_backingHeight;

    __unused  size_t m_length;
    float m_rotation;

    std::unique_ptr<uint8_t[]> m_pDataY;
//    uint8_t m_pDataY;
    uint8_t *m_pDataU;
    uint8_t *m_pDataV;

    size_t m_sizeY;
    size_t m_sizeU;
    size_t m_sizeV;

    GLuint m_textureIdY;
    GLuint m_textureIdU;
    GLuint m_textureIdV;

    GLuint m_vertexPos;
    GLuint m_textureLoc;
    GLint m_textureYLoc;
    GLint m_textureULoc;
    GLint m_textureVLoc;
//    GLint m_textureSize;
//    GLint m_uniformProjection;
//    GLint m_uniformRotation;
//    GLint m_uniformScale;

public:

    OpenglesTextureVideoPlay();

    ~OpenglesTextureVideoPlay();

    void setScreenWH(int w, int h);

    void init(ANativeWindow *window, AAssetManager *assetManager);

    void render();

    void draw(uint8_t *buffer, size_t length, size_t width, size_t height, float rotation);

    bool setSharderPath(const char *vertexPath, const char *fragmentPath);

    void printGLString(const char *name, GLenum s);

    void checkGlError(const char *op);

};

