#include <jni.h>
#include <string>
#include "getopt.h"
#include "gperf.h"
#include <android/log.h>
#include "FFmpegManger.h"
#include "OpenglesFlashLight.h"
#include "OpenglesCameraPre.h"

#define LOG_TAG "wy"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

//包名+类名字符串定义：
const char *mainactivity_class_name = "com/example/myyffmpeg/FFPlayCallJni";
FFmpegManger *fmpegManger = nullptr;
OpenglesFlashLight *flashLight = nullptr;
OpenglesCameraPre *cameraPre = nullptr;


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
    if (fmpegManger == nullptr)
        fmpegManger = new FFmpegManger();
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
    if (fmpegManger == nullptr)
        fmpegManger = new FFmpegManger();
    fmpegManger->mp4WaterMark(intput_url, png_url, output_url);
    env->ReleaseStringUTFChars(intputUrl, intput_url);
    env->ReleaseStringUTFChars(pngUlr, png_url);
    env->ReleaseStringUTFChars(outputUrl, output_url);
}

extern "C"
JNIEXPORT void JNICALL
cpp_play_audio(JNIEnv *env, jobject thiz, jstring intputUrl) {
    const char *url = env->GetStringUTFChars(intputUrl, 0);
    if (fmpegManger == nullptr)
        fmpegManger = new FFmpegManger();
    fmpegManger->playAudio(url);

    env->ReleaseStringUTFChars(intputUrl, url);
}


extern "C"
JNIEXPORT void JNICALL
cpp_stop_audio(JNIEnv *env, jobject thiz) {
    if (fmpegManger == nullptr)
        fmpegManger = new FFmpegManger();

    fmpegManger->stopAudio();

}

extern "C"
JNIEXPORT void JNICALL
cpp_play_video(JNIEnv *env, jobject thiz) {
    if (fmpegManger == nullptr)
        fmpegManger = new FFmpegManger();
    fmpegManger->playVideo();
}

extern "C"
JNIEXPORT void JNICALL
cpp_play_init(JNIEnv *env, jobject thiz, jstring intputUrl, jobject surface) {
    const char *url = env->GetStringUTFChars(intputUrl, 0);
    if (fmpegManger == nullptr)
        fmpegManger = new FFmpegManger();
    fmpegManger->initFFmpeg(env, thiz, url, surface);

    env->ReleaseStringUTFChars(intputUrl, url);

}

extern "C"
JNIEXPORT void JNICALL
cpp_play_uninit(JNIEnv *env, jobject thiz) {
    if (fmpegManger == nullptr)
        return;
    fmpegManger->unInit();
}


extern "C"
JNIEXPORT void JNICALL
cpp_stop_video(JNIEnv *env, jobject thiz) {
    if (fmpegManger == nullptr)
        return;
    fmpegManger->stopVideo();
}

extern "C"
JNIEXPORT void JNICALL
cpp_pause_video(JNIEnv *env, jobject thiz) {
    if (fmpegManger == nullptr)
        return;
    fmpegManger->pauseVideo();

}

extern "C"
JNIEXPORT void JNICALL
cpp_seek_to_position(JNIEnv *env, jobject thiz, jfloat position) {
    if (fmpegManger == nullptr)
        return;
    fmpegManger->seekToPosition(position);
}

extern "C"
JNIEXPORT void JNICALL
cpp_init_callback(JNIEnv *env, jobject thiz) {
    if (fmpegManger == nullptr)
        fmpegManger = new FFmpegManger();
    fmpegManger->initCallBack(env, thiz);

}

/*********************** GL 聚光手电筒********************/

extern "C"
JNIEXPORT jboolean JNICALL
cpp_flash_light_init_opengl(JNIEnv *env, jobject thiz, jint width, jint height) {
    if (flashLight == nullptr)
        flashLight = new OpenglesFlashLight();
    flashLight->setupGraphics(width, height);
    return 0;
}

extern "C"
JNIEXPORT void JNICALL
cpp_flash_light_render_frame(JNIEnv *env, jobject thiz) {
    if (flashLight == nullptr) return;
    flashLight->renderFrame();

}

extern "C"
JNIEXPORT void JNICALL
cpp_flash_light_frag_vertex_path(JNIEnv *env, jobject thiz, jstring frag, jstring vertex,
                                 jstring picsrc1, jstring picsrc2) {
    const char *fragPath = env->GetStringUTFChars(frag, nullptr);
    const char *vertexPath = env->GetStringUTFChars(vertex, nullptr);
    const char *picsrc1Path = env->GetStringUTFChars(picsrc1, nullptr);
    const char *picsrc2Path = env->GetStringUTFChars(picsrc2, nullptr);

    if (flashLight == nullptr) {
        flashLight = new OpenglesFlashLight();
    }
    flashLight->setSharderPath(vertexPath, fragPath);

    flashLight->setPicPath(picsrc1Path, picsrc2Path);

    env->ReleaseStringUTFChars(frag, fragPath);
    env->ReleaseStringUTFChars(vertex, vertexPath);
    env->ReleaseStringUTFChars(picsrc1, picsrc1Path);
    env->ReleaseStringUTFChars(picsrc2, picsrc2Path);

}

extern "C"
JNIEXPORT void JNICALL
cpp_flash_light_color_frag_vertex_path(JNIEnv *env, jobject thiz, jstring frag,
                                       jstring vertex) {
    const char *fragPath = env->GetStringUTFChars(frag, nullptr);
    const char *vertexPath = env->GetStringUTFChars(vertex, nullptr);

    if (flashLight == nullptr) {
        flashLight = new OpenglesFlashLight();
    }
    flashLight->setColorSharderPath(vertexPath, fragPath);

    env->ReleaseStringUTFChars(frag, fragPath);
    env->ReleaseStringUTFChars(vertex, vertexPath);

}

