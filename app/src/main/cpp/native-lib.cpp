#include <jni.h>
#include <string>
#include "getopt.h"
#include "gperf.h"
#include <android/log.h>
#include "FFmpegManger.h"

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


JNIEXPORT jlong JNICALL
cpp_initFFmpeg(JNIEnv *env, jobject thiz) {
    LOGD("cpp_initFFmpeg");
    FFmpegManger *fmpegManger = new FFmpegManger();
    fmpegManger->initFFmpeg();
    return 10;
}

JNIEXPORT jstring JNICALL
cpp_getFFmpegVersion(JNIEnv *env, jobject thiz) {
    char strBuffer[1024 * 4] = {0};
    strcat(strBuffer, "libavcodec : ");
    strcat(strBuffer, AV_STRINGIFY(LIBAVCODEC_VERSION));
    strcat(strBuffer, "\nlibavformat : ");
    strcat(strBuffer, AV_STRINGIFY(LIBAVFORMAT_VERSION));
    strcat(strBuffer, "\nlibavutil : ");
    strcat(strBuffer, AV_STRINGIFY(LIBAVUTIL_VERSION));
    strcat(strBuffer, "\nlibavfilter : ");
    strcat(strBuffer, AV_STRINGIFY(LIBAVFILTER_VERSION));
    strcat(strBuffer, "\nlibswresample : ");
    strcat(strBuffer, AV_STRINGIFY(LIBSWRESAMPLE_VERSION));
    strcat(strBuffer, "\nlibswscale : ");
    strcat(strBuffer, AV_STRINGIFY(LIBSWSCALE_VERSION));
    strcat(strBuffer, "\navcodec_configure : \n");
    strcat(strBuffer, avcodec_configuration());
    strcat(strBuffer, "\navcodec_license : ");
    strcat(strBuffer, avcodec_license());
    LOGD("GetFFmpegVersion\n%s", strBuffer);
    return env->NewStringUTF(strBuffer);
}


extern "C"
JNIEXPORT void JNICALL
cpp_set_input_url(JNIEnv *env, jobject thiz, jstring intputUrl) {
    const char *url = env->GetStringUTFChars(intputUrl, 0);

    env->ReleaseStringUTFChars(intputUrl, url);
}

extern "C"
JNIEXPORT void JNICALL
cpp_mp4_input_avi_output(JNIEnv *env, jobject thiz, jstring intputUrl, jstring outputUrl) {
    const char *intput_url = env->GetStringUTFChars(intputUrl, 0);
    const char *output_url = env->GetStringUTFChars(outputUrl, 0);
    FFmpegManger *fmpegManger = new FFmpegManger();
    fmpegManger->mp4ConversionAvi(intput_url, output_url);

    env->ReleaseStringUTFChars(intputUrl, intput_url);
    env->ReleaseStringUTFChars(outputUrl, output_url);
}

extern "C"
JNIEXPORT void JNICALL
cpp_mp4_water_mark(JNIEnv *env, jobject thiz, jstring intputUrl, jstring pngUlr,
                   jstring outputUrl) {
    const char *intput_url = env->GetStringUTFChars(intputUrl, 0);
    const char *png_url = env->GetStringUTFChars(pngUlr, 0);
    const char *output_url = env->GetStringUTFChars(outputUrl, 0);
    FFmpegManger *fmpegManger = new FFmpegManger();
    fmpegManger->mp4WaterMark(intput_url, png_url, output_url);
    env->ReleaseStringUTFChars(intputUrl, intput_url);
    env->ReleaseStringUTFChars(pngUlr, png_url);
    env->ReleaseStringUTFChars(outputUrl, output_url);
}

extern "C"
JNIEXPORT void JNICALL
cpp_play_audio(JNIEnv *env, jobject thiz, jstring intputUrl) {
    const char *url = env->GetStringUTFChars(intputUrl, 0);
    FFmpegManger *fmpegManger = new FFmpegManger();
    fmpegManger->playAudio(url);

    env->ReleaseStringUTFChars(intputUrl, url);
}


extern "C"
JNIEXPORT void JNICALL
cpp_stop_audio(JNIEnv *env, jobject thiz) {
    FFmpegManger *fmpegManger = new FFmpegManger();

    fmpegManger->stopAudio();

}

extern "C"
JNIEXPORT void JNICALL
cpp_play_video(JNIEnv *env, jobject thiz, jstring intputUrl, jobject surface) {
    const char *url = env->GetStringUTFChars(intputUrl, 0);
    LOGD("cpp_play_video url:%s", url);
    FFmpegManger *fmpegManger = new FFmpegManger();
    fmpegManger->playVideo(env, surface, url);

    env->ReleaseStringUTFChars(intputUrl, url);
}


extern "C"
JNIEXPORT void JNICALL
cpp_stop_video(JNIEnv *env, jobject thiz) {
    FFmpegManger *fmpegManger = new FFmpegManger();

    fmpegManger->stopAudio();

}

// 重点：定义类名和函数签名，如果有多个方法要动态注册，在数组里面定义即可
static const JNINativeMethod methods[] = {
        {"native_getFFmpegVersion", "()Ljava/lang/String;",                                      (void *) cpp_getFFmpegVersion},
        {"native_initFFmpeg",       "()J",                                                       (long *) cpp_initFFmpeg},
        {"stringFromJNI",           "()Ljava/lang/String;",                                      (std::string *) cpp_stringFromJNI},
        {"intFromJNI",              "(I)I",                                                      (void *) cpp_intFromJNI},
        {"setInputUrl",             "(Ljava/lang/String;)V",                                     (void *) cpp_set_input_url},
        {"native_MP4_AVI",          "(Ljava/lang/String;Ljava/lang/String;)V",                   (void *) cpp_mp4_input_avi_output},
        {"native_Water_mark",       "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V", (void *) cpp_mp4_water_mark},
        {"native_Play_Audio",       "(Ljava/lang/String;)V",                                     (void *) cpp_play_audio},
        {"native_Stop_Audio",       "()V",                                                       (void *) cpp_stop_audio},
        {"native_Play_Video",       "(Ljava/lang/String;Landroid/view/Surface;)V",               (void *) cpp_play_video},
        {"native_Stop_Video",       "()V",                                                       (void *) cpp_stop_video},

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

