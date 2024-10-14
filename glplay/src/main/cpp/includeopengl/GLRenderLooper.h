//  Author : wangyongyao https://github.com/wangyongyao1989
// Created by MMM on 2024/10/14.
//

#ifndef MYYFFMPEG_GLRENDERLOOPER_H
#define MYYFFMPEG_GLRENDERLOOPER_H
#include "Looper.h"
#include <GLES3/gl3.h>
#include <thread>
#include <EGL/egl.h>
#include "EglCore.h"
#include "OffscreenSurface.h"

using namespace std;

//enum {
//    MSG_SurfaceCreated,
//    MSG_SurfaceChanged,
//    MSG_DrawFrame,
//    MSG_SurfaceDestroyed,
//};

typedef void (*RenderDoneCallback)(void*, int);

//struct GLEnv {
//    GLuint inputTexId;
//    GLuint program;
//    GLuint vboIds[3];
//    EGLContext sharedCtx;
//    RenderDoneCallback renderDone;
//    void* callbackCtx;
//};

class GLRenderLooper : public Looper {
public:
    GLRenderLooper();
    virtual ~GLRenderLooper();

//    static GLRenderLooper* GetInstance();
//    static void ReleaseInstance();

private:
    virtual void handleMessage(LooperMessage *msg);

    virtual void OnSurfaceCreated() = 0;
    virtual void OnSurfaceChanged(int w, int h) = 0;
    virtual void OnDrawFrame() = 0;
    virtual void OnSurfaceDestroyed() = 0;

    virtual bool CreateFrameBufferObj() = 0;

private:
//    static mutex m_Mutex;
//    static GLRenderLooper* m_Instance;
//
//    GLEnv *m_GLEnv;
//    EglCore *m_EglCore = nullptr;
//    OffscreenSurface *m_OffscreenSurface = nullptr;
//    GLuint m_VaoId;
//    GLuint m_FboTextureId;
//    GLuint m_FboId;
//    int m_FrameIndex = 0;
};
#endif //MYYFFMPEG_GLRENDERLOOPER_H
