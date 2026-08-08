//  Author : wangyongyao https://github.com/wangyongyao1989
// Created by MMM on 2024/10/13.
//

#include "EGLSurfaceViewVideoRender.h"
#include "OpenGLShader.h"


/**
 * 外部调用：Surface创建时的回调
 * window: Android原生窗口指针
 */
void EGLSurfaceViewVideoRender::surfaceCreated(ANativeWindow *window, AAssetManager *assetManager) {
    LOGD("EGLSurfaceViewVideoRender::surfaceCreated [window=%p]", window);
    m_ANWindow = window;
    postMessage(MSG_SurfaceCreated, false);
}

/**
 * 外部调用：Surface尺寸改变时的回调
 */
void EGLSurfaceViewVideoRender::surfaceChanged(size_t width, size_t height) {
    LOGD("EGLSurfaceViewVideoRender::surfaceChanged [w=%zu, h=%zu]", width, height);
    postMessage(MSG_SurfaceChanged, width, height);
}

/**
 * 外部调用：触发一帧渲染请求
 */
void EGLSurfaceViewVideoRender::render() {
    if (m_isReleasing) return;
    postMessage(MSG_DrawFrame, false);
}

/**
 * 外部调用：释放资源，停止渲染线程
 */
void EGLSurfaceViewVideoRender::release() {
    LOGI("EGLSurfaceViewVideoRender::release - Triggering cleanup");
    m_isReleasing = true;
    postMessage(MSG_SurfaceDestroyed, true); // 使用 flush 确保销毁消息最先处理
    quit();
}

/**
 * 核心逻辑：接收并缓存视频帧数据
 * 处理 YUV 数据拷贝和步长（stride）适配
 */
void EGLSurfaceViewVideoRender::updateFrame(const egl_surface_video_frame &frame) {
    if (m_isReleasing) return;

    // 1. 计算内存布局
    m_sizeY = frame.width * frame.height;
    m_sizeU = frame.width * frame.height / 4;
    m_sizeV = frame.width * frame.height / 4;

    // 2. 按需重新分配缓冲区
    if (m_pDataY == nullptr || m_width != frame.width || m_height != frame.height) {
        LOGD("EGLSurfaceViewVideoRender::updateFrame - Allocating buffer for %zux%zu", frame.width, frame.height);
        m_pDataY = std::make_unique<uint8_t[]>(m_sizeY + m_sizeU + m_sizeV);
        m_pDataU = m_pDataY.get() + m_sizeY;
        m_pDataV = m_pDataU + m_sizeU;
        isProgramChanged = true;
    }

    m_width = frame.width;
    m_height = frame.height;

    // 3. 数据拷贝（处理 Stride）
    // Y 分量
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

    // U/V 分量
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

    isDirty = true; // 标记需要更新纹理
}

void
EGLSurfaceViewVideoRender::draw(uint8_t *buffer, size_t length, size_t width, size_t height,
                                float rotation) {
    egl_surface_video_frame frame{};
    frame.width = width;
    frame.height = height;
    frame.stride_y = width;
    frame.stride_uv = width / 2;
    frame.y = buffer;
    frame.u = buffer + width * height;
    frame.v = buffer + width * height * 5 / 4;

    updateFrame(frame);
}

void EGLSurfaceViewVideoRender::setParameters(uint32_t params) {
    m_params = params;
}

uint32_t EGLSurfaceViewVideoRender::getParameters() {
    return m_params;
}

/**
 * 内部逻辑：创建纹理 ID 并初始化纹理参数
 */
bool EGLSurfaceViewVideoRender::createTextures() {
    LOGD("EGLSurfaceViewVideoRender::createTextures");
    auto widthY = (GLsizei) m_width;
    auto heightY = (GLsizei) m_height;

    // 创建 Y 纹理
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

    // 创建 U 纹理
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

    // 创建 V 纹理
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

/**
 * 内部逻辑：将内存中的 YUV 数据上传到 GPU 纹理
 */
bool EGLSurfaceViewVideoRender::updateTextures() {
    if (!m_textureIdY && !m_textureIdU && !m_textureIdV /*&& !createTextures()*/) return false;

    if (isDirty) {
        // 更新 Y
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_textureIdY);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, (GLsizei) m_width, (GLsizei) m_height, 0,
                     GL_LUMINANCE, GL_UNSIGNED_BYTE, m_pDataY.get());

        // 更新 U
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, m_textureIdU);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, (GLsizei) m_width / 2, (GLsizei) m_height / 2,
                     0,
                     GL_LUMINANCE, GL_UNSIGNED_BYTE, m_pDataU);

        // 更新 V
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

/**
 * 内部逻辑：初始化 Shader 程序
 */
