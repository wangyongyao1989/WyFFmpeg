//
// Created by MMM on 2024/7/11.
//

#include "include/VideoStreamPacket.h"
#include "RtmpPusherManger.h"

using namespace std;


VideoStreamPacket::VideoStreamPacket() : m_frameLen(0),
                                         videoCodec(nullptr),
                                         pic_in(nullptr),
                                         mVideoCallback(nullptr) {

}

VideoStreamPacket::~VideoStreamPacket() {

}


int VideoStreamPacket::setVideoEncInfo(int width, int height, int fps, int bitrate) {
    callbackStatusMsg("VideoStreamPacket setVideoEncInfo",0);
    m_frameLen = width * height;
    if (videoCodec) {
        x264_encoder_close(videoCodec);
        videoCodec = nullptr;
    }
    if (pic_in) {
        x264_picture_clean(pic_in);
        delete pic_in;
        pic_in = nullptr;
    }

    return 0;
}

void VideoStreamPacket::encodeVideo(int8_t *data) {
//    callbackStatusMsg("VideoStreamPacket encodeVideo",0);
    lock_guard<mutex> lock(m_mutex);
    if (!pic_in)
        return;
    //YUV420解析分离
    int offset = 0;
    memcpy(pic_in->img.plane[0], data, (size_t) m_frameLen); // y
    offset += m_frameLen;
    memcpy(pic_in->img.plane[1], data + offset, (size_t) m_frameLen / 4); // u
    offset += m_frameLen / 4;
    memcpy(pic_in->img.plane[2], data + offset, (size_t) m_frameLen / 4);  //v

    //YUV封装成H264
    x264_nal_t *pp_nal;
    int pi_nal;
    x264_picture_t pic_out;
    x264_encoder_encode(videoCodec, &pp_nal, &pi_nal, pic_in, &pic_out);
    int pps_len, sps_len = 0;
    uint8_t sps[100] = {0};
    uint8_t pps[100] = {0};
    //H264包装成RTMP流格式
    for (int i = 0; i < pi_nal; ++i) {
        x264_nal_t nal = pp_nal[i];
        if (nal.i_type == NAL_SPS) {
            sps_len = nal.i_payload - 4;
            memcpy(sps, nal.p_payload + 4, static_cast<size_t>(sps_len));
        } else if (nal.i_type == NAL_PPS) {
            pps_len = nal.i_payload - 4;
            memcpy(pps, nal.p_payload + 4, static_cast<size_t>(pps_len));
            sendSpsPps(sps, pps, sps_len, pps_len);
        } else {
            sendFrame(nal.i_type, nal.p_payload, nal.i_payload);
        }
    }

}

void VideoStreamPacket::setVideoCallback(VideoCallback callback) {
    mVideoCallback = callback;
}
void VideoStreamPacket::setRtmpStatusCallback(void *context, RtmpStatusCallback callback) {
    mContext = context;
    mStatusCallback = callback;
}

void VideoStreamPacket::sendSpsPps(uint8_t *sps, uint8_t *pps, int sps_len, int pps_len) {
    int bodySize = 13 + sps_len + 3 + pps_len;
    auto *packet = new RTMPPacket();
    RTMPPacket_Alloc(packet, bodySize);
    int i = 0;
    // type
    packet->m_body[i++] = 0x17;
    packet->m_body[i++] = 0x00;
    // timestamp
    packet->m_body[i++] = 0x00;
    packet->m_body[i++] = 0x00;
    packet->m_body[i++] = 0x00;

    //version
    packet->m_body[i++] = 0x01;
    // profile
    packet->m_body[i++] = sps[1];
    packet->m_body[i++] = sps[2];
    packet->m_body[i++] = sps[3];
    packet->m_body[i++] = 0xFF;

    //sps
    packet->m_body[i++] = 0xE1;
    //sps len
    packet->m_body[i++] = (sps_len >> 8) & 0xFF;
    packet->m_body[i++] = sps_len & 0xFF;
    memcpy(&packet->m_body[i], sps, sps_len);
    i += sps_len;

    //pps
    packet->m_body[i++] = 0x01;
    packet->m_body[i++] = (pps_len >> 8) & 0xFF;
    packet->m_body[i++] = (pps_len) & 0xFF;
    memcpy(&packet->m_body[i], pps, pps_len);

    //video
    packet->m_packetType = RTMP_PACKET_TYPE_VIDEO;
    packet->m_nBodySize = bodySize;
    packet->m_nChannel = 0x10;
    //sps and pps no timestamp
    packet->m_nTimeStamp = 0;
    packet->m_hasAbsTimestamp = 0;
    packet->m_headerType = RTMP_PACKET_SIZE_MEDIUM;
    LOGE("VideoStreamPacket:%p",packet);

    mVideoCallback(packet);

}

void VideoStreamPacket::sendFrame(int type, uint8_t *payload, int i_payload) {
    if (payload[2] == 0x00) {
        i_payload -= 4;
        payload += 4;
    } else {
        i_payload -= 3;
        payload += 3;
    }
    int i = 0;
    int bodySize = 9 + i_payload;
    auto *packet = new RTMPPacket();
    RTMPPacket_Alloc(packet, bodySize);

    if (type == NAL_SLICE_IDR) {
        packet->m_body[i++] = 0x17; // 1:Key frame  7:AVC
    } else {
        packet->m_body[i++] = 0x27; // 2:None key frame 7:AVC
    }
    //AVC NALU
    packet->m_body[i++] = 0x01;
    //timestamp
    packet->m_body[i++] = 0x00;
    packet->m_body[i++] = 0x00;
    packet->m_body[i++] = 0x00;
    //packet len
    packet->m_body[i++] = (i_payload >> 24) & 0xFF;
    packet->m_body[i++] = (i_payload >> 16) & 0xFF;
    packet->m_body[i++] = (i_payload >> 8) & 0xFF;
    packet->m_body[i++] = (i_payload) & 0xFF;

    memcpy(&packet->m_body[i], payload, static_cast<size_t>(i_payload));

    packet->m_hasAbsTimestamp = 0;
    packet->m_nBodySize = bodySize;
    packet->m_packetType = RTMP_PACKET_TYPE_VIDEO;
    packet->m_nChannel = 0x10;
    packet->m_headerType = RTMP_PACKET_SIZE_LARGE;

    mVideoCallback(packet);

}

void VideoStreamPacket::callbackStatusMsg(const char *msg, float codeErr) {
    if (mContext != nullptr && mStatusCallback != nullptr)
        mStatusCallback(mContext, msg, 0);
}


