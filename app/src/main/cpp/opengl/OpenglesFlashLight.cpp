
#include "../includeopengl/OpenglesFlashLight.h"
#include <iostream>

bool OpenglesFlashLight::setupGraphics(int w, int h) {
    screenW = w;
    screenH = h;
    LOGI("setupGraphics(%d, %d)", w, h);
    GLuint lightingProgram = lightColorShader->createProgram();
    if (!lightingProgram) {
        LOGE("Could not create shaderId.");
        return false;
    }
    GLuint lightingPositionHandle = glGetAttribLocation(lightingProgram, "gl_Position");
    checkGlError("lightingProgram glGetAttribLocation");

    GLuint lightCubeProgram = lightCubeShader->createProgram();
    if (!lightCubeProgram) {
        LOGE("Could not create shaderId.");
        return false;
    }
    GLuint lightCubePositionHandle = glGetAttribLocation(lightCubeProgram, "gl_Position");
    checkGlError("lightCubeProgram glGetAttribLocation");
    glViewport(0, 0, w, h);
    checkGlError("glViewport");
    LOGI("glViewport successed!");

    //清屏
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    checkGlError("glClear");

    //绑定物体立方体数据
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(FlashLightVertices), FlashLightVertices, GL_STATIC_DRAW);
    glBindVertexArray(cubeVAO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                          (void *) (3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                          (void *) (6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    //绑定灯光立方体数据
    glGenVertexArrays(1, &lightCubeVAO);
    glBindVertexArray(lightCubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(0);

    // load and create a texture
    LOGI("load and create a texture!");
    GLenum format;
    if (nrChannels1 == 1) {
        format = GL_RED;
    } else if (nrChannels1 == 3) {
        format = GL_RGB;
    } else if (nrChannels1 == 4) {
        format = GL_RGBA;
    }
//    LOGI("texture1 format==%d", format);
    if (data1) {
        diffuseMapTexture = loadTexture(data1, width1, height1, format);
    }

    if (nrChannels2 == 1) {
        format = GL_RED;
    } else if (nrChannels2 == 3) {
        format = GL_RGB;
    } else if (nrChannels2 == 4) {
        format = GL_RGBA;
    }
    if (data2) {
        specularMapTexture = loadTexture(data2, width2, height2, format);
    }

    lightColorShader->use();
    lightColorShader->setInt("material.diffuse", 0);
    lightColorShader->setInt("material.specular", 1);

    return true;
}

void OpenglesFlashLight::renderFrame() {
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // also clear the depth buffer now!

    //开启深度测试
    glEnable(GL_DEPTH_TEST);
    // be sure to activate shader when setting uniforms/drawing objects
    lightColorShader->use();

    lightColorShader->setVec3("light.position", mCamera.Position);
    lightColorShader->setVec3("light.direction", mCamera.Front);
    lightColorShader->setFloat("light.cutOff", glm::cos(glm::radians(12.5f)));
    lightColorShader->setVec3("viewPos", mCamera.Position);

    // light properties
    lightColorShader->setVec3("light.ambient", 0.5f, 0.5f, 0.5f);
    lightColorShader->setVec3("light.diffuse", 1.0f, 1.0f, 1.0f);
    lightColorShader->setVec3("light.specular", 0.5f, 0.5f, 0.5f);
    lightColorShader->setFloat("light.constant", 1.0f);
    lightColorShader->setFloat("light.linear", 0.09f);
    lightColorShader->setFloat("light.quadratic", 0.032f);


    // material properties
    lightColorShader->setVec3("material.specular", 0.5f, 0.5f, 0.5f);
    lightColorShader->setFloat("material.shininess", 64.0f);

    // view/projection transformations
    glm::mat4 projection = glm::perspective(glm::radians(mCamera.Zoom),
                                            (float) screenW / (float) screenH, 0.1f, 100.0f);
    vec3 cameraMove(0.0f, 0.0f, 3.0f);
    mCamera.Position = cameraMove;
    glm::mat4 view = mCamera.GetViewMatrix();
    lightColorShader->setMat4("projection", projection);
    lightColorShader->setMat4("view", view);

    // world transformation
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::rotate(model, glm::radians(45.0f), FlashLightLightPos);
    lightColorShader->setMat4("model", model);

    // bind diffuse map
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, diffuseMapTexture);

    // bind specular map
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, specularMapTexture);

/*    // render the cube
    glBindVertexArray(cubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);*/

    // render containers
    glBindVertexArray(cubeVAO);
    for (unsigned int i = 0; i < 10; i++) {
        // calculate the model matrix for each object and pass it to shader before drawing
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, FlashLightCubePositions[i]);
        float angle = 20.0f * i;
        model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
        lightColorShader->setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }

    checkGlError("glDrawArrays");
}

