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
#include "VideoStreamPacket.h"
#include "AudioStreamPacket.h"

class RtmpPusherManger {

private:
    JNIEnv *mEnv = nullptr;
    JavaVM *mJavaVm = nullptr;
    jobject mJavaObj = nullptr;
    RtmpInit *rtmpInit = nullptr;
    VideoStreamPacket *videoStreamPacket = nullptr;
    AudioStreamPacket *audioStreamPacket = nullptr;
    std::atomic<bool> isPushing;

    static void RtmpStatusMessage(void *context, const char *status, float msgCode);

    static void callbackRtmpPacket(void *context, RTMPPacket *packet);

    static void callbackAudioTag(void *context);


public:
    int initCallBack(JNIEnv *env, jobject thiz);

    JNIEnv *GetJNIEnv(bool *isAttach);

    void init_rtmp(JNIEnv *env, jobject thiz);

    void start_rtmp(const char *path);

    void addRtmpPacket(RTMPPacket *packet);

    void stop_rtmp();

    void pause_rtmp();

    void release_rtmp();

    void initVideoPacket();

    void encodeVideoPacket(int8_t *data);

    int setVideoEncInfo(int width, int height, int fps, int bitrate);


    void initAudioPacket();

    int getAudioInputSample();

    void setAudioCodecInfo(int sampleRateInHz, int channels);

    RTMPPacket* getAudioTag();

    void encodeAudioPacket(int8_t *data);

};


#endif //MYYFFMPEG_RTMPPUSHERMANGER_H
