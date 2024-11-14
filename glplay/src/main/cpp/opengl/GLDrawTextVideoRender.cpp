//  Author : wangyongyao https://github.com/wangyongyao1989
// Created by MMM on 2024/11/6.
//

#include "GLDrawTextVideoRender.h"
#include "OpenGLShader.h"

void GLDrawTextVideoRender::surfaceCreated(ANativeWindow *window, AAssetManager *assetManager) {
    m_ANWindow = window;
    postMessage(MSG_Draw_Text_SurfaceCreated, false);
}

void GLDrawTextVideoRender::surfaceChanged(size_t width, size_t height) {
    postMessage(MSG_Draw_Text_SurfaceChanged, width, height);
}

void GLDrawTextVideoRender::render() {
    postMessage(MSG_Draw_Text_DrawFrame, false);
}

void GLDrawTextVideoRender::release() {
    postMessage(MSG_Draw_Text_SurfaceDestroyed, false);
}

void GLDrawTextVideoRender::updateFrame(const draw_text_video_frame &frame) {
    m_sizeY = frame.width * frame.height;
    m_sizeU = frame.width * frame.height / 4;
    m_sizeV = frame.width * frame.height / 4;

    if (m_pDataY == nullptr || m_width != frame.width || m_height != frame.height) {
        m_pDataY = std::make_unique<uint8_t[]>(m_sizeY + m_sizeU + m_sizeV);
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

void
GLDrawTextVideoRender::draw(uint8_t *buffer, size_t length, size_t width, size_t height,
                            float rotation) {
    draw_text_video_frame frame{};
    frame.width = width;
    frame.height = height;
    frame.stride_y = width;
    frame.stride_uv = width / 2;
    frame.y = buffer;
    frame.u = buffer + width * height;
    frame.v = buffer + width * height * 5 / 4;

    updateFrame(frame);
}

void GLDrawTextVideoRender::setParameters(uint32_t params) {
    m_params = params;
}

uint32_t GLDrawTextVideoRender::getParameters() {
    return m_params;
}

bool GLDrawTextVideoRender::createYUVTextures() {
    auto widthY = (GLsizei) m_width;
    auto heightY = (GLsizei) m_height;

    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1, &m_textureIdY);
    glBindTexture(GL_TEXTURE_2D, m_textureIdY);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, widthY, heightY, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE,
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
    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, widthU, heightU, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE,
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

bool GLDrawTextVideoRender::updateYUVTextures() {

    if (!m_textureIdY && !m_textureIdU && !m_textureIdV) return false;
//    LOGE("updateTextures m_textureIdY:%d,m_textureIdU:%d,m_textureIdV:%d,===isDirty:%d",
//         m_textureIdY,
//         m_textureIdU, m_textureIdV, isDirty);

    if (isDirty) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_textureIdY);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, (GLsizei) m_width, (GLsizei) m_height, 0,
                     GL_LUMINANCE, GL_UNSIGNED_BYTE, m_pDataY.get());

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, m_textureIdU);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, (GLsizei) m_width / 2, (GLsizei) m_height / 2,
                     0,
                     GL_LUMINANCE, GL_UNSIGNED_BYTE, m_pDataU);

        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, m_textureIdV);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, (GLsizei) m_width / 2, (GLsizei) m_height / 2,
                     0,
                     GL_LUMINANCE, GL_UNSIGNED_BYTE, m_pDataV);

        isDirty = false;
        return true;
    }

    return false;
}

void GLDrawTextVideoRender::bindPicTexture() {
    if (m_texturePicLoc) {
        // bind Texture
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_texturePicLoc);
    }
}

int
GLDrawTextVideoRender::createPicProgram() {
    //创建图片水印着色程序
    m_pic_program = picGLShader->createProgram();
    if (!m_pic_program) {
        LOGE("Could not create m_pic_program.");
        return 0;
    }
    m_pic_vertexPos = (GLuint) glGetAttribLocation(m_pic_program, "position");
    m_pic_textureCoordLoc = (GLuint) glGetAttribLocation(m_pic_program, "texcoord");
    m_texturePicLoc = (GLuint) glGetUniformLocation(m_pic_program, "s_texturePic");
    return m_pic_program;
}

int
GLDrawTextVideoRender::createTextProgram() {
    //创建文字水印着色程序
    m_text_program = textGLShader->createProgram();
    if (!m_text_program) {
        LOGE("Could not create m_text_program.");
        return 0;
    }
    return m_text_program;
}

