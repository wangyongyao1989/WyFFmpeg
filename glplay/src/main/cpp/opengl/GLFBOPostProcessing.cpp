//  Author : wangyongyao https://github.com/wangyongyao1989
// Created by MMM on 2025/1/23.
//

#include <android/native_window.h>
#include <android/asset_manager.h>
#include <GLES3/gl3.h>
#include "GLFBOPostProcessing.h"

bool GLFBOPostProcessing::setupGraphics(int w, int h) {
    screenW = w;
    screenH = h;
    LOGI("setupGraphics(%d, %d)", w, h);
    glViewport(0, 0, w, h);
    checkGlError("glViewport");
    LOGI("glViewport successed!");

    //清屏
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    checkGlError("glClear");
    //开启深度测试
    glEnable(GL_DEPTH_TEST);
    //创建YUV程序及YUV纹理
    useYUVProgram();
    createYUVTextures();

    //创建图片程序及图片纹理
    createPicProgram();
    creatPicTexture();

    //创建FBO程序及FBO纹理
    createFBOProgram();
    creatFBOTexture();

    return true;
}

void GLFBOPostProcessing::renderFrame() {

    if (m_filter != m_prevFilter) {
        m_prevFilter = m_filter;
        //切换后期处理的着色器，实现滤镜切换。
        if (m_filter >= 0 && m_filter < m_fragmentStringPathes.size()) {
            delete_program(screenProgram);
            LOGI("render---m_filter：%d", m_filter);
            screenShader->getSharderStringPath(m_vertexStringPath,
                                               m_fragmentStringPathes.at(m_prevFilter));
            createFBOProgram();
        }
    }

    //绑定到帧缓冲区，像往常一样绘制场景以着色纹理
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    //启用深度测试（禁用渲染屏幕空间四边形）
    glEnable(GL_DEPTH_TEST);

    // 确保清除帧缓冲区的内容
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //绘制YUV视频数据纹理
    yuvGLShader->use();
    glm::mat4 model1 = glm::mat4(1.0f);
    glm::mat4 view1 = mCamera.GetViewMatrix();
    glm::mat4 projection1 = glm::perspective(glm::radians(mCamera.Zoom),
                                             (float) screenW / (float) screenH,
                                             0.1f, 100.0f);
    yuvGLShader->setMat4("view", view1);
    yuvGLShader->setMat4("projection", projection1);
    if (!updateYUVTextures() || !useYUVProgram()) return;
    model1 = glm::scale(model1, glm::vec3(6.0f, 4.0f, 1.0f));
    yuvGLShader->setMat4("model", model1);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    //绘制图片水印数据纹理
    bindPicTexture();
    usePicProgram();
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);


    //现在绑定回默认帧缓冲区，并使用附加的帧缓冲区颜色纹理绘制一个四边形平面
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    //禁用深度测试，这样屏幕空间四边形就不会因为深度测试而被丢弃。
    glDisable(GL_DEPTH_TEST);

    // 清除所有相关缓冲区
    // 将透明颜色设置为白色（实际上并没有必要，因为我们无论如何都看不到四边形后面）
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    screenShader->use();
    useFBOProgram();
    //使用颜色附着纹理作为四边形平面的纹理
    glBindTexture(GL_TEXTURE_2D, fboTexture);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    checkGlError("glDrawArrays");


    //切换到m_WindowSurface
    m_WindowSurface->makeCurrent();
    m_WindowSurface->swapBuffers();
}

GLFBOPostProcessing::GLFBOPostProcessing() {
    screenShader = new OpenGLShader;
    yuvGLShader = new OpenGLShader();
    picGLShader = new OpenGLShader();

}