int
EGLSurfaceViewVideoRender::createProgram() {
    LOGD("EGLSurfaceViewVideoRender::createProgram");
    m_program = openGlShader->createProgram();
    m_vertexShader = openGlShader->vertexShader;
    m_pixelShader = openGlShader->fraShader;
    LOGI("EGLSurfaceViewVideoRender createProgram m_program:%d", m_program);

    if (!m_program) {
        LOGE("Could not create program.");
        return 0;
    }

    //Get Uniform Variables Location
    m_vertexPos = (GLuint) glGetAttribLocation(m_program, "position");
    m_textureYLoc = glGetUniformLocation(m_program, "s_textureY");
    m_textureULoc = glGetUniformLocation(m_program, "s_textureU");
    m_textureVLoc = glGetUniformLocation(m_program, "s_textureV");
    m_textureLoc = (GLuint) glGetAttribLocation(m_program, "texcoord");
    m_textureSize = glGetUniformLocation(m_program, "texSize");

    return m_program;
}

GLuint EGLSurfaceViewVideoRender::useProgram() {
    if (!m_program && !createProgram()) {
        LOGE("Could not use program.");
        return 0;
    }

    if (isProgramChanged) {
        glUseProgram(m_program);
        glVertexAttribPointer(m_vertexPos, 2, GL_FLOAT, GL_FALSE, 0, EGLVerticek);
        glEnableVertexAttribArray(m_vertexPos);

        glUniform1i(m_textureYLoc, 0);
        glUniform1i(m_textureULoc, 1);
        glUniform1i(m_textureVLoc, 2);
        glVertexAttribPointer(m_textureLoc, 2, GL_FLOAT, GL_FALSE, 0, EGLTextureCoord);
        glEnableVertexAttribArray(m_textureLoc);

        if (m_textureSize >= 0) {
            GLfloat size[2];
            size[0] = m_width;
            size[1] = m_height;
            glUniform2fv(m_textureSize, 1, &size[0]);
        }

        isProgramChanged = false;
    }

    return m_program;
}

bool
EGLSurfaceViewVideoRender::setSharderPath(const char *vertexPath, const char *fragmentPath) {
    openGlShader->getSharderPath(vertexPath, fragmentPath);
    return 0;
}

bool EGLSurfaceViewVideoRender::setSharderStringPath(string vertexPath, string fragmentPath) {
    openGlShader->getSharderStringPath(vertexPath, fragmentPath);
    return 0;
}

EGLSurfaceViewVideoRender::EGLSurfaceViewVideoRender() {
    openGlShader = new OpenGLShader();
}

