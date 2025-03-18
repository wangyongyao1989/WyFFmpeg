
#include <jni.h>
#include <string>
#include <android/log.h>

#include "h265_bitstream_parser.h"
#include "h265_common.h"
#include "bit_buffer.h"

//  Author : wangyongyao https://github.com/wangyongyao1989
// Created by MMM on 2025/3/18.
//

#define LOG_TAG "wy"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
using namespace std;
//包名+类名字符串定义：
const char *java_call_jni_class = "com/wangyongyao/h265/H265CallJni";
using namespace h265nal;


typedef struct arg_options {
    int debug;
    bool as_one_line;
    bool add_offset;
    bool add_length;
    bool add_parsed_length;
    bool add_checksum;
    bool add_contents;
    char *infile;
    char *outfile;
} arg_options;


extern "C"
JNIEXPORT void JNICALL
cpp_test_h265(JNIEnv *env, jobject thiz) {
    LOGE("cpp_test_h265=====");
    arg_options *options;


    
}


static const JNINativeMethod methods[] = {
        {"n_test_h265",            "()V",                 (void *) cpp_test_h265},
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