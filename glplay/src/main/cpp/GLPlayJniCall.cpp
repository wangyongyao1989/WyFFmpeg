#include <jni.h>
#include <string>
#include <android/log.h>
#include "includeopengl/OpenglesFlashLight.h"
#include "includeopengl/OpenglesCameraPre.h"
#include "includeopengl/OpenGLShader.h"
#include "includeopengl/OpenGLCamera3D.h"
#include <android/native_window_jni.h>
#include <android/asset_manager_jni.h>
#include "OpenglesTexureVideoRender.h"

#define LOG_TAG "wy"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

//包名+类名字符串定义：
const char *rtmp_class_name = "com/wangyongyao/glplay/OpenGLPlayCallJni";

OpenglesFlashLight *flashLight;
OpenglesCameraPre *cameraPre;
OpenglesTexureVideoRender *textureVideoRender;


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
JNIEXPORT jint JNICALL
cpp_camera_pre_init_opengl(JNIEnv *env, jobject thiz) {
    int textureId = -1;
    if (cameraPre == nullptr)
        cameraPre = new OpenglesCameraPre();
    textureId = cameraPre->initGraphics();
    return textureId;
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
cpp_camera_pre_set_video_texture(JNIEnv *env, jobject thiz, jint videoTexture) {
    if (cameraPre == nullptr) return;
    cameraPre->setVideoTexture(videoTexture);
}

extern "C"
JNIEXPORT void JNICALL
cpp_camera_pre_render_frame(JNIEnv *env, jobject thiz, jfloatArray jArray) {
    if (cameraPre == nullptr) return;
    const int MAX_LENGTH = 1024;
    float cppArray[MAX_LENGTH];
    jsize length = env->GetArrayLength(jArray);
    jfloat *cArray = env->GetFloatArrayElements(jArray, 0);
    if (cArray != NULL) {
        memcpy(cppArray, cArray, length * sizeof(float));
        env->ReleaseFloatArrayElements(jArray, cArray, 0);
    }
    cameraPre->renderFrame(cppArray);

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

extern "C"
JNIEXPORT void JNICALL
cpp_camera_pre_frag_vertex_pic(JNIEnv *env, jobject thiz, jstring pic) {
    const char *picPath = env->GetStringUTFChars(pic, nullptr);

    if (cameraPre == nullptr) {
        cameraPre = new OpenglesCameraPre();
    }
    cameraPre->setSharderPic(picPath);

    env->ReleaseStringUTFChars(pic, picPath);

}


/*********************** OpenGL Texture预览Camera视频********************/
extern "C"
JNIEXPORT void JNICALL
cpp_texture_video_play_creat(JNIEnv *env, jobject thiz, jint type,
                             jstring vertex,
                             jstring frag) {
    const char *vertexPath = env->GetStringUTFChars(vertex, nullptr);
    const char *fragPath = env->GetStringUTFChars(frag, nullptr);
    if (textureVideoRender == nullptr)
        textureVideoRender = new OpenglesTexureVideoRender();

    textureVideoRender->setSharderPath(vertexPath, fragPath);

    env->ReleaseStringUTFChars(vertex, vertexPath);
    env->ReleaseStringUTFChars(frag, fragPath);
}

extern "C"
JNIEXPORT void JNICALL
cpp_texture_video_play_destroy(JNIEnv *env, jobject thiz) {

}

extern "C"
JNIEXPORT void JNICALL
cpp_texture_video_play_init(JNIEnv *env, jobject thiz,
                            jobject surface,
                            jobject assetManager,
                            jint width,
                            jint height) {
    if (textureVideoRender != nullptr) {
        ANativeWindow *window = surface ? ANativeWindow_fromSurface(env, surface) : nullptr;
        auto *aAssetManager = assetManager ? AAssetManager_fromJava(env, assetManager) : nullptr;
        textureVideoRender->init(window, aAssetManager, (size_t) width, (size_t) height);
    }
}

extern "C"
JNIEXPORT void JNICALL
cpp_texture_video_play_render(JNIEnv *env, jobject thiz) {
    if (textureVideoRender != nullptr) {
        textureVideoRender->render();
    }
}

extern "C"
JNIEXPORT void JNICALL
cpp_texture_video_play_draw(JNIEnv *env, jobject obj, jbyteArray data, jint width, jint height,
                            jint rotation) {
    jbyte *bufferPtr = env->GetByteArrayElements(data, nullptr);
    jsize arrayLength = env->GetArrayLength(data);

    if (textureVideoRender != nullptr) {

        textureVideoRender->draw((uint8_t *) bufferPtr, (size_t) arrayLength, (size_t) width,
                                 (size_t) height,
                                 rotation);
    }

    env->ReleaseByteArrayElements(data, bufferPtr, 0);
}

extern "C"
JNIEXPORT void JNICALL
cpp_texture_video_play_setParameters(JNIEnv *env, jobject thiz, jint p) {

    if (textureVideoRender != nullptr) {
        textureVideoRender->setParameters((uint32_t) p);
    }

}

extern "C"
JNIEXPORT jint JNICALL
cpp_texture_video_play_getParameters(JNIEnv *env, jobject thiz) {
    if (textureVideoRender != nullptr) {
        textureVideoRender->getParameters();
    }
    return 0;

}

/*********************** OpenGL Texture预览Camera滤镜视频********************/
extern "C"
JNIEXPORT void JNICALL
cpp_texture_filter_player_creat(JNIEnv *env, jobject thiz, jint type,
                             jstring vertex,
                             jstring frag) {
    const char *vertexPath = env->GetStringUTFChars(vertex, nullptr);
    const char *fragPath = env->GetStringUTFChars(frag, nullptr);
    if (textureVideoRender == nullptr)
        textureVideoRender = new OpenglesTexureVideoRender();

    textureVideoRender->setSharderPath(vertexPath, fragPath);

    env->ReleaseStringUTFChars(vertex, vertexPath);
    env->ReleaseStringUTFChars(frag, fragPath);
}

extern "C"
JNIEXPORT void JNICALL
cpp_texture_filter_player_destroy(JNIEnv *env, jobject thiz) {

}

extern "C"
JNIEXPORT void JNICALL
cpp_texture_filter_player_init(JNIEnv *env, jobject thiz,
                            jobject surface,
                            jobject assetManager,
                            jint width,
                            jint height) {
    if (textureVideoRender != nullptr) {
        ANativeWindow *window = surface ? ANativeWindow_fromSurface(env, surface) : nullptr;
        auto *aAssetManager = assetManager ? AAssetManager_fromJava(env, assetManager) : nullptr;
        textureVideoRender->init(window, aAssetManager, (size_t) width, (size_t) height);
    }
}

extern "C"
JNIEXPORT void JNICALL
cpp_texture_filter_player_render(JNIEnv *env, jobject thiz) {
    if (textureVideoRender != nullptr) {
        textureVideoRender->render();
    }
}

extern "C"
JNIEXPORT void JNICALL
cpp_texture_filter_player_draw(JNIEnv *env, jobject obj, jbyteArray data, jint width, jint height,
                            jint rotation) {
    jbyte *bufferPtr = env->GetByteArrayElements(data, nullptr);
    jsize arrayLength = env->GetArrayLength(data);

    if (textureVideoRender != nullptr) {

        textureVideoRender->draw((uint8_t *) bufferPtr, (size_t) arrayLength, (size_t) width,
                                 (size_t) height,
                                 rotation);
    }

    env->ReleaseByteArrayElements(data, bufferPtr, 0);
}

extern "C"
JNIEXPORT void JNICALL
cpp_texture_filter_player_setParameters(JNIEnv *env, jobject thiz, jint p) {

    if (textureVideoRender != nullptr) {
        textureVideoRender->setParameters((uint32_t) p);
    }

}

extern "C"
JNIEXPORT jint JNICALL
cpp_texture_filter_player_getParameters(JNIEnv *env, jobject thiz) {
    if (textureVideoRender != nullptr) {
        textureVideoRender->getParameters();
    }
    return 0;

}


static const JNINativeMethod methods[] = {
        //GLCameraPre
        {"native_camera_pre_init_opengl",            "()I",                   (void *) cpp_camera_pre_init_opengl},
        {"native_camera_pre_set_wh_opengl",          "(II)Z",                 (void *) cpp_camera_pre_set_wh_opengl},
        {"native_camera_pre_set_video_texture",      "(I)V",                  (void *) cpp_camera_pre_set_video_texture},
        {"native_camera_pre_render_frame",           "([F)V",                 (void *) cpp_camera_pre_render_frame},
        {"native_camera_pre_set_glsl_path",          "(Ljava/lang/String"
                                                     ";Ljava/lang/String;)V", (void *) cpp_camera_pre_frag_vertex_path},
        {"native_camera_pre_set_glsl_pic",           "(Ljava/lang/String;)V", (void *) cpp_camera_pre_frag_vertex_pic},


        /*********************** OpenGL 聚光手电筒********************/
        {"native_flash_light_init_opengl",           "(II)Z",                 (void *) cpp_flash_light_init_opengl},
        {"native_flash_light_render_frame",          "()V",                   (void *) cpp_flash_light_render_frame},
        {"native_flash_light_color_set_glsl_path",   "(Ljava/lang/String"
                                                     ";Ljava/lang/String;)V", (void *) cpp_flash_light_color_frag_vertex_path},
        {"native_flash_light_set_glsl_path",         "(Ljava/lang/String"
                                                     ";Ljava/lang/String"
                                                     ";Ljava/lang/String"
                                                     ";Ljava/lang/String;)V", (void *) cpp_flash_light_frag_vertex_path},
        {"native_flash_light_move_xy",               "(FFI)V",                (void *) cpp_flash_light_move_xy},
        {"native_flash_light_on_scale",              "(FFFI)V",               (void *) cpp_flash_light_on_scale},

        /*********************** OpenGL Texture显示视频********************/
        {"native_texture_video_play_create",         "(I"
                                                     "Ljava/lang/String;"
                                                     "Ljava/lang/String;)V",  (void *) cpp_texture_video_play_creat},
        {"native_texture_video_play_destroy",        "()V",                   (void *) cpp_texture_video_play_destroy},
        {"native_texture_video_play_init",           "(Landroid/view/Surface;"
                                                     "Landroid/content/res"
                                                     "/AssetManager;II)V",    (void *) cpp_texture_video_play_init},
        {"native_texture_video_play_render",         "()V",                   (void *) cpp_texture_video_play_render},
        {"native_texture_video_play_draw",           "([BIII)V",              (void *) cpp_texture_video_play_draw},
        {"native_texture_video_play_set_parameters", "(I)V",                  (void *) cpp_texture_video_play_setParameters},
        {"native_texture_video_play_get_parameters", "()I",                   (void *) cpp_texture_video_play_getParameters},

        /*********************** OpenGL Texture显示滤镜视频********************/
        {"native_texture_filter_player_create",         "(I"
                                                     "Ljava/lang/String;"
                                                     "Ljava/lang/String;)V",  (void *) cpp_texture_filter_player_creat},
        {"native_texture_filter_player_destroy",        "()V",                   (void *) cpp_texture_filter_player_destroy},
        {"native_texture_filter_player_init",           "(Landroid/view/Surface;"
                                                     "Landroid/content/res"
                                                     "/AssetManager;II)V",    (void *) cpp_texture_filter_player_init},
        {"native_texture_filter_player_render",         "()V",                   (void *) cpp_texture_filter_player_render},
        {"native_texture_filter_player_draw",           "([BIII)V",              (void *) cpp_texture_filter_player_draw},
        {"native_texture_filter_player_set_parameters", "(I)V",                  (void *) cpp_texture_filter_player_setParameters},
        {"native_texture_filter_player_get_parameters", "()I",                   (void *) cpp_texture_filter_player_getParameters},
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