bool OpenglesFlashLight::setSharderPath(const char *vertexPath, const char *fragmentPath) {
    lightColorShader->getSharderPath(vertexPath, fragmentPath);
    return 0;
}

bool OpenglesFlashLight::setColorSharderPath(const char *vertexPath, const char *fragmentPath) {
    lightCubeShader->getSharderPath(vertexPath, fragmentPath);
    return false;
}

void OpenglesFlashLight::setPicPath(const char *pic1, const char *pic2) {
    LOGI("setPicPath pic1==%s", pic1);
    LOGI("setPicPath pic2==%s", pic2);
    data1 = stbi_load(pic1, &width1, &height1, &nrChannels1, 0);
    data2 = stbi_load(pic2, &width2, &height2, &nrChannels2, 0);
}

void OpenglesFlashLight::setMoveXY(float dx, float dy, int actionMode) {
    LOGI("setMoveXY dx:%f,dy:%f,actionMode:%d", dy, dy, actionMode);
    float xoffset = dx - lastX;
    float yoffset = lastY - dy; // reversed since y-coordinates go from bottom to top
    lastX = dx;
    lastY = dy;
    mActionMode = actionMode;
    mCamera.ProcessXYMovement(xoffset, yoffset);
}

void OpenglesFlashLight::setOnScale(float scaleFactor, float focusX, float focusY, int actionMode) {
//    LOGI("setOnScale scaleFactor:%f,focusX:%f,focusY:%f,actionMode:%d", scaleFactor, focusX, focusY,
//         actionMode);
//    LOGI("setOnScale scaleFactor:%f", scaleFactor);
    float scale;
    if (actionMode == 1 || actionMode == 3) {
        scale = 45.0f;
    } else {
        if (scaleFactor > 1) {
            scale = (scaleFactor - 1) * 1000 + 45;
        } else {
            scale = 50 - (1 - scaleFactor) * 1000;
        }
    }
    LOGI("setOnScale scale:%f", scale);
    mCamera.ProcessScroll(scale);
}


OpenglesFlashLight::OpenglesFlashLight() {
    lightColorShader = new OpenGLShader();
    lightCubeShader = new OpenGLShader();
}

OpenglesFlashLight::~OpenglesFlashLight() {
    diffuseMapTexture = 0;
    specularMapTexture = 0;
    //析构函数中释放资源
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    lightCubeShader = nullptr;
    lightColorShader = nullptr;

    if (data1) {
        stbi_image_free(data1);
        data1 = nullptr;
    }

    if (data2) {
        stbi_image_free(data2);
        data2 = nullptr;
    }

    colorVertexCode.clear();
    colorFragmentCode.clear();
}

void OpenglesFlashLight::printGLString(const char *name, GLenum s) {
    const char *v = (const char *) glGetString(s);
    LOGI("OpenGL %s = %s\n", name, v);
}

void OpenglesFlashLight::checkGlError(const char *op) {
    for (GLint error = glGetError(); error; error = glGetError()) {
        LOGI("after %s() glError (0x%x)\n", op, error);
    }
}

/**
 * 加载纹理
 * @param path
 * @return
 */
int OpenglesFlashLight::loadTexture(unsigned char *data, int width, int height, GLenum format) {
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