int
GLDrawTextVideoRender::createYUVProgram() {

    //创建YUV视频通道着色器程序
    m_yuv_program = yuvGLShader->createProgram();
    m_vertexShader = yuvGLShader->vertexShader;
    m_pixelShader = yuvGLShader->fraShader;
    LOGI("GLDrawTextVideoRender createProgram m_yuv_program:%d", m_yuv_program);

    if (!m_yuv_program) {
        LOGE("Could not create program.");
        return 0;
    }

    //Get Uniform Variables Location
    m_yuv_vertexPos = (GLuint) glGetAttribLocation(m_yuv_program, "position");
    m_textureYLoc = glGetUniformLocation(m_yuv_program, "s_textureY");
    m_textureULoc = glGetUniformLocation(m_yuv_program, "s_textureU");
    m_textureVLoc = glGetUniformLocation(m_yuv_program, "s_textureV");
    m_yuv_textureCoordLoc = (GLuint) glGetAttribLocation(m_yuv_program, "texcoord");
    return m_yuv_program;
}


GLuint GLDrawTextVideoRender::useYUVProgram() {
    if (!m_yuv_program && !createYUVProgram()) {
        LOGE("Could not use program.");
        return 0;
    }
    glUseProgram(m_yuv_program);
    glVertexAttribPointer(m_yuv_vertexPos, 3, GL_FLOAT, GL_FALSE, 0, EGLTextVerticek);
    glEnableVertexAttribArray(m_yuv_vertexPos);

    glUniform1i(m_textureYLoc, 0);
    glUniform1i(m_textureULoc, 1);
    glUniform1i(m_textureVLoc, 2);
    glVertexAttribPointer(m_yuv_textureCoordLoc, 3, GL_FLOAT, GL_FALSE, 0, EGLTextTextureCoord);
    glEnableVertexAttribArray(m_yuv_textureCoordLoc);
    return m_yuv_program;
}


void GLDrawTextVideoRender::usePicProgram() {
    glUseProgram(m_pic_program);
    glVertexAttribPointer(m_pic_vertexPos, 3, GL_FLOAT, GL_FALSE, 0, EGLTextVerticek1);
    glEnableVertexAttribArray(m_pic_vertexPos);

    glUniform1i(m_texturePicLoc, 3);
    glVertexAttribPointer(m_pic_textureCoordLoc, 3, GL_FLOAT, GL_FALSE, 0, EGLTextTextureCoord);
    glEnableVertexAttribArray(m_pic_textureCoordLoc);
}


bool
GLDrawTextVideoRender::setSharderPath(const char *vertexPath, const char *fragmentPath,
                                      const char *fragmentPath1) {
    yuvGLShader->getSharderPath(vertexPath, fragmentPath);
    picGLShader->getSharderStringPath(vertexPath, fragmentPath1);
    return 0;
}

bool
GLDrawTextVideoRender::setTextSharderPath(const char *vertexPath, const char *fragmentPath,
                                          const char *freeTypePath) {
    textGLShader->getSharderPath(vertexPath, fragmentPath);
    freeTypePathString = freeTypePath;
    return 0;
}

bool
GLDrawTextVideoRender::setPicTextPath(const char *picPath) {
    LOGI("setPicTextPath picPath:%s", picPath);
    picData = stbi_load(picPath, &picWidth, &picHeight, &picChannels, 0);
    return 0;
}

bool GLDrawTextVideoRender::setSharderStringPath(string vertexPath, string fragmentPath) {
    yuvGLShader->getSharderStringPath(vertexPath, fragmentPath);
    return 0;
}

GLDrawTextVideoRender::GLDrawTextVideoRender() {
    yuvGLShader = new OpenGLShader();
    picGLShader = new OpenGLShader();
    textGLShader = new OpenGLShader();
}