EGLSurfaceViewVideoRender::~EGLSurfaceViewVideoRender() {
    deleteTextures();
    delete_program(m_program);
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

    if (openGlShader) {
        delete openGlShader;
        openGlShader = nullptr;
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

void EGLSurfaceViewVideoRender::delete_program(GLuint &program) {
    if (program) {
        glUseProgram(0);
        glDeleteProgram(program);
        program = 0;
    }
}

void EGLSurfaceViewVideoRender::deleteTextures() {
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


/**
 * Looper 消息循环处理
 */
void EGLSurfaceViewVideoRender::handleMessage(LooperMessage *msg) {
    Looper::handleMessage(msg);
    switch (msg->what) {
        case MSG_SurfaceCreated:
            LOGI("EGLSurfaceViewVideoRender [Looper] -> MSG_SurfaceCreated");
            OnSurfaceCreated();
            break;
        case MSG_SurfaceChanged:
            LOGI("EGLSurfaceViewVideoRender [Looper] -> MSG_SurfaceChanged");
            OnSurfaceChanged(msg->arg1, msg->arg2);
            break;
        case MSG_DrawFrame:
            // LOGD("EGLSurfaceViewVideoRender [Looper] -> MSG_DrawFrame");
            OnDrawFrame();
            break;
        case MSG_SurfaceDestroyed:
            LOGI("EGLSurfaceViewVideoRender [Looper] -> MSG_SurfaceDestroyed");
            OnSurfaceDestroyed();
            break;
        default:
            break;
    }
}

/**
 * 初始化 EGL 环境
 */
void EGLSurfaceViewVideoRender::OnSurfaceCreated() {
    LOGD("EGLSurfaceViewVideoRender::OnSurfaceCreated - Initializing EglCore");
    m_EglCore = new EglCore(eglGetCurrentContext(), FLAG_RECORDABLE);
    if (!m_EglCore) {
        LOGE("new EglCore failed!");
        return;
    }

    LOGD("EGLSurfaceViewVideoRender::OnSurfaceCreated - Initializing WindowSurface with ANativeWindow: %p", m_ANWindow);
    m_WindowSurface = new WindowSurface(m_EglCore, m_ANWindow);
    if (!m_WindowSurface) {
        LOGE("new WindowSurface failed!");
        return;
    }
    m_WindowSurface->makeCurrent();
}

/**
 * 处理显示区域适配
 */
void EGLSurfaceViewVideoRender::OnSurfaceChanged(int w, int h) {
    m_backingWidth = w;
    m_backingHeight = h;
    LOGI("EGLSurfaceViewVideoRender::OnSurfaceChanged [w=%d, h=%d]", w, h);

    // 计算缩放适配 (Aspect Ratio)
    float windowAspect = (float) m_backingHeight / (float) m_backingWidth;
    size_t outWidth, outHeight;
    if (VIDEO_HEIGHT > VIDEO_WIDTH * windowAspect) {
        outWidth = VIDEO_WIDTH;
        outHeight = (int) (VIDEO_WIDTH * windowAspect);
    } else {
        outHeight = VIDEO_HEIGHT;
        outWidth = (int) (VIDEO_HEIGHT / windowAspect);
    }

    offX = (VIDEO_WIDTH - outWidth) / 2;
    offY = (VIDEO_HEIGHT - outHeight) / 2;
    off_right = offX + outWidth;
    off_bottom = offY + outHeight;

    LOGD("EGLSurfaceViewVideoRender - Aspect Adjust: offX=%zu, offY=%zu, right=%zu, bottom=%zu", offX, offY, off_right, off_bottom);

    useProgram();
    createTextures();
}

/**
 * 实际渲染循环逻辑
 */
void EGLSurfaceViewVideoRender::OnDrawFrame() {
    if (m_WindowSurface == nullptr) {
        LOGW("EGLSurfaceViewVideoRender::OnDrawFrame - Skip: m_WindowSurface is nullptr");
        return;
    }

    // 1. 清屏
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    // 2. 更新纹理并使用 Shader
    if (!updateTextures() || !useProgram()) return;

    // 3. 执行绘制（窗口显示）
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    // 4. 处理编码录制流程（如果已开启）
    if (m_TextureMovieEncoder2 != nullptr) {
        m_TextureMovieEncoder2->frameAvailableSoon();
    }
    if (m_InputWindowSurface != nullptr) {
        m_InputWindowSurface->makeCurrentReadFrom(*m_WindowSurface);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glBlitFramebuffer(0, 0, m_backingWidth, m_backingHeight, offX, offY, off_right, off_bottom,
                          GL_COLOR_BUFFER_BIT, GL_NEAREST);
        m_InputWindowSurface->swapBuffers();
    }

    // 5. 交换缓冲区，显示到屏幕
    m_WindowSurface->makeCurrent();
    m_WindowSurface->swapBuffers();
}

/**
 * 销毁 EGL 环境和资源
 */
void EGLSurfaceViewVideoRender::OnSurfaceDestroyed() {
    LOGI("EGLSurfaceViewVideoRender::OnSurfaceDestroyed - Cleaning up OpenGL resources");
    m_vertexShader = 0;
    m_pixelShader = 0;

    deleteTextures();
    delete_program(m_program);

    if (m_EglCore) {
        delete m_EglCore;
        m_EglCore = nullptr;
    }

    if (m_WindowSurface) {
        delete m_WindowSurface;
        m_WindowSurface = nullptr;
    }
}

void EGLSurfaceViewVideoRender::printGLString(const char *name, GLenum s) {
    const char *v = (const char *) glGetString(s);
    LOGI("OpenGL %s = %s\n", name, v);
}

void EGLSurfaceViewVideoRender::checkGlError(const char *op) {
    for (GLint error = glGetError(); error; error = glGetError()) {
        LOGI("after %s() glError (0x%x)\n", op, error);
    }
}

void EGLSurfaceViewVideoRender::startEncoder(const char *recordPath) {
    LOGD("EGLSurfaceViewVideoRender::startEncoder()");
    m_VideoEncoderCore = new VideoEncoderCore(VIDEO_WIDTH, VIDEO_HEIGHT, BIT_RATE, recordPath);
    m_InputWindowSurface = new WindowSurface(m_EglCore, m_VideoEncoderCore->getInputSurface());
    m_TextureMovieEncoder2 = new TextureMovieEncoder2(m_VideoEncoderCore);
}

void EGLSurfaceViewVideoRender::stopEncoder() {
    LOGD("EGLSurfaceViewVideoRender::stopEncoder()");
    if (m_TextureMovieEncoder2 != nullptr) {
        m_TextureMovieEncoder2->stopRecording();
    }
    if (m_VideoEncoderCore != nullptr) {
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
