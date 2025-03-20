#include <jni.h>
#include <string>
#include <android/log.h>

#include "h264include/H264LogUtils.h"

//  Author : wangyongyao https://github.com/wangyongyao1989
// Created by MMM on 2025/3/20.
//

using namespace std;
//包名+类名字符串定义：
const char *java_call_jni_class = "com/wangyongyao/h264/H264CallJni";

extern "C"
JNIEXPORT void JNICALL
cpp_test_h264(JNIEnv *env, jobject thiz, jstring dataPath) {
    const char *cDataPath = env->GetStringUTFChars(dataPath, nullptr);
    LOGE("cpp_test_h264=====%s", cDataPath);


    env->ReleaseStringUTFChars(dataPath, cDataPath);

}

static const JNINativeMethod methods[] = {
        {"n_test_h264", "(Ljava/lang/String;)V", (void *) cpp_test_h264},
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
    jclass clazz = env->FindClass(java_call_jni_class);

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

extern "C" void JNI_OnUnload(JavaVM *jvm, void *p) {
    JNIEnv *env = NULL;
    if (jvm->GetEnv((void **) (&env), JNI_VERSION_1_6) != JNI_OK) {
        return;
    }
    jclass clazz = env->FindClass(java_call_jni_class);
    if (env != NULL) {
        env->UnregisterNatives(clazz);
    }

}