
#include "../includeopengl/GLRenderLooper.h"

//mutex GLRenderLooper::m_Mutex;
//GLRenderLooper* GLRenderLooper::m_Instance = nullptr;

GLRenderLooper::~GLRenderLooper() {

}

GLRenderLooper::GLRenderLooper() {

}

void GLRenderLooper::handleMessage(LooperMessage *msg) {
    Looper::handleMessage(msg);
    switch (msg->what) {
        case MSG_SurfaceCreated: {
            LOGE("GLRenderLooper::handleMessage MSG_SurfaceCreated");
            OnSurfaceCreated();
        }
            break;
        case MSG_SurfaceChanged:
            LOGE("GLRenderLooper::handleMessage MSG_SurfaceChanged");
            OnSurfaceChanged(msg->arg1, msg->arg2);
            break;
        case MSG_DrawFrame:
            LOGE("GLRenderLooper::handleMessage MSG_DrawFrame");
            OnDrawFrame();
            break;
        case MSG_SurfaceDestroyed:
            LOGE("GLRenderLooper::handleMessage MSG_SurfaceDestroyed");
            OnSurfaceDestroyed();
            break;
        default:
            break;
    }
}

//GLRenderLooper *GLRenderLooper::GetInstance() {
//    LOGE("GLRenderLooper::GetInstance");
//    if(m_Instance == nullptr) {
//        unique_lock<mutex> lock(m_Mutex);
//        if(m_Instance == nullptr) {
//            m_Instance = new GLRenderLooper();
//        }
//    }
//
//    return m_Instance;
//}
//
//void GLRenderLooper::ReleaseInstance() {
//    LOGE("GLRenderLooper::GetInstance");
//    if(m_Instance != nullptr) {
//        unique_lock<mutex> lock(m_Mutex);
//        if(m_Instance != nullptr) {
//            delete m_Instance;
//            m_Instance = nullptr;
//        }
//    }
//}

//void GLRenderLooper::OnSurfaceCreated() {
//
//}
//
//void GLRenderLooper::OnSurfaceChanged(int w, int h) {
//    LOGE("GLRenderLooper::OnSurfaceChanged [w,h]=[%d, %d]", w, h);
//
//}
//
//void GLRenderLooper::OnDrawFrame() {
//
//}
//
//void GLRenderLooper::OnSurfaceDestroyed() {
//
//}

bool GLRenderLooper::CreateFrameBufferObj() {

    return true;
}


