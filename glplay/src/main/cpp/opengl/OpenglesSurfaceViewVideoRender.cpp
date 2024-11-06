//  Author : wangyongyao https://github.com/wangyongyao1989
// Created by MMM on 2024/9/5.
//

#include "OpenglesSurfaceViewVideoRender.h"
#include "OpenGLShader.h"

void
OpenglesSurfaceViewVideoRender::init(ANativeWindow *window, AAssetManager *assetManager,
                                     size_t width,
                                     size_t height) {
    LOGI("OpenglesSurfaceViewVideoRender init==%d, %d", width, height);
    m_backingWidth = width;
    m_backingHeight = height;
    ///EGL
    //1 EGL display创建和初始化
    display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (display == EGL_NO_DISPLAY) {
        LOGE("eglGetDisplay failed!");
        return;
    }
    if (EGL_TRUE != eglInitialize(display, 0, 0)) {
        LOGE("eglInitialize failed!");
        return;
    }
    //2 surface
    //2-1 surface窗口配置
    //输出配置
    EGLConfig config;
    EGLint configNum;
    EGLint configSpec[] = {
            EGL_RED_SIZE, 8,
            EGL_GREEN_SIZE, 8,
            EGL_BLUE_SIZE, 8,
            EGL_SURFACE_TYPE, EGL_WINDOW_BIT, EGL_NONE
    };
    if (EGL_TRUE != eglChooseConfig(display, configSpec, &config, 1, &configNum)) {
        LOGE("eglChooseConfig failed!");
        return;
    }
    //创建surface
    ANativeWindow_acquire(window);
    ANativeWindow_setBuffersGeometry(window, 0, 0, AHARDWAREBUFFER_FORMAT_R8G8B8A8_UNORM);
    winsurface = eglCreateWindowSurface(display, config, window, 0);
    if (winsurface == EGL_NO_SURFACE) {
        LOGE("eglCreateWindowSurface failed!");
        return;
    }

    //3 context 创建关联的上下文
    const EGLint ctxAttr[] = {
            EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE
    };
    EGLContext context = eglCreateContext(display, config, EGL_NO_CONTEXT, ctxAttr);
    if (context == EGL_NO_CONTEXT) {
        LOGE("eglCreateContext failed!");
        return;
    }
    if (EGL_TRUE != eglMakeCurrent(display, winsurface, winsurface, context)) {
        LOGE("eglMakeCurrent failed!");
        return;
    }

    useProgram();
    createTextures();

}

void OpenglesSurfaceViewVideoRender::render() {
//    LOGI("OpenglesSurfaceViewVideoRender render");

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    if (!updateTextures() /*|| !useProgram()*/) return;

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    //窗口显示
    eglSwapBuffers(display, winsurface);
}

void OpenglesSurfaceViewVideoRender::release() {
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

}

void OpenglesSurfaceViewVideoRender::updateFrame(const surface_video_frame &frame) {
    m_sizeY = frame.width * frame.height;
    m_sizeU = frame.width * frame.height / 4;
    m_sizeV = frame.width * frame.height / 4;

    if (m_pDataY == nullptr || m_width != frame.width || m_height != frame.height) {
        m_pDataY = std::make_unique<uint8_t[]>(m_sizeY + m_sizeU + m_sizeV);
        m_pDataU = m_pDataY.get() + m_sizeY;
        m_pDataV = m_pDataU + m_sizeU;
        isProgramChanged = true;
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
OpenglesSurfaceViewVideoRender::draw(uint8_t *buffer, size_t length, size_t width, size_t height,
                                     float rotation) {
    m_length = length;
    m_rotation = rotation;

    surface_video_frame frame{};
    frame.width = width;
    frame.height = height;
    frame.stride_y = width;
    frame.stride_uv = width / 2;
    frame.y = buffer;
    frame.u = buffer + width * height;
    frame.v = buffer + width * height * 5 / 4;

    updateFrame(frame);
}

void OpenglesSurfaceViewVideoRender::setParameters(uint32_t params) {
    m_params = params;
}

uint32_t OpenglesSurfaceViewVideoRender::getParameters() {
    return m_params;
}

bool OpenglesSurfaceViewVideoRender::createTextures() {
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

bool OpenglesSurfaceViewVideoRender::updateTextures() {
    if (!m_textureIdY && !m_textureIdU && !m_textureIdV /*&& !createYUVTextures()*/) return false;
//    LOGI("OpenglesSurfaceViewVideoRender updateTextures");
    LOGE("updateTextures m_textureIdY:%d,m_textureIdU:%d,m_textureIdV:%d,===isDirty:%d",
         m_textureIdY,
         m_textureIdU, m_textureIdV, isDirty);

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

int
OpenglesSurfaceViewVideoRender::createProgram() {

    m_program = openGlShader->createProgram();
    m_vertexShader = openGlShader->vertexShader;
    m_pixelShader = openGlShader->fraShader;
    LOGI("OpenglesSurfaceViewVideoRender createProgram m_program:%d", m_program);

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

GLuint OpenglesSurfaceViewVideoRender::useProgram() {
    if (!m_program && !createProgram()) {
        LOGE("Could not use program.");
        return 0;
    }

    if (isProgramChanged) {
        glUseProgram(m_program);
        glVertexAttribPointer(m_vertexPos, 2, GL_FLOAT, GL_FALSE, 0, kVerticekSurface);
        glEnableVertexAttribArray(m_vertexPos);

        glUniform1i(m_textureYLoc, 0);
        glUniform1i(m_textureULoc, 1);
        glUniform1i(m_textureVLoc, 2);
        glVertexAttribPointer(m_textureLoc, 2, GL_FLOAT, GL_FALSE, 0, kTextureCoordSurface);
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
OpenglesSurfaceViewVideoRender::setSharderPath(const char *vertexPath, const char *fragmentPath) {
    openGlShader->getSharderPath(vertexPath, fragmentPath);
    return 0;
}

bool OpenglesSurfaceViewVideoRender::setSharderStringPath(string vertexPath, string fragmentPath) {
    openGlShader->getSharderStringPath(vertexPath, fragmentPath);
    return 0;
}

OpenglesSurfaceViewVideoRender::OpenglesSurfaceViewVideoRender() {
    openGlShader = new OpenGLShader();
}

OpenglesSurfaceViewVideoRender::~OpenglesSurfaceViewVideoRender() {
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
}

void OpenglesSurfaceViewVideoRender::delete_program(GLuint &program) {
    if (program) {
        glUseProgram(0);
        glDeleteProgram(program);
        program = 0;
    }
}

void OpenglesSurfaceViewVideoRender::deleteTextures() {
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

void OpenglesSurfaceViewVideoRender::printGLString(const char *name, GLenum s) {
    const char *v = (const char *) glGetString(s);
    LOGI("OpenGL %s = %s\n", name, v);
}

void OpenglesSurfaceViewVideoRender::checkGlError(const char *op) {
    for (GLint error = glGetError(); error; error = glGetError()) {
        LOGI("after %s() glError (0x%x)\n", op, error);
    }
}