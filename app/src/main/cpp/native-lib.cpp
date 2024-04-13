#include <jni.h>
#include <string>
#include "getopt.h"
#include "gperf.h"
#include <android/log.h>

#define LOG_TAG "wy"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

//包名+类名字符串定义：
const char *mainactivity_class_name = "com/example/myyffmpeg/CallJni";

extern "C" JNIEXPORT jstring JNICALL
cpp_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    LOGD("cpp_stringFromJNI  hello = %c", hello.c_str());
    return env->NewStringUTF(hello.c_str());
}


extern "C"
JNIEXPORT jint JNICALL
cpp_intFromJNI(JNIEnv *env, jobject thiz, jint age) {
    LOGD("cpp_intFromJNI  age = %d", age);
    return age + 10;
}


// 重点：定义类名和函数签名，如果有多个方法要动态注册，在数组里面定义即可
static const JNINativeMethod methods[] = {
        {"stringFromJNI", "()Ljava/lang/String;", (void *) cpp_stringFromJNI},
        {"intFromJNI",    "(I)I",                 (void *) cpp_intFromJNI},
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
    jclass clazz = env->FindClass(mainactivity_class_name);

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