GLFBOPostProcessing::~GLFBOPostProcessing() {
    deletePicTextures();
    delete_program(m_pic_program);
    delete_program(screenProgram);
    fboTexture = 0;
    //析构函数中释放资源
    glDeleteRenderbuffers(1, &rbo);
    glDeleteFramebuffers(1, &framebuffer);

    if (winsurface) {
        winsurface = nullptr;
    }

    if (m_EglCore) {
        delete m_EglCore;
        m_EglCore = nullptr;
    }

    if (m_WindowSurface) {
        delete m_WindowSurface;
        m_WindowSurface = nullptr;
    }


    if (m_pDataY) {
        m_pDataY = nullptr;
    }
    if (m_pDataU) {
        delete m_pDataU;
        m_pDataU = nullptr;
    }
    if (m_pDataV) {
        delete m_pDataV;
        m_pDataV = nullptr;
    }

    screenShader = nullptr;

    screenProgram = 0;
    m_filter = 0;

    if (picData) {
        stbi_image_free(picData);
        picData = nullptr;
    }

    if (picGLShader) {
        delete picGLShader;
        picGLShader = nullptr;
    }

    deleteYUVTextures();

    if (yuvGLShader) {
        delete yuvGLShader;
        yuvGLShader = nullptr;
    }

}

bool
GLFBOPostProcessing::setSharderScreenPathes(string vertexScreenPath,
                                            vector<string> fragmentScreenPathes) {
    screenShader->getSharderStringPath(vertexScreenPath, fragmentScreenPathes.front());
    m_vertexStringPath = vertexScreenPath;
    m_fragmentStringPathes = fragmentScreenPathes;
    return 0;
}

void GLFBOPostProcessing::setPicPath(const char *pic) {
    LOGI("setPicPath pic==%s", pic);
    picData = stbi_load(pic, &picWidth, &picHeight, &picChannels, 0);

}


bool GLFBOPostProcessing::setPicSharderPath(const char *vertexPath,
                                            const char *fragmentPath) {
    picGLShader->getSharderStringPath(vertexPath, fragmentPath);
    return 0;
}

