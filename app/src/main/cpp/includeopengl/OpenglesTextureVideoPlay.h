
#ifndef ANDROIDLEARNOPENGL_OPENGLESFOUNDATION_H
#define ANDROIDLEARNOPENGL_OPENGLESFOUNDATION_H
#endif //ANDROIDLEARNOPENGL_OPENGLESFOUNDATION_H

#include <stb_image.h>
#include "OpenGLCamera3D.h"
#include "OpenGLShader.h"

using namespace std;
using namespace glm;


const float VIDEO_PLAY_TEXTURE[] = {
        0.5f,0.5f, // 右下
        0.5f,0.0f, // 右上
        0.0f,0.5f, // 左下
        0.0f,0.0f // 左上左上
};

const float VIDEO_PLAY_VERTEX[] = {
         // positions             // color
        0.5f,-0.5f, // 右下
        0.5f,0.5f, // 右上
        -0.5f,-0.5f, // 左下
        -0.5f,0.5f // 左上
};

class OpenglesTextureVideoPlay {

private:

    int screenW, screenH;
    unsigned char *data1;

    unsigned int texture;
    int width1, height1, nrChannels1;
    GLuint program;
    GLuint aTexCoord;
    string colorVertexCode;
    string colorFragmentCode;

    // camera
    OpenGLCamera3D mCamera;
    OpenGLShader *openGlShader;

public:

    OpenglesTextureVideoPlay();

    ~OpenglesTextureVideoPlay();

    int initGraphics();

    void setScreenWH(int w, int h);

    void renderFrame(const float mtx[]);

    void setVideoTexture(int renderTexture);

    bool setSharderPath(const char *vertexPath, const char *fragmentPath);
    void setSharderPic(const char *picPath);


    void printGLString(const char *name, GLenum s);

    void checkGlError(const char *op);

    int loadTexture(unsigned char *data, int width, int height);
};

