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
const char *java_call_jni_class = "com/example/rtmplive/RtmpLivePusher";

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
    rtmpManger->initVideoPacket();
    rtmpManger->initAudioPacket();
}

extern "C"
JNIEXPORT void JNICALL
cpp_start_rtmp(JNIEnv *env, jobject thiz, jstring intputUrl) {
    const char *url = env->GetStringUTFChars(intputUrl, 0);
    char *path = new char[strlen(url) + 1];
    strcpy(path, url);
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

extern "C"
JNIEXPORT void JNICALL
cpp_push_video_data(JNIEnv *env, jobject thiz, jbyteArray yuv) {
    if (rtmpManger == nullptr) return;
    jbyte *yuv_plane = env->GetByteArrayElements(yuv, JNI_FALSE);
    rtmpManger->encodeVideoPacket(yuv_plane);
    env->ReleaseByteArrayElements(yuv, yuv_plane, 0);

}

extern "C"
JNIEXPORT void JNICALL
cpp_video_codec_info(JNIEnv *env, jobject thiz, jint width, jint height, jint fps, jint bitrate) {
    if (rtmpManger == nullptr) return;
    rtmpManger->setVideoEncInfo(width, height, fps, bitrate);
}

extern "C"
JNIEXPORT jint JNICALL
cpp_audio_get_input_samples(JNIEnv *env, jobject thiz) {
    if (rtmpManger == nullptr) return -1;
    return rtmpManger->getAudioInputSample();
}

extern "C"
JNIEXPORT void JNICALL
cpp_audio_set_audio_codec_info(JNIEnv *env, jobject thiz, jint sampleRateInHz, jint channels) {
    if (rtmpManger == nullptr) return;
    rtmpManger->setAudioCodecInfo(sampleRateInHz, channels);
    return;
}

extern "C"
JNIEXPORT void JNICALL
cpp_push_audio_data(JNIEnv *env, jobject thiz, jbyteArray audioData) {
    if (rtmpManger == nullptr) return;
    jbyte *audio_data = env->GetByteArrayElements(audioData, JNI_FALSE);
    rtmpManger->encodeAudioPacket(audio_data);
    env->ReleaseByteArrayElements(audioData, audio_data, 0);

}

static const JNINativeMethod methods[] = {
        {"native_init_callback",     "()V",                   (void *) cpp_init_rtmp_callback},
        {"native_rtmp_init",         "()V",                   (void *) cpp_init_rtmp},
        {"native_rtmp_start_push",   "(Ljava/lang/String;)V", (void *) cpp_start_rtmp},
        {"native_rtmp_stop_push",    "()V",                   (void *) cpp_stop_rtmp},
        {"native_rtmp_pause",        "()V",                   (void *) cpp_pause_rtmp},
        {"native_rtmp_release",      "()V",                   (void *) cpp_release_rtmp},

        {"native_push_video_data",   "([B)V",                 (void *) cpp_push_video_data},
        {"native_video_codec_info",  "(IIII)V",               (void *) cpp_video_codec_info},

        {"native_getInputSamples",   "()I",                   (void *) cpp_audio_get_input_samples},
        {"native_setAudioCodecInfo", "(II)V",                 (void *) cpp_audio_set_audio_codec_info},
        {"native_push_audio_data",   "([B)V",                 (void *) cpp_push_audio_data},


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