GLDrawTextVideoRender::~GLDrawTextVideoRender() {
    deleteYUVTextures();
    deletePicTextures();
    delete_program(m_yuv_program);
    delete_program(m_pic_program);
    m_vertexShader = 0;
    m_pixelShader = 0;
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

    if (yuvGLShader) {
        delete yuvGLShader;
        yuvGLShader = nullptr;
    }

    if (picGLShader) {
        delete picGLShader;
        picGLShader = nullptr;
    }

    if (textGLShader) {
        delete textGLShader;
        textGLShader = nullptr;
    }

    if (display) {
        display = nullptr;
    }

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
    quit();

    if (picData) {
        picData = nullptr;
    }

    if (VAO) {
        glDeleteVertexArrays(1, &VAO);

    }
    if (VBO) {
        glDeleteBuffers(1, &VBO);
    }

    std::map<GLchar, Character>::const_iterator iter;
    for (iter = Characters.begin(); iter != Characters.end(); iter++) {
        glDeleteTextures(1, &Characters[iter->first].TextureID);
    }

}

void GLDrawTextVideoRender::delete_program(GLuint &program) {
    if (program) {
        glUseProgram(0);
        glDeleteProgram(program);
        program = 0;
    }
}

void GLDrawTextVideoRender::deleteYUVTextures() {
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

void GLDrawTextVideoRender::deletePicTextures() {
    if (m_texturePicLoc) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 0);
        glDeleteTextures(1, &m_texturePicLoc);
        m_texturePicLoc = 0;
    }
}


void GLDrawTextVideoRender::handleMessage(LooperMessage *msg) {
    Looper::handleMessage(msg);
    switch (msg->what) {
        case MSG_Draw_Text_SurfaceCreated: {
            OnSurfaceCreated();
        }
            break;
        case MSG_Draw_Text_SurfaceChanged:
            OnSurfaceChanged(msg->arg1, msg->arg2);
            break;
        case MSG_Draw_Text_DrawFrame:
            OnDrawFrame();
            break;
        case MSG_Draw_Text_SurfaceDestroyed:
            OnSurfaceDestroyed();
            break;
        default:
            break;
    }
}

void GLDrawTextVideoRender::OnSurfaceCreated() {
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

void GLDrawTextVideoRender::OnSurfaceChanged(int w, int h) {
    m_backingWidth = w;
    m_backingHeight = h;
    LOGE("OnSurfaceChanged m_backingWidth:%d,m_backingHeight:%d", m_backingWidth, m_backingHeight);
    float windowAspect = (float) m_backingHeight / (float) m_backingWidth;
    size_t outWidth, outHeight;
    if (VIDEO_HEIGHT_DRAW_TEXT > VIDEO_WIDTH_DRAW_TEXT * windowAspect) {
        // limited by narrow width; reduce height
        outWidth = VIDEO_WIDTH_DRAW_TEXT;
        outHeight = (int) (VIDEO_WIDTH_DRAW_TEXT * windowAspect);
    } else {
        // limited by short height; restrict width
        outHeight = VIDEO_HEIGHT_DRAW_TEXT;
        outWidth = (int) (VIDEO_HEIGHT_DRAW_TEXT / windowAspect);
    }
    LOGE(" outWidth:%d,outHeight:%d", outWidth, outHeight);

    offX = (VIDEO_WIDTH_DRAW_TEXT - outWidth) / 2;
    offY = (VIDEO_HEIGHT_DRAW_TEXT - outHeight) / 2;
    off_right = offX + outWidth;
    off_bottom = offY + outHeight;
    //Adjusting window 1920x1104 to +14,+0 1252x720
    LOGE("Adjusting window offX:%d,offY:%d,off_right:%d,off_bottom:%d", offX, offY, off_right,
         off_bottom);
    // Set OpenGL options
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    useYUVProgram();
    createPicProgram();
    createTextProgram();

    createYUVTextures();
    creatPicTexture();

    const char *freeTypePath = freeTypePathString.data();
    LoadFacesByASCII(freeTypePath);
    bindTextVertexData();
}

void GLDrawTextVideoRender::OnDrawFrame() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

    //绘制YUV视频数据纹理
    if (!updateYUVTextures() || !useYUVProgram()) return;
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    //绘制图片水印数据纹理
    bindPicTexture();
    usePicProgram();
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    //绘制文字水印
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); //禁用byte-alignment限制
    glEnable(GL_BLEND);
    //glEnable(GL_CULL_FACE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    checkGlError("glBlendFunc");
    glm::vec2 viewport(m_backingWidth, m_backingHeight);
    RenderText("https://blog.csdn.net/wangyongyao1989", 300.0f, 500.0f, 1.0f, glm::vec3(0.8, 0.1f, 0.1f), viewport);


