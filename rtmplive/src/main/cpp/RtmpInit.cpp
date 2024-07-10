//
// Created by MMM on 2024/7/10.
//

#include "include/RtmpInit.h"


using namespace std;

void RtmpInit::startRtmp(const char *path) {
    mUrl = path;
    if (childThread == nullptr) {
        childThread = new thread(DoAVdecoding, this);
        childThread->detach();
    }
}

void RtmpInit::stopRtmp() {
    callbackMsg("Rtmp stopRtmp", 0);

}

void RtmpInit::pauseRtmp() {
    callbackMsg("Rtmp pauseRtmp", 0);

}

void RtmpInit::releaseRtmp() {
    callbackMsg("Rtmp releaseRtmp", 0);


}

void RtmpInit::DoAVdecoding(RtmpInit *rtmpInit) {
    rtmpInit->startThread();
}

void RtmpInit::startThread() {
    callbackMsg("Rtmp startThread", 0);
    LOGE("murl:%s", mUrl);

    char *url = const_cast<char *>(mUrl);
    RTMP *rtmp = 0;
    do {
        rtmp = RTMP_Alloc();
        if (!rtmp) {
            callbackMsg("Rtmp create fail", -1);
            break;
        }
        RTMP_Init(rtmp);
        rtmp->Link.timeout = 5;
        int ret = RTMP_SetupURL(rtmp, url);
        if (!ret) {
            callbackMsg("Rtmp SetupURL fail", ret);
            break;
        }
        //开启输出模式
        RTMP_EnableWrite(rtmp);
        ret = RTMP_Connect(rtmp, 0);
        if (!ret) {
            callbackMsg("rtmp连接地址失败", ret);
            break;
        }
        ret = RTMP_ConnectStream(rtmp, 0);
        if (!ret) {
            callbackMsg("rtmp连接流失败", ret);
            break;
        }



    } while (0);

    //释放rtmp
    if (rtmp) {
        RTMP_Close(rtmp);
        RTMP_Free(rtmp);
    }
}

void RtmpInit::setRtmpStatusCallback(void *context, RtmpStatusCallback callback) {
    mContext = context;
    mStatusCallback = callback;
}

void RtmpInit::callbackMsg(const char *msg, float codeErr) {
    if (mContext != nullptr && mStatusCallback != nullptr)
        mStatusCallback(mContext, msg, 0);
}