bool GLFBOPostProcessing::setYUVSharderPath(const char *vertexPath,
                                            const char *fragmentPath) {
    yuvGLShader->getSharderPath(vertexPath, fragmentPath);
    return 0;
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


void GLFBOPostProcessing::setParameters(uint32_t i) {
    m_filter = i;
    LOGI("setParameters---m_filter：%d", m_filter);

}

jint GLFBOPostProcessing::getParameters() {
    return m_filter;
}

void GLFBOPostProcessing::createFBOProgram() {

    screenProgram = screenShader->createProgram();
    if (!screenProgram) {
        LOGE("Could not create screenProgram shaderId.");
        return;
    }
    //获取顶点着色器中的in的参数，及片段着色器的uniform
    m_screen_vertexPos = (GLuint) glGetAttribLocation(screenProgram,
                                                      "aPos");
    m_screen_textureCoordLoc = (GLuint) glGetAttribLocation(screenProgram,
                                                            "aTexCoords");
    m_textureScreenLoc = (GLuint) glGetAttribLocation(screenProgram,
                                                      "screenTexture");

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

void GLFBOPostProcessing::OnSurfaceCreated() {
    m_EglCore = new EglCore(eglGetCurrentContext(), FLAG_RECORDABLE);
    if (!m_EglCore) {
        LOGE("new EglCore failed!");
        return;
    }

    LOGE("OnSurfaceCreated m_ANWindow:%p", m_ANWindow);

    m_WindowSurface = new WindowSurface(m_EglCore, m_ANWindow);
    if (!m_EglCore) {
        LOGE("new WindowSurface failed!");
        return;
    }
    m_WindowSurface->makeCurrent();
}


void GLFBOPostProcessing::surfaceCreated(ANativeWindow *window,
                                         AAssetManager *assetManager) {
    m_ANWindow = window;
    postMessage(MSG_PS_SurfaceCreated, false);
}

void GLFBOPostProcessing::surfaceChanged(size_t width, size_t height) {
    postMessage(MSG_PS_SurfaceChanged, width, height);
}

void GLFBOPostProcessing::render() {
    postMessage(MSG_PS_DrawFrame, false);
}

void GLFBOPostProcessing::release() {
    postMessage(MSG_PS_SurfaceDestroyed, false);
}

void GLFBOPostProcessing::handleMessage(LooperMessage *msg) {
    Looper::handleMessage(msg);
    switch (msg->what) {
        case MSG_PS_SurfaceCreated: {
            OnSurfaceCreated();
        }
            break;
        case MSG_PS_SurfaceChanged:
            setupGraphics(msg->arg1, msg->arg2);
            break;
        case MSG_PS_DrawFrame:
            renderFrame();
            break;
        case MSG_PS_SurfaceDestroyed:
//            OnSurfaceDestroyed();
            break;
        default:
            break;
    }
}

void
GLFBOPostProcessing::draw(uint8_t *buffer, size_t length,
                          size_t width, size_t height,
                          float rotation) {
    ps_video_frame frame{};
    frame.width = width;
    frame.height = height;
    frame.stride_y = width;
    frame.stride_uv = width / 2;
    frame.y = buffer;
    frame.u = buffer + width * height;
    frame.v = buffer + width * height * 5 / 4;

    updateFrame(frame);
}

void GLFBOPostProcessing::updateFrame(const ps_video_frame &frame) {
    m_sizeY = frame.width * frame.height;
    m_sizeU = frame.width * frame.height / 4;
    m_sizeV = frame.width * frame.height / 4;

    if (m_pDataY == nullptr || m_width != frame.width
        || m_height != frame.height) {
        m_pDataY = std::make_unique<uint8_t[]>(m_sizeY
                                               + m_sizeU + m_sizeV);
        m_pDataU = m_pDataY.get() + m_sizeY;
        m_pDataV = m_pDataU + m_sizeU;
    }

    m_width = frame.width;
    m_height = frame.height;

    if (m_width == frame.stride_y) {
        memcpy(m_pDataY.get(), frame.y, m_sizeY);
    } else {
        uint8_t *pSrcY = frame.y;
        uint8_t *pDstY = m_pDataY.get();

        for (int h = 0; h < m_height; h++) {
            memcpy(pDstY, pSrcY, m_width);

            pSrcY += frame.stride_y;
            pDstY += m_width;
        }
    }

    if (m_width / 2 == frame.stride_uv) {
        memcpy(m_pDataU, frame.u, m_sizeU);
        memcpy(m_pDataV, frame.v, m_sizeV);
    } else {
        uint8_t *pSrcU = frame.u;
        uint8_t *pSrcV = frame.v;
        uint8_t *pDstU = m_pDataU;
        uint8_t *pDstV = m_pDataV;

        for (int h = 0; h < m_height / 2; h++) {
            memcpy(pDstU, pSrcU, m_width / 2);
            memcpy(pDstV, pSrcV, m_width / 2);

            pDstU += m_width / 2;
            pDstV += m_width / 2;

            pSrcU += frame.stride_uv;
            pSrcV += frame.stride_uv;
        }
    }

    isDirty = true;
}


bool GLFBOPostProcessing::createYUVTextures() {
    auto widthY = (GLsizei) m_width;
    auto heightY = (GLsizei) m_height;

    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1, &m_textureIdY);
    glBindTexture(GL_TEXTURE_2D, m_textureIdY);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, widthY, heightY,
                 0, GL_LUMINANCE, GL_UNSIGNED_BYTE,
                 nullptr);

    if (!m_textureIdY) {
        LOGE("OpenGL Error Create Y texture");
        return false;
    }

    GLsizei widthU = (GLsizei) m_width / 2;
    GLsizei heightU = (GLsizei) m_height / 2;

    glActiveTexture(GL_TEXTURE1);
    glGenTextures(1, &m_textureIdU);
    glBindTexture(GL_TEXTURE_2D, m_textureIdU);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, widthU, heightU,
                 0, GL_LUMINANCE, GL_UNSIGNED_BYTE,
                 nullptr);

    if (!m_textureIdU) {
        LOGE("OpenGL Error Create U texture");
        return false;
    }

    GLsizei widthV = (GLsizei) m_width / 2;
    GLsizei heightV = (GLsizei) m_height / 2;

    glActiveTexture(GL_TEXTURE2);
    glGenTextures(1, &m_textureIdV);
    glBindTexture(GL_TEXTURE_2D, m_textureIdV);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, widthV, heightV, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE,
                 nullptr);

    if (!m_textureIdV) {
        LOGE("OpenGL Error Create V texture");
        return false;
    }

    return true;
}

