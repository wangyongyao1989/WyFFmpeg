//
// Created by MMM on 2024/7/10.
//

#ifndef MYYFFMPEG_RTMPINIT_H
#define MYYFFMPEG_RTMPINIT_H

#define LOG_TAG "wy"
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

#include <android/log.h>
#include <thread>
#include <mutex>
#include <string>
#include "PacketQueue.h"

using namespace std;

typedef void (*RtmpStatusCallback)(void *, const char *, float codeErr);

typedef void (*GetAudioTagCallback)(void *);

extern "C"
{
#include "rtmp.h"
}


class RtmpInit {

private:
    // 推流地址：持有值而不是裸指针，见 RtmpInit::startRtmp
    string mUrl;
    thread *childThread = nullptr;
    // 保护 childThread：推流线程退出时会把它置空，Java 侧同时可能调 startRtmp
    mutex mThreadMutex;
    void *mContext = nullptr;
    // 推流起始时刻（毫秒），所有包的相对时间戳都基于它。
    // 以前未初始化就参与 RTMP_GetTime() - start_time，时间戳是随机值，
    // 可能远超 24bit，触发扩展时间戳分支并被服务器判为非法流。
    uint32_t start_time = 0;
    std::atomic<bool> isPushing;
    // 暂停态：真正的"暂停"是继续维持 RTMP 连接、但不再送音视频包。
    // 原来 pauseRtmp() 只打了一行日志，点暂停推流效果和数据完全不变。
    std::atomic<bool> isPaused{false};
    uint32_t pauseBegin = 0;
    PacketQueue<RTMPPacket *> packetQueue;


public:
    RtmpStatusCallback mStatusCallback = nullptr;
    GetAudioTagCallback mGetAudioTagCallback = nullptr;

private:
    static void DoRtmpInit(RtmpInit *rtmpInit);

    void callbackStatusMsg(const char *msg, float codeErr);

    void startThread();

    void releasePackets(RTMPPacket *&packet);

public:
    void startRtmp(const char *path);

    void addRtmpPacket(RTMPPacket *packet);

    void stopRtmp();

    void pauseRtmp();

    void releaseRtmp();

    void setRtmpStatusCallback(void *context, RtmpStatusCallback callback);

    void setAudioTagCallBack(void *context, GetAudioTagCallback mGetAudioTagCallback);

    void getAudioTag();

    ~RtmpInit();
};


#endif //MYYFFMPEG_RTMPINIT_H
