
#ifndef ANDROIDLEARNOPENGL_OPENGLESFOUNDATION_H
#define ANDROIDLEARNOPENGL_OPENGLESFOUNDATION_H
#endif //ANDROIDLEARNOPENGL_OPENGLESFOUNDATION_H

#include <stb_image.h>
#include "OpenGLCamera3D.h"
#include "OpenGLShader.h"

using namespace std;
using namespace glm;

const float CAMERA_PRE_VERTEX[] = {
        -1.0f, -1.0f,
        1.0f, -1.0f,
        -1.0f, 1.0f,
        1.0f, 1.0f
};

const float CAMERA_PRE_TEXTURE[] = {
        0.0f, 0.0f,
        1.0f, 0.0f,
        0.0f, 1.0f,
        1.0f, 1.0f
};

const float vertices[] = {
    0.5f, 0.5f, 0.0f,  // top right
    0.5f, -0.5f, 0.0f,  // bottom right
    -0.5f, -0.5f, 0.0f,  // bottom left
    -0.5f, 0.5f, 0.0f   // top left
};

const unsigned int indices[] = {
    // 注意索引从0开始!
    // 此例的索引(0,1,2,3)就是顶点数组vertices的下标，
    // 这样可以由下标代表顶点组合成矩形
    0, 1, 3, // 第一个三角形
    1, 2, 3  // 第二个三角形
};

class OpenglesCameraPre {

private:
    int screenW, screenH;
    float lastX, lastY;
    int mActionMode;

    unsigned int VBO, VAO;
    unsigned int EBO;
    int vertexColorLocation;

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

    bool setSharderPath(const char *vertexPath, const char *fragmentPath);


    void printGLString(const char *name, GLenum s);

    void checkGlError(const char *op);

    int loadTexture(unsigned char *data, int width, int height, GLenum format);
};