bool GLFBOPostProcessing::updateYUVTextures() {

    if (!m_textureIdY && !m_textureIdU && !m_textureIdV) return false;
//    LOGE("updateTextures m_textureIdY:%d,m_textureIdU:%d,m_textureIdV:%d,===isDirty:%d",
//         m_textureIdY,
//         m_textureIdU, m_textureIdV, isDirty);

    if (isDirty) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_textureIdY);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE,
                     (GLsizei) m_width, (GLsizei) m_height, 0,
                     GL_LUMINANCE, GL_UNSIGNED_BYTE, m_pDataY.get());

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, m_textureIdU);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE,
                     (GLsizei) m_width / 2, (GLsizei) m_height / 2,
                     0,
                     GL_LUMINANCE, GL_UNSIGNED_BYTE, m_pDataU);

        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, m_textureIdV);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE,
                     (GLsizei) m_width / 2, (GLsizei) m_height / 2,
                     0,
                     GL_LUMINANCE, GL_UNSIGNED_BYTE, m_pDataV);

        isDirty = false;
        return true;
    }

    return false;
}

void GLFBOPostProcessing::deleteYUVTextures() {
    if (m_textureIdY) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 0);
        glDeleteTextures(1, &m_textureIdY);

        m_textureIdY = 0;
    }

    if (m_textureIdU) {
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, 0);
        glDeleteTextures(1, &m_textureIdU);

        m_textureIdU = 0;
    }

    if (m_textureIdV) {
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, 0);
        glDeleteTextures(1, &m_textureIdV);
        m_textureIdV = 0;
    }
}

int
GLFBOPostProcessing::createYUVProgram() {

    //创建YUV视频通道着色器程序
    m_yuv_program = yuvGLShader->createProgram();
    LOGI("GLFboDrawTextVideoRender createProgram m_yuv_program:%d", m_yuv_program);

    if (!m_yuv_program) {
        LOGE("Could not create program.");
        return 0;
    }
    //获取顶点着色器中的in的参数，及片段着色器的uniform
    //Get Uniform Variables Location
    m_yuv_vertexPos = (GLuint) glGetAttribLocation(m_yuv_program,
                                                   "position");
    m_textureYLoc = glGetUniformLocation(m_yuv_program,
                                         "s_textureY");
    m_textureULoc = glGetUniformLocation(m_yuv_program,
                                         "s_textureU");
    m_textureVLoc = glGetUniformLocation(m_yuv_program,
                                         "s_textureV");
    m_yuv_textureCoordLoc = (GLuint) glGetAttribLocation(m_yuv_program,
                                                         "texcoord");

    return m_yuv_program;
}

GLuint GLFBOPostProcessing::useYUVProgram() {
    if (!m_yuv_program && !createYUVProgram()) {
        LOGE("Could not use program.");
        return 0;
    }

    //绑定顶点数据及纹理数据
    glUseProgram(m_yuv_program);
    glVertexAttribPointer(m_yuv_vertexPos, 3, GL_FLOAT, GL_FALSE,
                          0, FboPsVerticek);
    glEnableVertexAttribArray(m_yuv_vertexPos);

    glUniform1i(m_textureYLoc, 0);
    glUniform1i(m_textureULoc, 1);
    glUniform1i(m_textureVLoc, 2);
    glVertexAttribPointer(m_yuv_textureCoordLoc, 3, GL_FLOAT, GL_FALSE,
                          0, FboPsTextureCoord);
    glEnableVertexAttribArray(m_yuv_textureCoordLoc);
    return m_yuv_program;
}

