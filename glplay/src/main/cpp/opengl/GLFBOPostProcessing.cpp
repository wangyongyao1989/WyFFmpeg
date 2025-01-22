//  Author : wangyongyao https://github.com/wangyongyao1989
// Created by MMM on 2024/9/5.
//

#include <android/native_window.h>
#include <android/asset_manager.h>
#include <GLES3/gl3.h>
#include "GLFBOPostProcessing.h"

bool GLFBOPostProcessing::setupGraphics(int w, int h) {
    screenW = w;
    screenH = h;
    LOGI("setupGraphics(%d, %d)", w, h);
    GLuint fBOProgram = fBOShader->createProgram();
    if (!fBOProgram) {
        LOGE("Could not create fBOProgram shaderId.");
        return false;
    }


    glViewport(0, 0, w, h);
    checkGlError("glViewport");
    LOGI("glViewport successed!");

    //清屏
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    checkGlError("glClear");

    //开启深度测试
    glEnable(GL_DEPTH_TEST);

    // cube VAO
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);
    glBindVertexArray(cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(PostProcessingVertices), &PostProcessingVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                          (void *) (3 * sizeof(float)));
    glBindVertexArray(0);

    // plane VAO
    glGenVertexArrays(1, &planeVAO);
    glGenBuffers(1, &planeVBO);
    glBindVertexArray(planeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(PostProcessingPlaneVertices), &PostProcessingPlaneVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                          (void *) (3 * sizeof(float)));
    glBindVertexArray(0);

    // screen quad VAO
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(PostProcessingQuadVertices),
                 &PostProcessingQuadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
                          (void *) (2 * sizeof(float)));


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
        cubeTexture = loadTexture(data1, width1, height1, format);
    }

    if (nrChannels2 == 1) {
        format = GL_RED;
    } else if (nrChannels2 == 3) {
        format = GL_RGB;
    } else if (nrChannels2 == 4) {
        format = GL_RGBA;
    }
    if (data2) {
        floorTexture = loadTexture(data2, width2, height2, format);
    }


    // shader configuration
    // --------------------
    fBOShader->use();
    fBOShader->setInt("texture1", 0);

    createPostProcessingProgram();

    //1.首先要创建一个帧缓冲对象，并绑定它，这些都很直观
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    //2.接下来我们需要创建一个纹理图像，我们将它作为一个颜色附件附加到帧缓冲上。
    // 我们将纹理的维度设置为窗口的宽度和高度，并且不初始化它的数据
    glGenTextures(1, &textureColorbuffer);
    glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, screenW, screenH, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer,
                           0);

    //3.创建渲染缓冲对象
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, screenW,
                          screenH);
    //4.将渲染缓冲对象附加到帧缓冲的深度和模板附件上
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER,
                              rbo);
    //5.最后，我们希望检查帧缓冲是否是完整的，如果不是，我们将打印错误信息
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        LOGE("ERROR::FRAMEBUFFER:: Framebuffer is not complete!");
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return true;
}

void GLFBOPostProcessing::renderFrame() {
    if (m_filter != m_prevFilter) {
        m_prevFilter = m_filter;
        if (m_filter >= 0 && m_filter < m_fragmentStringPathes.size()) {
            delete_program(screenProgram);
            LOGI("render---m_filter：%d", m_filter);
            screenShader->getSharderStringPath(m_vertexStringPath, m_fragmentStringPathes.at(m_prevFilter));
            createPostProcessingProgram();
        }
    }

    //绑定到帧缓冲区，像往常一样绘制场景以着色纹理
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    //启用深度测试（禁用渲染屏幕空间四边形）
    glEnable(GL_DEPTH_TEST);

    // 确保清除帧缓冲区的内容
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    fBOShader->use();
    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view = mCamera.GetViewMatrix();
    glm::mat4 projection = glm::perspective(glm::radians(mCamera.Zoom),
                                            (float) screenW / (float) screenH, 0.1f, 100.0f);
    fBOShader->setMat4("view", view);
    fBOShader->setMat4("projection", projection);
    // cubes
    glBindVertexArray(cubeVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, cubeTexture);
    model = glm::translate(model, glm::vec3(-1.0f, 0.0f, -1.0f));
    fBOShader->setMat4("model", model);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(2.0f, 0.0f, 0.0f));
    fBOShader->setMat4("model", model);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    // floor
    glBindVertexArray(planeVAO);
    glBindTexture(GL_TEXTURE_2D, floorTexture);
    fBOShader->setMat4("model", glm::mat4(1.0f));
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);

    //现在绑定回默认帧缓冲区，并使用附加的帧缓冲区颜色纹理绘制一个四边形平面
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    //禁用深度测试，这样屏幕空间四边形就不会因为深度测试而被丢弃。
    glDisable(GL_DEPTH_TEST);

    // 清除所有相关缓冲区
    // 将透明颜色设置为白色（实际上并没有必要，因为我们无论如何都看不到四边形后面）
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    screenShader->use();
    glBindVertexArray(quadVAO);
    //使用颜色附着纹理作为四边形平面的纹理
    glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    checkGlError("glDrawArrays");
}

