
#include <jni.h>
#include <string>
#include <android/log.h>

#include "h265_bitstream_parser.h"
#include "h265_common.h"
#include "bit_buffer.h"

#include "HevcLogUtils.h"
#include "HevcNalParse.h"
#include "Hevc2MP4.h"

//  Author : wangyongyao https://github.com/wangyongyao1989
// Created by MMM on 2025/3/18.
//

using namespace std;
//包名+类名字符串定义：
const char *java_call_jni_class = "com/wangyongyao/h265/H265CallJni";
using namespace h265nal;


HevcNalParse *hevcNalParse;
Hevc2MP4 *hevc2MP4;


extern "C"
JNIEXPORT void JNICALL
cpp_test_h265(JNIEnv *env, jobject thiz, jstring dataPath) {
    const char *cDataPath = env->GetStringUTFChars(dataPath, nullptr);
    LOGE("cpp_test_h265=====%s", cDataPath);

    if (hevcNalParse == nullptr) {
        hevcNalParse = new HevcNalParse();
    }

    hevcNalParse->setHevcNalDataPath(cDataPath);

    env->ReleaseStringUTFChars(dataPath, cDataPath);

}

extern "C"
JNIEXPORT void JNICALL
cpp_hevc_to_mp4(JNIEnv *env, jobject thiz, jstring inPath, jstring outPath) {
    const char *cInPath = env->GetStringUTFChars(inPath, nullptr);
    const char *cOutPath = env->GetStringUTFChars(outPath, nullptr);

    LOGE("cpp_hevc_to_mp4 cInPath:%s", cInPath);
    LOGE("cpp_hevc_to_mp4 cOutPath:%s", cOutPath);
    if (hevc2MP4 == nullptr) {
        hevc2MP4 = new Hevc2MP4();
    }

    hevc2MP4->hevcConverterMp4(cInPath, cOutPath);


    env->ReleaseStringUTFChars(inPath, cInPath);
    env->ReleaseStringUTFChars(outPath, cOutPath);

}


static const JNINativeMethod methods[] = {
        {"n_test_h265",   "(Ljava/lang/String;)V", (void *) cpp_test_h265},
        {"n_hevc_to_mp4", "(Ljava/lang/String;"
                          "Ljava/lang/String;)V",  (void *) cpp_hevc_to_mp4},

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