#include <jni.h>
#include <string>
#include <android/log.h>
#include "RtmpPusherManger.h"

#define LOG_TAG "wy"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

//包名+类名字符串定义：
const char *rtmp_class_name = "com/example/rtmplive/RtmpLivePusher";

RtmpPusherManger *rtmpManger;

extern "C"
JNIEXPORT void JNICALL
cpp_init_rtmp_callback(JNIEnv *env, jobject thiz) {
    if (rtmpManger == nullptr)
        rtmpManger = new RtmpPusherManger();
    rtmpManger->initCallBack(env, thiz);

}

extern "C"
JNIEXPORT void JNICALL
cpp_init_rtmp(JNIEnv *env, jobject thiz) {
    if (rtmpManger == nullptr)
        rtmpManger = new RtmpPusherManger();
    rtmpManger->init_rtmp(env, thiz);
    rtmpManger->initCallBack(env, thiz);
}

extern "C"
JNIEXPORT void JNICALL
cpp_start_rtmp(JNIEnv *env, jobject thiz, jstring intputUrl) {
    const char *url = env->GetStringUTFChars(intputUrl, 0);
    char *path = new char[strlen(url) + 1];
    strcpy(path,url);
    if (rtmpManger != nullptr) {
        rtmpManger->start_rtmp(path);
    }

    env->ReleaseStringUTFChars(intputUrl, url);
}

extern "C"
JNIEXPORT void JNICALL
cpp_stop_rtmp(JNIEnv *env, jobject thiz) {
    if (rtmpManger != nullptr) {
        rtmpManger->stop_rtmp();
    }
}

extern "C"
JNIEXPORT void JNICALL
cpp_pause_rtmp(JNIEnv *env, jobject thiz) {
    if (rtmpManger != nullptr) {
        rtmpManger->pause_rtmp();
    }
}

extern "C"
JNIEXPORT void JNICALL
cpp_release_rtmp(JNIEnv *env, jobject thiz) {
    if (rtmpManger != nullptr) {
        rtmpManger->release_rtmp();
        delete rtmpManger;
    }

    rtmpManger = 0;
}

static const JNINativeMethod methods[] = {
        {"native_init_callback",   "()V",                   (void *) cpp_init_rtmp_callback},
        {"native_rtmp_init",       "()V",                   (void *) cpp_init_rtmp},
        {"native_rtmp_start_push", "(Ljava/lang/String;)V", (void *) cpp_start_rtmp},
        {"native_rtmp_stop_push",  "()V",                   (void *) cpp_stop_rtmp},
        {"native_rtmp_pause",      "()V",                   (void *) cpp_pause_rtmp},
        {"native_rtmp_release",    "()V",                   (void *) cpp_release_rtmp},


};

// 定义注册方法
JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved) {
    LOGD("动态注册");
    JNIEnv *env;
    if ((vm)->GetEnv((void **) &env, JNI_VERSION_1_6) != JNI_OK) {
        LOGD("动态注册GetEnv  fail");
        return JNI_ERR;
    }

    // 获取类引用
    jclass clazz = env->FindClass(rtmp_class_name);

    // 注册native方法
    jint regist_result = env->RegisterNatives(clazz, methods,
                                              sizeof(methods) / sizeof(methods[0]));
    if (regist_result) { // 非零true 进if
        LOGE("动态注册 fail regist_result = %d", regist_result);
    } else {
        LOGI("动态注册 success result = %d", regist_result);
    }
    return JNI_VERSION_1_6;
}