//    LOGE("OnDrawFrame thread:%ld", pthread_self());
    if (m_TextureMovieEncoder2 != nullptr) {
        m_TextureMovieEncoder2->frameAvailableSoon();
    }
    if (m_InputWindowSurface != nullptr) {
        m_InputWindowSurface->makeCurrentReadFrom(*m_WindowSurface);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        checkGlError("before glBlitFramebuffer");
        glBlitFramebuffer(0, 0, m_backingWidth, m_backingHeight, offX, offY, off_right, off_bottom,
                          GL_COLOR_BUFFER_BIT, GL_NEAREST);
        m_InputWindowSurface->swapBuffers();

    }

    //切换到m_WindowSurface
    m_WindowSurface->makeCurrent();
    m_WindowSurface->swapBuffers();

}

void GLDrawTextVideoRender::OnSurfaceDestroyed() {
    m_vertexShader = 0;
    m_pixelShader = 0;

    if (m_pDataY) {
        m_pDataY = nullptr;
    }

    if (m_pDataU) {
        m_pDataU = nullptr;
    }

    if (m_pDataV) {
        m_pDataV = nullptr;
    }

    if (yuvGLShader) {
        delete yuvGLShader;
        yuvGLShader = nullptr;
    }

    if (display) {
        display = nullptr;
    }

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

    quit();

}

void GLDrawTextVideoRender::printGLString(const char *name, GLenum s) {
    const char *v = (const char *) glGetString(s);
    LOGI("OpenGL %s = %s\n", name, v);
}

void GLDrawTextVideoRender::checkGlError(const char *op) {
    for (GLint error = glGetError(); error; error = glGetError()) {
        LOGI("after %s() glError (0x%x)\n", op, error);
    }
}

void GLDrawTextVideoRender::startEncoder(const char *recordPath) {
    LOGD("GLDrawTextVideoRender::startEncoder()");
    m_VideoEncoderCore = new VideoEncoderCore(VIDEO_WIDTH_DRAW_TEXT, VIDEO_HEIGHT_DRAW_TEXT,
                                              BIT_RATE_DRAW_TEXT, recordPath);
    m_InputWindowSurface = new WindowSurface(m_EglCore, m_VideoEncoderCore->getInputSurface());
    m_TextureMovieEncoder2 = new TextureMovieEncoder2(m_VideoEncoderCore);
}

void GLDrawTextVideoRender::stopEncoder() {
    LOGD("GLDrawTextVideoRender::stopEncoder()");
    if (m_TextureMovieEncoder2 != nullptr) {
        m_TextureMovieEncoder2->stopRecording();
    }
    if (m_VideoEncoderCore != nullptr) {
        m_VideoEncoderCore->release();
        m_VideoEncoderCore = nullptr;
    }
    if (m_InputWindowSurface != nullptr) {
        m_InputWindowSurface->release();
        m_InputWindowSurface = nullptr;
    }

    if (m_TextureMovieEncoder2 != nullptr) {
        m_TextureMovieEncoder2 = nullptr;
    }
}


void GLDrawTextVideoRender::creatPicTexture() {

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
        glTexImage2D(GL_TEXTURE_2D, 0, format, picWidth, picHeight, 0, format, GL_UNSIGNED_BYTE,
                     picData);
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        stbi_image_free(picData);
    } else {
        LOGE("creatPicTexture picData  =(null)");
        stbi_image_free(picData);
    }

    if (!m_texturePicLoc) {
        LOGE("creatPicTexture Error Create PIC texture");
    }

}

void GLDrawTextVideoRender::bindTextVertexData() {
    glm::vec2 viewport(m_backingWidth, m_backingHeight);
    glm::mat4 projection = glm::ortho(0.0f, static_cast<GLfloat>(m_backingWidth), 0.0f,
                                      static_cast<GLfloat>(m_backingHeight));
    textGLShader->use();
    textGLShader->setMat4("projection", projection);
    checkGlError("drawTextShader->setMat4(projection");


    // Configure VAO/VBO for texture quads
    glGenVertexArrays(1, &VAO);
    checkGlError("glGenVertexArrays(1, &VAO);");

    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    checkGlError("glBindVertexArray");
}