extern "C"
JNIEXPORT void JNICALL
cpp_flash_light_move_xy(JNIEnv *env, jobject thiz, jfloat dx, jfloat dy, jint actionMode) {
    if (flashLight == nullptr) return;
    flashLight->setMoveXY(dx, dy, actionMode);
}

extern "C"
JNIEXPORT void JNICALL
cpp_flash_light_on_scale(JNIEnv *env, jobject thiz, jfloat scaleFactor, jfloat focusX,
                         jfloat focusY,
                         jint actionMode) {
    if (flashLight == nullptr) return;
    flashLight->setOnScale(scaleFactor, focusX, focusY, actionMode);
}

/*********************** GL 摄像头预览********************/

extern "C"
JNIEXPORT jboolean JNICALL
cpp_camera_pre_init_opengl(JNIEnv *env, jobject thiz) {
    if (cameraPre == nullptr)
        cameraPre = new OpenglesCameraPre();
    cameraPre->initGraphics();
    return 0;
}

extern "C"
JNIEXPORT jboolean JNICALL
cpp_camera_pre_set_wh_opengl(JNIEnv *env, jobject thiz, jint width, jint height) {
    if (cameraPre == nullptr) return -1;
    cameraPre->setScreenWH(width, height);
    return 0;
}

extern "C"
JNIEXPORT void JNICALL
cpp_camera_pre_render_frame(JNIEnv *env, jobject thiz,jint renderTexture) {
    if (cameraPre == nullptr) return;
    cameraPre->renderFrame(renderTexture);

}

extern "C"
JNIEXPORT void JNICALL
cpp_camera_pre_frag_vertex_path(JNIEnv *env, jobject thiz, jstring frag, jstring vertex) {
    const char *fragPath = env->GetStringUTFChars(frag, nullptr);
    const char *vertexPath = env->GetStringUTFChars(vertex, nullptr);

    if (cameraPre == nullptr) {
        cameraPre = new OpenglesCameraPre();
    }
    cameraPre->setSharderPath(vertexPath, fragPath);

    env->ReleaseStringUTFChars(frag, fragPath);
    env->ReleaseStringUTFChars(vertex, vertexPath);

}






// 重点：定义类名和函数签名，如果有多个方法要动态注册，在数组里面定义即可
static const JNINativeMethod methods[] = {
        {"native_callback",         "()V",                                                       (void *) cpp_init_callback},

        {"native_getFFmpegVersion", "()Ljava/lang/String;",                                      (void *) cpp_getFFmpegVersion},
        {"stringFromJNI",           "()Ljava/lang/String;",                                      (std::string *) cpp_stringFromJNI},
        {"intFromJNI",              "(I)I",                                                      (void *) cpp_intFromJNI},
        {"setInputUrl",             "(Ljava/lang/String;)V",                                     (void *) cpp_set_input_url},

        {"native_MP4_AVI",          "(Ljava/lang/String;Ljava/lang/String;)V",                   (void *) cpp_mp4_input_avi_output},
        {"native_Water_mark",       "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V", (void *) cpp_mp4_water_mark},

        {"native_Play_Audio",       "(Ljava/lang/String;)V",                                     (void *) cpp_play_audio},
        {"native_Stop_Audio",       "()V",                                                       (void *) cpp_stop_audio},

        {"native_Play_init",        "(Ljava/lang/String;Landroid/view/Surface;)V",               (void *) cpp_play_init},
        {"native_Play_Uninit",      "()V",                                                       (void *) cpp_play_uninit},
        {"native_Play_Video",       "()V",                                                       (void *) cpp_play_video},
        {"native_Stop_Video",       "()V",                                                       (void *) cpp_stop_video},
        {"native_Pause_Video",      "()V",                                                       (void *) cpp_pause_video},
        {"native_seek_to_position", "(F)V",                                                      (void *) cpp_seek_to_position},

        //GLCameraPre
        {"native_camera_pre_init_opengl",          "()Z",                 (void *) cpp_camera_pre_init_opengl},
        {"native_camera_pre_set_wh_opengl",          "(II)Z",                 (void *) cpp_camera_pre_set_wh_opengl},
        {"native_camera_pre_render_frame",         "(I)V",                   (void *) cpp_camera_pre_render_frame},
        {"native_camera_pre_set_glsl_path",        "(Ljava/lang/String"
                                                    ";Ljava/lang/String;)V", (void *) cpp_camera_pre_frag_vertex_path},


        //聚光手电筒
        {"native_flash_light_init_opengl",          "(II)Z",                 (void *) cpp_flash_light_init_opengl},
        {"native_flash_light_render_frame",         "()V",                   (void *) cpp_flash_light_render_frame},
        {"native_flash_light_color_set_glsl_path",  "(Ljava/lang/String"
                                                    ";Ljava/lang/String;)V", (void *) cpp_flash_light_color_frag_vertex_path},
        {"native_flash_light_set_glsl_path",        "(Ljava/lang/String"
                                                    ";Ljava/lang/String"
                                                    ";Ljava/lang/String"
                                                    ";Ljava/lang/String;)V", (void *) cpp_flash_light_frag_vertex_path},
        {"native_flash_light_move_xy",              "(FFI)V",                (void *) cpp_flash_light_move_xy},
        {"native_flash_light_on_scale",             "(FFFI)V",               (void *) cpp_flash_light_on_scale},


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

