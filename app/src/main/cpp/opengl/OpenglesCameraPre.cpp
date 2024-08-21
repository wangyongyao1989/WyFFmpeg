
#include "../includeopengl/OpenglesCameraPre.h"
#include <iostream>

bool OpenglesCameraPre::initGraphics() {
    GLuint lightingProgram = camerPreShader->createProgram();
    if (!lightingProgram) {
        LOGE("Could not create shaderId.");
        return false;
    }
    GLuint lightingPositionHandle = glGetAttribLocation(lightingProgram, "gl_Position");
    checkGlError("lightingProgram glGetAttribLocation");

    LOGI("load VAO VBO Data");

    //绑定VAO
    glBindVertexArray(VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    //把顶点数组复制到缓冲中供OpenGL使用
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    //绑定EBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // 1. 设置顶点属性指针
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(0);


    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // load and create a texture
    LOGI("load and create a texture!");

    return true;
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

void OpenglesCameraPre::renderFrame() {
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // also clear the depth buffer now!

    //开启深度测试
//    glEnable(GL_DEPTH_TEST);

    camerPreShader->use();
    glBindVertexArray(VAO);

    double timeValue = clock() * 10 / CLOCKS_PER_SEC;
    float greenValue = static_cast<float>(sin(timeValue) / 3.0 + 0.5);


    camerPreShader->setVec4("ourColor", 0.0f, greenValue, 0.0f, 1.0f);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    checkGlError("glDrawArrays");
}

bool OpenglesCameraPre::setSharderPath(const char *vertexPath, const char *fragmentPath) {
    camerPreShader->getSharderPath(vertexPath, fragmentPath);
    return 0;
}


OpenglesCameraPre::OpenglesCameraPre() {
    camerPreShader = new OpenGLShader();
}

OpenglesCameraPre::~OpenglesCameraPre() {
    //析构函数中释放资源
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
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
 * 加载纹理
 * @param path
 * @return
 */
int OpenglesCameraPre::loadTexture(unsigned char *data, int width, int height, GLenum format) {
    unsigned int textureID;
    glGenTextures(1, &textureID);
//    LOGI("loadTexture format =%d", format);
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

