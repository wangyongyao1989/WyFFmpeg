//
// Created by MMM on 2024/7/10.
//

#include "include/RtmpInit.h"

#include <vector>
#include <mutex>


using namespace std;

void RtmpInit::startRtmp(const char *path) {
    // path 是 JNI 层 new char[] 出来的临时缓冲。原来只是把指针存进 mUrl，
    // 由 startThread() 结尾的 delete url 释放：new[] 配 delete 属于未定义行为，
    // 且连接失败提前 break 时它永远泄漏。这里当场拷进 std::string 再释放。
    lock_guard<mutex> lock(mThreadMutex);
    if (path) {
        mUrl = path;
        delete[] path;
    }
    if (childThread == nullptr) {
        childThread = new thread(DoRtmpInit, this);
        childThread->detach();
    }
}


void RtmpInit::addRtmpPacket(RTMPPacket *packet) {
    if (packet == nullptr) return;
    if (isPaused) {
        // 暂停期间编码器仍在出帧，直接丢掉并释放：入队会让 packetQueue 无限增长。
        // 另外不能把这段空档留在时间戳里：恢复时 pauseRtmp() 会把空档从基准里扣掉。
        releasePackets(packet);
        return;
    }
    packet->m_nTimeStamp = RTMP_GetTime() - start_time;
    packetQueue.push(packet);
}
void RtmpInit::stopRtmp() {
    callbackStatusMsg("Rtmp stopRtmp", 0);
    isPushing = false;
    packetQueue.setRunning(false);
}

void RtmpInit::pauseRtmp() {
    if (!isPushing) {
        // 没在推流时暂停没有意义：isPaused 会一直挂着，等下次开始推流莫名其妙不出画面
        callbackStatusMsg("Rtmp pauseRtmp ignored: not pushing", 0);
        return;
    }
    if (isPaused) {
        isPaused = false;
        // 把暂停的这段时间从基准里扣掉，恢复后时间戳接着暂停前走，
        // 否则播放器会看到几十毫秒到几分钟的跳变而重新缓冲
        start_time += RTMP_GetTime() - pauseBegin;
        callbackStatusMsg("Rtmp resume", 0);
    } else {
        isPaused = true;
        pauseBegin = RTMP_GetTime();
        // 注意：暂停期间不再发任何 chunk，超过服务器空闲超时（mediamtx 默认 10s）
        // 仍会被断开，断开后需要重新点"开始推流"。
        callbackStatusMsg("Rtmp pauseRtmp", 0);
    }
}

void RtmpInit::releaseRtmp() {
    callbackStatusMsg("Rtmp releaseRtmp", 0);
    isPushing = false;
    packetQueue.setRunning(false);
    // 与 DoRtmpInit 收尾处的置空互斥：两边都可能 delete 同一个 thread 对象
    lock_guard<mutex> lock(mThreadMutex);
    if (childThread) {
        delete childThread;
        childThread = nullptr;
    }
}

void RtmpInit::DoRtmpInit(RtmpInit *rtmpInit) {
    rtmpInit->startThread();
    // 推流线程退出后必须把句柄清掉：RtmpInit 是进程内单例（停止推流只走 stopRtmp，
    // 不走 releaseRtmp），childThread 一直非空的话，"停止→再点开始推流" 不会再创建
    // 线程，表现为第二次开始推流完全没反应。detach 过的 thread 对象析构是安全的。
    lock_guard<mutex> lock(rtmpInit->mThreadMutex);
    delete rtmpInit->childThread;
    rtmpInit->childThread = nullptr;
}

void RtmpInit::startThread() {
    callbackStatusMsg("Rtmp startThread", 0);
    LOGE("murl:%s", mUrl.c_str());

    // RTMP_SetupURL 会原地改写传入的 url（截断空格后的参数），并且
    // Link.playpath/app/tcUrl 的 av_val 直接指向这块缓冲内部的子串，
    // 所以整条连接期间都得有一份自己的可写副本，不能把 std::string 的内部缓冲交出去。
    vector<char> urlBuf(mUrl.begin(), mUrl.end());
    urlBuf.push_back('\0');
    RTMP *rtmp = 0;
    do {
        rtmp = RTMP_Alloc();
        if (!rtmp) {
            callbackStatusMsg("Rtmp create fail", -1);
            break;
        }
        RTMP_Init(rtmp);
        rtmp->Link.timeout = 5;
        int ret = RTMP_SetupURL(rtmp, urlBuf.data());
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
        isPaused = false;   // 上次会话若停在暂停态，不能把新会话也变成"只连不推"
        // 时间戳基准：连接建立成功才取。start_time 以前从未赋值，
        // addRtmpPacket 里的 RTMP_GetTime() - start_time 是随机大数，
        // 轻则时间戳从设备开机毫秒数开始，重则跨 24bit 触发扩展时间戳被服务器判非法。
        start_time = RTMP_GetTime();
        packetQueue.setRunning(true);
        //获取音频的首帧值
        if (mContext != nullptr) {
            mGetAudioTagCallback(mContext);
        }
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
            // 某条 chunk stream 的第一条消息必须发 fmt0（12 字节完整头）：
            // fmt1/fmt2/fmt3 都是"参照上一条 chunk"的增量头，服务器没有参照状态时
            // 会直接断开（mediamtx: "received type 1 chunk without previous chunk"）。
            // 上层发包只保证了自己的 m_headerType，这里用 librtmp 自己的通道状态兜底。
            if (rtmp->m_vecChannelsOut[packet->m_nChannel] == nullptr) {
                packet->m_headerType = RTMP_PACKET_SIZE_LARGE;
            }
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

void RtmpInit::setAudioTagCallBack(void *context, GetAudioTagCallback call) {
    mContext = context;
    mGetAudioTagCallback = call;
}

void RtmpInit::callbackStatusMsg(const char *msg, float codeErr) {
    if (mContext != nullptr && mStatusCallback != nullptr)
        mStatusCallback(mContext, msg, 0);
}

RtmpInit::~RtmpInit() {
    isPushing = false;
    packetQueue.setRunning(false);
    if (childThread) {
        delete childThread;
        childThread = nullptr;
    }
}