void GLDrawTextVideoRender::RenderText(std::string text, GLfloat x, GLfloat y, GLfloat scale,
                                       glm::vec3 color, glm::vec2 viewport) {
    // 激活对应的渲染状态
    textGLShader->use();
    checkGlError("drawTextShader->use()");
    textGLShader->setVec3("textColor", color.x, color.y, color.z);

    glActiveTexture(GL_TEXTURE0);
    checkGlError("glActiveTexture");
    glBindVertexArray(VAO);
    checkGlError("glBindVertexArray(VAO)");

    // 遍历文本中所有的字符
    std::string::const_iterator c;

    LOGE("RenderText x:%f == y:%f", x, y);
    LOGE("RenderText viewportX:%f == viewportY:%f", viewport.x, viewport.y);

    for (c = text.begin(); c != text.end(); c++) {
        Character ch = Characters[*c];

        GLfloat xpos = x + ch.Bearing.x * scale;
        GLfloat ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

        GLfloat w = ch.Size.x * scale;
        GLfloat h = ch.Size.y * scale;

//        LOGE("TextRenderSample::RenderText [xpos,ypos,w,h]=[%f, %f, %f, %f], ch.advance >> 6 = %d"
//                , xpos, ypos, w, h, ch.Advance >> 6);

        // 对每个字符更新VBO
        GLfloat vertices[6][4] = {
                {xpos,     ypos + h, 0.0, 0.0},
                {xpos,     ypos,     0.0, 1.0},
                {xpos + w, ypos,     1.0, 1.0},

                {xpos,     ypos + h, 0.0, 0.0},
                {xpos + w, ypos,     1.0, 1.0},
                {xpos + w, ypos + h, 1.0, 0.0}
        };
        // 在四边形上绘制字形纹理
        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        checkGlError("glBindTexture");
        // 更新VBO内存的内容
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        checkGlError("glBindBuffer");
        // Be sure to use glBufferSubData and not glBufferData
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        checkGlError("glBufferSubData");
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        checkGlError("glBindBuffer(GL_ARRAY_BUFFER");
        // 绘制四边形
        glDrawArrays(GL_TRIANGLES, 0, 6);
        checkGlError("glDrawArrays(GL_TRIANGLES");

        // 更新位置到下一个字形的原点，注意单位是1/64像素
        // 位偏移6个单位来获取单位为像素的值 (2^6 = 64)
        x += (ch.Advance >> 6) * scale;
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}


void GLDrawTextVideoRender::LoadFacesByASCII(const char *path) {
    LOGE("GLDrawTextVideoRender::LoadFacesByASCII path:%s", path);
    // FreeType
    FT_Library ft;
    // All functions return a value different than 0 whenever an error occurred
    if (FT_Init_FreeType(&ft))
        LOGE("ERROR::FREETYPE: Could not init FreeType Library");

    // Load font as face
    FT_Face face;
    if (FT_New_Face(ft, path, 0, &face))
        LOGE("ERROR::FREETYPE: Failed to load font");

    // Set size to load glyphs as
    FT_Set_Pixel_Sizes(face, 0, 48);

    // Disable byte-alignment restriction
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    // Load first 128 characters of ASCII set
    for (GLubyte c = 0; c < 128; c++) {
        // Load character glyph
        if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
            LOGE("ERROR::FREETYTPE: Failed to load Glyph");
            continue;
        }
        // Generate texture
        GLuint texture;
        glGenTextures(1, &texture);
        checkGlError("LoadFacesByASCII glGenTextures");
//        LOGE("fore c= %d",c);
//        LOGE("texture === %d",texture);

        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
                GL_TEXTURE_2D,
                0,
                //选择GL_LUMINANCE来于:
                // https://stackoverflow.com/questions/70285879/android12-opengles3-0-glteximage2d-0x502-error
                GL_LUMINANCE,
                face->glyph->bitmap.width,
                face->glyph->bitmap.rows,
                0,
                GL_LUMINANCE,
                GL_UNSIGNED_BYTE,
                face->glyph->bitmap.buffer
        );
        // Set texture options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // Now store character for later use
        Character character = {
                texture,
                glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
                glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
                static_cast<GLuint>(face->glyph->advance.x)
        };
        Characters.insert(std::pair<GLchar, Character>(c, character));
    }
    glBindTexture(GL_TEXTURE_2D, 0);
    checkGlError("glBindTexture(GL_TEXTURE_2D, 0);");

    // Destroy FreeType once we're finished
    FT_Done_Face(face);
    FT_Done_FreeType(ft);
    LOGE("FT_Done_FreeType");

}


