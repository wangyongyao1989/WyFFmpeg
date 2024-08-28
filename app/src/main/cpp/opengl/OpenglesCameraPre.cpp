
#include "../includeopengl/OpenglesCameraPre.h"
#include <iostream>

bool OpenglesCameraPre::initGraphics() {
    program = camerPreShader->createProgram();
    if (!program) {
        LOGE("Could not create shaderId.");
        return false;
    }
    GLuint lightingPositionHandle = glGetAttribLocation(program, "gl_Position");
    checkGlError("program glGetAttribLocation");

    // 1. 设置顶点属性指针
    // position attribute
    GLuint aPos = glGetAttribLocation(program, "aPos");
    LOGI("aPos = %d\n", aPos);
    // 启用顶点数据
    glEnableVertexAttribArray(aPos);
    glVertexAttribPointer(aPos, 2, GL_FLOAT, GL_FALSE, 0, CAMERA_PRE_VERTEX);

    GLuint aTexCoord = glGetAttribLocation(program, "aTexCoord");
    // 纹理坐标
    glEnableVertexAttribArray(aTexCoord);
    glVertexAttribPointer(aTexCoord, 2, GL_FLOAT, GL_FALSE, 0, CAMERA_PRE_TEXTURE);

    // load and create a texture
    if (data1) {
        texture = loadTexture(data1, width1, height1);
    }

    return true;
}

void OpenglesCameraPre::renderFrame() {
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // also clear the depth buffer now!

    // bind Texture
    glActiveTexture(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture);

    glUseProgram(program);
    checkGlError("glUseProgram");
    // 4个顶点绘制两个三角形组成矩形
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    camerPreShader->use();

}

void OpenglesCameraPre::setVideoTexture(int renderTexture) {
//    texture = renderTexture;
}

void OpenglesCameraPre::setScreenWH(int w, int h) {
    screenW = w;
    screenH = h;
    glViewport(0, 0, w, h);
    checkGlError("glViewport");
    LOGI("glViewport successed!");

    //清屏
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    checkGlError("glClear");
}

bool OpenglesCameraPre::setSharderPath(const char *vertexPath, const char *fragmentPath) {
    camerPreShader->getSharderPath(vertexPath, fragmentPath);
    return 0;
}

void OpenglesCameraPre::setSharderPic(const char *picPath) {
    LOGI("setPicPath pic==%s", picPath);
    data1 = stbi_load(picPath, &width1, &height1, &nrChannels1, 0);
}


OpenglesCameraPre::OpenglesCameraPre() {
    camerPreShader = new OpenGLShader();
}

OpenglesCameraPre::~OpenglesCameraPre() {
    texture = 0;
    data1 = 0;
    //析构函数中释放资源
    camerPreShader = nullptr;


    colorVertexCode.clear();
    colorFragmentCode.clear();
}

void OpenglesCameraPre::printGLString(const char *name, GLenum s) {
    const char *v = (const char *) glGetString(s);
    LOGI("OpenGL %s = %s\n", name, v);
}

void OpenglesCameraPre::checkGlError(const char *op) {
    for (GLint error = glGetError(); error; error = glGetError()) {
        LOGI("after %s() glError (0x%x)\n", op, error);
    }
}

/**
 * 加载图片纹理
 * @param path
 * @return
 */
int OpenglesCameraPre::loadTexture(unsigned char *data, int width, int height) {
    unsigned int textureID;
    glGenTextures(1, &textureID);
    GLenum format;
    if (nrChannels1 == 1) {
        format = GL_RED;
    } else if (nrChannels1 == 3) {
        format = GL_RGB;
    } else if (nrChannels1 == 4) {
        format = GL_RGBA;
    }
    LOGI("loadTexture format =%d", format);
    if (data) {
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        stbi_image_free(data);
    } else {
        checkGlError("Texture failed to load at path: ");
        stbi_image_free(data);
    }

    return textureID;
}

