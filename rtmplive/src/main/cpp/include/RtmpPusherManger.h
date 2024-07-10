//
// Created by MMM on 2024/7/8.
//

#ifndef MYYFFMPEG_RTMPPUSHERMANGER_H
#define MYYFFMPEG_RTMPPUSHERMANGER_H
#define LOG_TAG "wy"
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

#include <android/log.h>
#include <jni.h>
#include "RtmpInit.h"

class RtmpPusherManger {

private:
    JNIEnv *mEnv = nullptr;
    JavaVM *mJavaVm = nullptr;
    jobject mJavaObj = nullptr;
    RtmpInit *rtmpInit = nullptr;

    static void RtmpStatusMessage(void *context, const char *status, float msgCode);

public:
    int initCallBack(JNIEnv *env, jobject thiz);

    JNIEnv *GetJNIEnv(bool *isAttach);

    void init_rtmp(JNIEnv *env, jobject thiz);

    void start_rtmp(const char *path);

    void stop_rtmp();

    void pause_rtmp();

    void release_rtmp();

};


#endif //MYYFFMPEG_RTMPPUSHERMANGER_H
