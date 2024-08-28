
#ifndef ANDROIDLEARNOPENGL_OPENGLESFOUNDATION_H
#define ANDROIDLEARNOPENGL_OPENGLESFOUNDATION_H
#endif //ANDROIDLEARNOPENGL_OPENGLESFOUNDATION_H

#include <stb_image.h>
#include "OpenGLCamera3D.h"
#include "OpenGLShader.h"

using namespace std;
using namespace glm;


const float CAMERA_PRE_TEXTURE[] = {
        0.5f,0.5f, // 右下
        0.5f,0.0f, // 右上
        0.0f,0.5f, // 左下
        0.0f,0.0f // 左上左上
};

const float CAMERA_PRE_VERTEX[] = {
         // positions             // color
        0.5f,-0.5f, // 右下
        0.5f,0.5f, // 右上
        -0.5f,-0.5f, // 左下
        -0.5f,0.5f // 左上
};

const unsigned int indices[] = {
    // 注意索引从0开始!
    // 此例的索引(0,1,2,3)就是顶点数组vertices的下标，
    // 这样可以由下标代表顶点组合成矩形
    0, 1, 3, // 第一个三角形
    1, 2, 3  // 第二个三角形
};

const float textureDemoVertices[] = {
        // positions       // colors        // texture coords
        0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top right
        0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom right
        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom left
        -0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f  // top left
};

static unsigned int textureDemoIndices[] = {
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
};

class OpenglesCameraPre {

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
    OpenGLShader *camerPreShader;

public:

    OpenglesCameraPre();

    ~OpenglesCameraPre();

    bool initGraphics();

    void setScreenWH(int w, int h);

    void renderFrame();

    void setVideoTexture(int renderTexture);

    bool setSharderPath(const char *vertexPath, const char *fragmentPath);
    void setSharderPic(const char *picPath);


    void printGLString(const char *name, GLenum s);

    void checkGlError(const char *op);

    int loadTexture(unsigned char *data, int width, int height);
};

