//
// Created by MMM on 2024/7/8.
//

#include "RtmpPusherManger.h"

void RtmpPusherManger::init_rtmp(JNIEnv *env, jobject thiz) {
    mEnv = env;
    env->GetJavaVM(&mJavaVm);
    mJavaObj = env->NewGlobalRef(thiz);
    RtmpStatusMessage(this, "init_rtmp", 0);
    if (rtmpInit == nullptr)
        rtmpInit = new RtmpInit();

}

void RtmpPusherManger::start_rtmp(const char *path) {
    if (rtmpInit == nullptr) return;
    rtmpInit->startRtmp(path);
    isPushing = true;
}

void RtmpPusherManger::stop_rtmp() {
    if (rtmpInit == nullptr) return;
    rtmpInit->stopRtmp();
    isPushing = false;

}

void RtmpPusherManger::pause_rtmp() {
    if (rtmpInit == nullptr) return;
    rtmpInit->pauseRtmp();
}

void RtmpPusherManger::release_rtmp() {
    if (rtmpInit == nullptr) return;
    rtmpInit->releaseRtmp();
}


void RtmpPusherManger::encodeVideoPacket(int8_t *data) {
//    RtmpStatusMessage(this, "encodeVideoPacket", 0);
    if (videoStreamPacket == nullptr) return;
    if (!isPushing) return;
    videoStreamPacket->encodeVideo(data);
}

void RtmpPusherManger::initVideoPacket() {
    RtmpStatusMessage(this, "initVideoPacket", 0);
    if (videoStreamPacket == nullptr) {
        videoStreamPacket = new VideoStreamPacket();
    }
    videoStreamPacket->setRtmpStatusCallback(this, RtmpStatusMessage);
    videoStreamPacket->setVideoCallback(callbackRtmpPacket);
}


int RtmpPusherManger::initCallBack(JNIEnv *env, jobject thiz) {
    if (mEnv == nullptr) {
        mEnv = env;
        env->GetJavaVM(&mJavaVm);
    }
    if (mJavaObj == nullptr)
        mJavaObj = env->NewGlobalRef(thiz);
    RtmpStatusMessage(this, "initCallBack", 0);
    if (rtmpInit == nullptr)
        rtmpInit = new RtmpInit();
    rtmpInit->setRtmpStatusCallback(this, RtmpStatusMessage);
    return 0;
}

JNIEnv *RtmpPusherManger::GetJNIEnv(bool *isAttach) {
    JNIEnv *env;
    int status;
    if (nullptr == mJavaVm) {
        return nullptr;
    }
    *isAttach = false;
    status = mJavaVm->GetEnv((void **) &env, JNI_VERSION_1_6);
    if (status != JNI_OK) {
        status = mJavaVm->AttachCurrentThread(&env, nullptr);
        if (status != JNI_OK) {
            LOGD("::GetJNIEnv failed to attach current thread");
            return nullptr;
        }
        *isAttach = true;
    }
    return env;
}

void RtmpPusherManger::RtmpStatusMessage(void *context, const char *status, float msgCode) {
    if (context != nullptr) {
        RtmpPusherManger *pFmpegManger = static_cast<RtmpPusherManger *>(context);
        bool isAttach = false;
        //C++子线程回到回Java主线程，两线程之间的调用切换
        JNIEnv *pEnv = pFmpegManger->GetJNIEnv(&isAttach);
        if (pEnv == nullptr) {
            return;
        }
        jobject javaObj = pFmpegManger->mJavaObj;
        jmethodID mid = pEnv->GetMethodID(pEnv->GetObjectClass(javaObj), "rtmpStatusMessage",
                                          "(Ljava/lang/String;F)V");
        jstring pJstring = pEnv->NewStringUTF(status);
        pEnv->CallVoidMethod(javaObj, mid, pJstring, msgCode);
        if (isAttach) {
            JavaVM *pJavaVm = pFmpegManger->mJavaVm;
            pJavaVm->DetachCurrentThread();
        }
    }

}

void RtmpPusherManger::callbackRtmpPacket(RTMPPacket *packet) {
    LOGE("callbackRtmpPacket ===%p", packet);
}









