////  Author : wangyongyao https://github.com/wangyongyao1989
//// Created by MMM on 2024/10/11.
////
//
//#ifndef MYYFFMPEG_GLTHREAD_H
//#define MYYFFMPEG_GLTHREAD_H
//#include <android/native_window.h>
//#include <thread>
//#include <memory>
//#include <cstdint>
//#include <jni.h>
//#include <EGL/egl.h>
//#include <GLES3/gl3.h>
//
//
//class GLThread {
//private:
//    const char *TAG = "GLRender";         //OpenGL渲染状态
//
//    enum STATE {
//        NO_SURFACE,         //没有有效的surface
//        FRESH_SURFACE,      //持有一个为初始化的新的surface
//        RENDERING,          //初始化终了，能够开端渲染
//        SURFACE_DESTROY,    //surface销毁
//        STOP                //中止绘制
//    };
//    JNIEnv *m_env = NULL;         //线程依附的jvm环境
//    JavaVM *m_jvm_for_thread = NULL;         //Surface援用，必需要运用援用，否则无法在线程中操作
//    jobject m_surface_ref = NULL;         //本地屏幕
//    ANativeWindow *m_native_window = NULL;         //EGL显现外表
////    EglSurface *m_egl_surface = NULL;
//    int m_window_width = 0;
//    int m_window_height = 0;                  // 绘制代理器
////    ImageRender *pImageRender;                  //OpenGL渲染状态
//    STATE m_state = NO_SURFACE;// 初始化相关的办法
//    void InitRenderThread();
//
//    bool InitEGL();
//
//    void InitDspWindow(JNIEnv *env);
//
//    // 创立/销毁
//    Surface void CreateSurface();
//
//    void DestroySurface();
//
//    // 渲染办法
//    void Render();
//
//    void ReleaseSurface();
//
//    void ReleaseWindow();
//
//    // 渲染线程回调办法
//    static void sRenderThread(std::shared_ptr that);
//
//public:
//    GLThread(JNIEnv *env);
//
//    ~GLThread();         //外部传入Surface
//    void SetSurface(jobject surface);
//
//    void Stop();
//
////    void SetBitmapRender(ImageRender *bitmapRender);
//
//    // 释放资源相关办法
//    void ReleaseRender();
//
////    ImageRender *GetImageRender();
//};
//
//
//#endif //MYYFFMPEG_GLTHREAD_H