int
GLFBOPostProcessing::createPicProgram() {
    //创建图片水印着色程序
    m_pic_program = picGLShader->createProgram();
    if (!m_pic_program) {
        LOGE("Could not create m_pic_program.");
        return 0;
    }
    //获取顶点着色器中的in的参数，及片段着色器的uniform
    m_pic_vertexPos = (GLuint) glGetAttribLocation(
            m_pic_program, "position");
    m_pic_textureCoordLoc = (GLuint) glGetAttribLocation(
            m_pic_program, "texcoord");
    m_texturePicLoc = (GLuint) glGetUniformLocation(
            m_pic_program, "s_texturePic");
    return m_pic_program;
}

void GLFBOPostProcessing::usePicProgram() {
    glUseProgram(m_pic_program);
    //绑定顶点数据及纹理数据
    glVertexAttribPointer(m_pic_vertexPos, 3, GL_FLOAT,
                          GL_FALSE, 0, EGLFboPSTextVerticek1);
    glEnableVertexAttribArray(m_pic_vertexPos);

    glUniform1i(m_texturePicLoc, 3);
    glVertexAttribPointer(m_pic_textureCoordLoc, 3,
                          GL_FLOAT, GL_FALSE, 0,
                          EGLFboPSTextTextureCoord);
    glEnableVertexAttribArray(m_pic_textureCoordLoc);
}

void GLFBOPostProcessing::useFBOProgram() {
    glUseProgram(screenProgram);
    //绑定顶点数据及纹理数据
    glVertexAttribPointer(m_screen_vertexPos, 3,
                          GL_FLOAT, GL_FALSE, 0,
                          PostProcessingQuadVertices1);
    glEnableVertexAttribArray(m_screen_vertexPos);

    glUniform1i(m_textureScreenLoc, 3);
    glVertexAttribPointer(m_screen_textureCoordLoc, 3,
                          GL_FLOAT, GL_FALSE, 0,
                          PostProcessingQuadTextCoord);
    glEnableVertexAttribArray(m_screen_textureCoordLoc);
}

void GLFBOPostProcessing::deletePicTextures() {
    if (m_texturePicLoc) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 0);
        glDeleteTextures(1, &m_texturePicLoc);
        m_texturePicLoc = 0;
    }

}

void GLFBOPostProcessing::bindPicTexture() {
    if (m_texturePicLoc) {
        // bind Texture
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_texturePicLoc);
    }
}

void GLFBOPostProcessing::creatPicTexture() {

    if (picData) {
        GLenum format = 0;
        if (picChannels == 1) {
            format = GL_RED;
        } else if (picChannels == 3) {
            format = GL_RGB;
        } else if (picChannels == 4) {
            format = GL_RGBA;
        }
        glGenTextures(1, &m_texturePicLoc);
        glBindTexture(GL_TEXTURE_2D, m_texturePicLoc);
        glTexImage2D(GL_TEXTURE_2D, 0, format, picWidth,
                     picHeight, 0, format, GL_UNSIGNED_BYTE,
                     picData);
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                        GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
                        GL_LINEAR);
        stbi_image_free(picData);
    } else {
        LOGE("creatPicTexture picData  =(null)");
        stbi_image_free(picData);
    }

    if (!m_texturePicLoc) {
        LOGE("creatPicTexture Error Create PIC texture");
    }

}

void GLFBOPostProcessing::creatFBOTexture() {
    //1.首先要创建一个帧缓冲对象，并绑定它，这些都很直观
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    //2.接下来我们需要创建一个纹理图像，我们将它作为一个颜色附件附加到帧缓冲上。
    // 我们将纹理的维度设置为窗口的宽度和高度，并且不初始化它的数据
    glGenTextures(1, &fboTexture);
    glBindTexture(GL_TEXTURE_2D, fboTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, screenW, screenH,
                 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                    GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
                    GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                           GL_TEXTURE_2D, fboTexture,
                           0);

    //3.创建渲染缓冲对象
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, screenW,
                          screenH);
    //4.将渲染缓冲对象附加到帧缓冲的深度和模板附件上
    glFramebufferRenderbuffer(GL_FRAMEBUFFER,
                              GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER,
                              rbo);
    //5.最后，我们希望检查帧缓冲是否是完整的，如果不是，我们将打印错误信息
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        LOGE("ERROR::FRAMEBUFFER:: Framebuffer is not complete!");
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

}