////  Author : wangyongyao https://github.com/wangyongyao1989
//// Created by MMM on 2024/10/11.
////
//
//#include "../includeopengl/GLThread.h"
//
////结构函数
//GLThread::GLThread(JNIEnv *env) {
//    this->m_env = env;     //获取JVM虚拟机，为创立线程作准备
//    env->GetJavaVM(&m_jvm_for_thread);
//    InitRenderThread();
//}
//
////析构函数
//GLThread::~GLThread() {
//    delete m_egl_surface;
//}
//
////初始化渲染线程
//void GLThread::InitRenderThread() {
//    // 运用智能指针，线程完毕时，自动删除本类指针
//
//    std::shared_ptr that(this);
//    std::thread t(sRenderThread, that);
//    t.detach();
//}
//
////线程回调函数
//void GLThread::sRenderThread(std::shared_ptr that) {
//    JNIEnv *env;     //(1) 将线程附加到虚拟机，并获取env
//    if (that->m_jvm_for_thread->AttachCurrentThread(&env, NULL) != JNI_OK) {
//        LOGE(that->TAG, "线程初始化异常");
//        return;
//    }
//    // (2) 初始化 EGL
//    if (!that->InitEGL()) {
//        //解除线程和jvm关联
//        that->m_jvm_for_thread->DetachCurrentThread();
//        return;
//    }
//    //进入循环
//    while (true) {
//        //依据OpenGL渲染状态进入不同的处置
//        switch (that->m_state) {
//            //刷新Surface，从外面设置Surface后m_state置为该状态，阐明曾经从外部(java层)取得Surface的对象了
//            case FRESH_SURFACE:
//                LOGI(that->TAG, "Loop Render FRESH_SURFACE");
//                // (3) 初始化Window
//                that->InitDspWindow(env);
//                // (4) 创立EglSurface
//                that->CreateSurface();
//                // m_state置为RENDERING状态进入渲染
//                that->m_state = RENDERING;
//                break;
//            case RENDERING:
//                LOGI(that->TAG, "Loop Render RENDERING");
//                // (5) 渲染
//                that->Render();
//                break;
//            case STOP:
//                LOGI(that->TAG, "Loop Render STOP");
//                //(6) 解除线程和jvm关联
//                that->ReleaseRender();
//                that->m_jvm_for_thread->DetachCurrentThread();
//                return;
//            case SURFACE_DESTROY:
//                LOGI(that->TAG, "Loop Render SURFACE_DESTROY");
//                //(7) 释放资源
//                that->DestroySurface();
//                that->m_state = NO_SURFACE;
//                break;
//            case NO_SURFACE:
//            default:
//                break;
//        }
////        usleep(20000);
//    }
//
//
//}