bool GLFBOPostProcessing::setSharderPath(const char *vertexPath, const char *fragmentPath) {
    fBOShader->getSharderPath(vertexPath, fragmentPath);
    return 0;
}



bool
GLFBOPostProcessing::setSharderScreenPathes(string vertexScreenPath, vector<string> fragmentScreenPathes) {
    screenShader->getSharderStringPath(vertexScreenPath, fragmentScreenPathes.front());
    m_vertexStringPath = vertexScreenPath;
    m_fragmentStringPathes = fragmentScreenPathes;
    return 0;
}

void GLFBOPostProcessing::setPicPath(const char *pic1, const char *pic2) {
    LOGI("setPicPath pic1==%s", pic1);
    LOGI("setPicPath pic2==%s", pic2);
    data1 = stbi_load(pic1, &width1, &height1, &nrChannels1, 0);
    data2 = stbi_load(pic2, &width2, &height2, &nrChannels2, 0);
}

void GLFBOPostProcessing::setMoveXY(float dx, float dy, int actionMode) {
    LOGI("setMoveXY dx:%f,dy:%f,actionMode:%d", dy, dy, actionMode);
    float xoffset = dx - lastX;
    float yoffset = lastY - dy; // reversed since y-coordinates go from bottom to top
    lastX = dx;
    lastY = dy;
    mActionMode = actionMode;
    mCamera.ProcessXYMovement(xoffset, yoffset);
}

void GLFBOPostProcessing::setOnScale(float scaleFactor, float focusX, float focusY, int actionMode) {
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


GLFBOPostProcessing::GLFBOPostProcessing() {
    fBOShader = new OpenGLShader();
    screenShader = new OpenGLShader;
}

GLFBOPostProcessing::~GLFBOPostProcessing() {
    cubeTexture = 0;
    floorTexture = 0;
    textureColorbuffer = 0;

    //析构函数中释放资源
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteVertexArrays(1, &planeVAO);
    glDeleteVertexArrays(1, &quadVAO);

    glDeleteBuffers(1, &cubeVBO);
    glDeleteBuffers(1, &planeVBO);
    glDeleteBuffers(1, &quadVBO);

    glDeleteRenderbuffers(1, &rbo);
    glDeleteFramebuffers(1, &framebuffer);


    fBOShader = nullptr;
    screenShader = nullptr;

    screenProgram = 0;
    m_filter = 0;

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

void GLFBOPostProcessing::printGLString(const char *name, GLenum s) {
    const char *v = (const char *) glGetString(s);
    LOGI("OpenGL %s = %s\n", name, v);
}

void GLFBOPostProcessing::checkGlError(const char *op) {
    for (GLint error = glGetError(); error; error = glGetError()) {
        LOGI("after %s() glError (0x%x)\n", op, error);
    }
}

/**
 * 加载纹理
 * @param path
 * @return
 */
int GLFBOPostProcessing::loadTexture(unsigned char *data, int width, int height, GLenum format) {
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

void GLFBOPostProcessing::setParameters(uint32_t i) {
    m_filter = i;
    LOGI("setParameters---m_filter：%d", m_filter);

}

jint GLFBOPostProcessing::getParameters() {
    return m_filter;
}

void GLFBOPostProcessing::createPostProcessingProgram() {

    screenProgram = screenShader->createProgram();
    if (!screenProgram) {
        LOGE("Could not create screenProgram shaderId.");
        return ;
    }
    screenShader->use();
    screenShader->setInt("screenTexture", 0);
}

void GLFBOPostProcessing::delete_program(GLuint &program) {
    if (program) {
        glUseProgram(0);
        glDeleteProgram(program);
        program = 0;
    }
}
