//
// Created by MMM on 2024/4/15.
//

#include "include/FFmpegManger.h"


const char *FFmpegManger::getInputUrl() const {
    return inputUrl;
}

void FFmpegManger::setInputUrl(const char *inputUrl) {
    FFmpegManger::inputUrl = inputUrl;
}


int FFmpegManger::mp4ConversionAvi(const char *in_filename, const char *out_filename) {
    if (fFmpegVideoTrans == nullptr) {
        fFmpegVideoTrans = new FFmpegVideoTrans();
    }
    int i = fFmpegVideoTrans->mp4ConversionAvi(in_filename, out_filename);
    return i;
}

int FFmpegManger::mp4WaterMark(const char *inputUrl, const char *pngUrl, const char *outputUrl) {
//    FFmpegWaterMark *waterMark = new FFmpegWaterMark();
//    int ret = waterMark->mp4WaterMark(inputUrl, pngUrl, outputUrl);
    if (waterMark == nullptr) {
        waterMark = new FFmpegWaterMarkTest();
    }
    waterMark->mp4WaterMark(inputUrl, pngUrl, outputUrl);

//    FFmpegWaterMarkTest *waterMark = new FFmpegWaterMarkTest();
//    waterMark->mp4WaterMark_leixiaohua(inputUrl, pngUrl, outputUrl);
    return 0;
}

int FFmpegManger::playAudio(const char *inputUrl) {
    if (fmpegPlayAudio == nullptr) {
        fmpegPlayAudio = new FFmpegPlayAudio();
    }
    fmpegPlayAudio->playAudio(inputUrl);
    return 0;
}

int FFmpegManger::stopAudio() {
    if (fmpegPlayAudio == nullptr) {
        return -1;
    }
    fmpegPlayAudio->stopAudio();
    return 0;
}

int FFmpegManger::playVideo() {
    if (videoPlay != nullptr) {
        videoPlay->playVideo();
    }
    return 0;
}

int FFmpegManger::stopVideo() {
    if (videoPlay == nullptr) {
        return -1;
    }
    videoPlay->stopVideo();
    return 0;
}

int FFmpegManger::pauseVideo() {
    if (videoPlay == nullptr) {
        return -1;
    }
    videoPlay->pauseVideo();
    return 0;
}

int FFmpegManger::seekToPosition(float position) {
    if (videoPlay == nullptr) {
        return -1;
    }
    videoPlay->seekToPosition(position);
    return 0;
}

int FFmpegManger::initFFmpeg(JNIEnv *env, jobject thiz, const char *inputUrl, jobject surface) {
    mEnv = env;
//    mJvm = env->NewGlobalRef(thiz);
    env->GetJavaVM(&mJavaVm);
    mJavaObj = env->NewGlobalRef(thiz);
    if (videoPlay == nullptr) {
        videoPlay = new FFmpegVideoPlay();
        videoPlay->init(env, thiz, inputUrl, surface);
    }
    return 0;
}

void FFmpegManger::unInit() {
    if (videoPlay != nullptr) {
        videoPlay->unInit();
    }
    mEnv->DeleteGlobalRef(mJavaObj);
}

int FFmpegManger::initCallBack(JNIEnv *env, jobject thiz) {
    if (videoPlay == nullptr) {
        videoPlay = new FFmpegVideoPlay();
    }
    mEnv = env;
    mJavaObj = env->NewGlobalRef(thiz);
    videoPlay->initCallback(env, thiz);
    videoPlay->SetMessageCallback(this, PostMessage);
    videoPlay->SetPlayStatusCallback(this, PostPlayStatusMessage);
    return 0;
}

JNIEnv *FFmpegManger::GetJNIEnv(bool *isAttach) {
    JNIEnv *env;
    int status;
    if (nullptr == mJavaVm) {
        LOGD("FFmpegManger::GetJNIEnv mJavaVm == nullptr");
        return nullptr;
    }
    *isAttach = false;
    status = mJavaVm->GetEnv((void **) &env, JNI_VERSION_1_6);
    if (status != JNI_OK) {
        status = mJavaVm->AttachCurrentThread(&env, nullptr);
        if (status != JNI_OK) {
            LOGD("FFmpegManger::GetJNIEnv failed to attach current thread");
            return nullptr;
        }
        *isAttach = true;
    }
    return env;
}

void FFmpegManger::PostMessage(void *context, int msgType, float msgCode) {
    if (context != nullptr) {
        FFmpegManger *pFmpegManger = static_cast<FFmpegManger *>(context);
        bool isAttach = false;
        //C++子线程回到回Java主线程，两线程之间的调用切换
        JNIEnv *pEnv = pFmpegManger->GetJNIEnv(&isAttach);
        if (pEnv == nullptr) {
            return;
        }
        jobject javaObj = pFmpegManger->mJavaObj;
        jmethodID mid = pEnv->GetMethodID(pEnv->GetObjectClass(javaObj), "CppEventCallback",
                                          "(IF)V");
        pEnv->CallVoidMethod(javaObj, mid, msgType, msgCode);
        if (isAttach) {
            JavaVM *pJavaVm = pFmpegManger->mJavaVm;
            pJavaVm->DetachCurrentThread();
        }
    }
}

void FFmpegManger::PostPlayStatusMessage(void *context, const char *status) {
    if (context != nullptr) {
        FFmpegManger *pFmpegManger = static_cast<FFmpegManger *>(context);
        bool isAttach = false;
        JNIEnv *pEnv = pFmpegManger->GetJNIEnv(&isAttach);
        if (pEnv == nullptr) {
            return;
        }
        jobject javaObj = pFmpegManger->mJavaObj;
        jmethodID mid = pEnv->GetMethodID(pEnv->GetObjectClass(javaObj), "CppPlayStatusCallback",
                                          "(Ljava/lang/String;)V");
        jstring pJstring = pEnv->NewStringUTF(status);
        pEnv->CallVoidMethod(javaObj, mid, pJstring);
//        if (isAttach) {
//            JavaVM *pJavaVm = pFmpegManger->mJavaVm;
//            pJavaVm->DetachCurrentThread();
//        }
    }
}


