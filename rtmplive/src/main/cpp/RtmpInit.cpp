//
// Created by MMM on 2024/7/10.
//

#include "include/RtmpInit.h"


using namespace std;

void RtmpInit::startRtmp(const char *path) {
    mUrl = path;
    if (childThread == nullptr) {
        childThread = new thread(DoRtmpInit, this);
        childThread->detach();
    }
}


void RtmpInit::addRtmpPacket(RTMPPacket *packet) {
    callbackStatusMsg("Rtmp addRtmpPacket", 0);
    packet->m_nTimeStamp = RTMP_GetTime() - start_time;
    packetQueue.push(packet);
}

void RtmpInit::stopRtmp() {
    callbackStatusMsg("Rtmp stopRtmp", 0);

}

void RtmpInit::pauseRtmp() {
    callbackStatusMsg("Rtmp pauseRtmp", 0);

}

void RtmpInit::releaseRtmp() {
    callbackStatusMsg("Rtmp releaseRtmp", 0);


}

void RtmpInit::DoRtmpInit(RtmpInit *rtmpInit) {
    rtmpInit->startThread();
}

void RtmpInit::startThread() {
    callbackStatusMsg("Rtmp startThread", 0);
    LOGE("murl:%s", mUrl);

    char *url = const_cast<char *>(mUrl);
    RTMP *rtmp = 0;
    do {
        rtmp = RTMP_Alloc();
        if (!rtmp) {
            callbackStatusMsg("Rtmp create fail", -1);
            break;
        }
        RTMP_Init(rtmp);
        rtmp->Link.timeout = 5;
        int ret = RTMP_SetupURL(rtmp, url);
        if (!ret) {
            callbackStatusMsg("Rtmp SetupURL fail", ret);
            break;
        }
        //开启输出模式
        RTMP_EnableWrite(rtmp);
        ret = RTMP_Connect(rtmp, 0);
        if (!ret) {
            callbackStatusMsg("rtmp连接地址失败", ret);
            break;
        }
        ret = RTMP_ConnectStream(rtmp, 0);
        if (!ret) {
            callbackStatusMsg("rtmp连接流失败", ret);
            break;
        }
        //start pushing
        isPushing = true;
        packetQueue.setRunning(true);
        RTMPPacket *packet = nullptr;
        while (isPushing) {
            packetQueue.pop(packet);
            if (!isPushing) {
                break;
            }
            if (!packet) {
                continue;
            }

            packet->m_nInfoField2 = rtmp->m_stream_id;
            ret = RTMP_SendPacket(rtmp, packet, 1);
            releasePackets(packet);
            if (!ret) {
                LOGE("RTMP_SendPacket fail...");
                callbackStatusMsg("RTMP_SendPacket fail...", -2);
                break;
            }
        }
        releasePackets(packet);

    } while (0);
    isPushing = false;
    packetQueue.setRunning(false);
    packetQueue.clear();
    //释放rtmp
    if (rtmp) {
        RTMP_Close(rtmp);
        RTMP_Free(rtmp);
    }
    delete url;
}

void RtmpInit::releasePackets(RTMPPacket *&packet) {
    if (packet) {
        RTMPPacket_Free(packet);
        delete packet;
        packet = nullptr;
    }
}

void RtmpInit::setRtmpStatusCallback(void *context, RtmpStatusCallback callback) {
    mContext = context;
    mStatusCallback = callback;
}

void RtmpInit::callbackStatusMsg(const char *msg, float codeErr) {
    if (mContext != nullptr && mStatusCallback != nullptr)
        mStatusCallback(mContext, msg, 0);
}


