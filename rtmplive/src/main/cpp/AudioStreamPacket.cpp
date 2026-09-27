//
// Created by MMM on 2024/7/17.
//

#include "AudioStreamPacket.h"
#include <cstdlib>    // faacEncGetDecoderSpecificInfo 返回的缓冲用 free() 释放

AudioStreamPacket::AudioStreamPacket() {

}

int AudioStreamPacket::setAudioEncInfo(int samplesInHZ, int channels) {
    callbackStatusMsg("AudioStreamPacket setAudioEncInfo", 0);
    m_channels = channels;
    //open faac encoder
    m_audioCodec = faacEncOpen(static_cast<unsigned long>(samplesInHZ),
                               static_cast<unsigned int>(channels),
                               &m_inputSamples,
                               &m_maxOutputBytes);
    if (m_audioCodec == nullptr) {
        // 采样率/声道数组合不被 faac 支持时返回空句柄，后续 faacEncEncode 会直接段错误
        LOGE("faacEncOpen failed: rate=%d channels=%d", samplesInHZ, channels);
        m_inputSamples = 0;
        m_maxOutputBytes = 0;
        return -1;
    }
    m_buffer = new u_char[m_maxOutputBytes];

    //set encoder params
    faacEncConfigurationPtr config = faacEncGetCurrentConfiguration(m_audioCodec);
    config->mpegVersion = MPEG4;    //设置版本,录制MP4文件时要用MPEG4
    config->aacObjectType = LOW;     //编码类型
    config->inputFormat = FAAC_INPUT_16BIT;     //输入数据类型
    config->outputFormat = 0;
    return faacEncSetConfiguration(m_audioCodec, config);
}


int AudioStreamPacket::getInputSamples() const {
    return static_cast<int>(m_inputSamples);
}

void AudioStreamPacket::encodeData(int8_t *data) {
    if (m_audioCodec == nullptr || m_buffer == nullptr) {
        // faacEncOpen 失败时句柄为空，继续编码会段错误
        return;
    }
//    callbackStatusMsg("AudioStreamPacket encodeData", 0);

    //encode a frame, and return encoded len
    int byteLen = faacEncEncode(m_audioCodec, reinterpret_cast<int32_t *>(data),
                                static_cast<unsigned int>(m_inputSamples),
                                m_buffer,
                                static_cast<unsigned int>(m_maxOutputBytes));
    if (byteLen > 0) {
        int bodySize = 2 + byteLen;
        auto *packet = new RTMPPacket();
        RTMPPacket_Alloc(packet, bodySize);
        //stereo
        packet->m_body[0] = 0xAF;
        if (m_channels == 1) {
            packet->m_body[0] = 0xAE;
        }

        packet->m_body[1] = 0x01;
        memcpy(&packet->m_body[2], m_buffer, static_cast<size_t>(byteLen));

        packet->m_hasAbsTimestamp = 0;
        packet->m_nBodySize = bodySize;
        packet->m_packetType = RTMP_PACKET_TYPE_AUDIO;
        packet->m_nChannel = 0x11;
        packet->m_headerType = RTMP_PACKET_SIZE_LARGE;
        if (mContext != nullptr && mAudioCallback != nullptr) {
            mAudioCallback(mContext, packet);
        }
    }
}

RTMPPacket *AudioStreamPacket::getAudioTag() {
    callbackStatusMsg("AudioStreamPacket getAudioTag", 0);

    u_char *buf = nullptr;
    u_long len = 0;
    // faacEncGetDecoderSpecificInfo 的成功值是 0，失败才是负数
    //（真机实测：44100/2ch 返回 rc=0, len=2, ASC=0x12 0x10，即 AAC-LC 双声道 44.1k）。
    // 若按 rc<=0 判失败，音频序列头就不会发出，服务器只建了视频轨，
    // 随后第一个音频帧到达即报 "received a packet for audio track 0, but track is not set up" 并断开。
    int ascRc = m_audioCodec ? static_cast<int>(faacEncGetDecoderSpecificInfo(m_audioCodec, &buf, &len)) : -1;
    if (ascRc < 0 || buf == nullptr || len == 0) {
        LOGE("faacEncGetDecoderSpecificInfo failed: rc=%d len=%lu, skip audio sequence header",
             ascRc, len);
        free(buf);
        return nullptr;
    }
    int bodySize = static_cast<int>(2 + len);
    auto *packet = new RTMPPacket();
    RTMPPacket_Alloc(packet, bodySize);
    //channel layout: stereo
    packet->m_body[0] = 0xAF;
    if (m_channels == 1) {
        packet->m_body[0] = 0xAE;
    }
    packet->m_body[1] = 0x00;

    memcpy(&packet->m_body[2], buf, len);
    // faac 用 malloc 分配 ASC，文档要求调用方 free，原来直接丢掉指针每推一次漏一份
    free(buf);

    packet->m_hasAbsTimestamp = 0;
    packet->m_nBodySize = bodySize;
    packet->m_packetType = RTMP_PACKET_TYPE_AUDIO;
    packet->m_nChannel = 0x11;
    packet->m_headerType = RTMP_PACKET_SIZE_LARGE;

    return packet;
}


void AudioStreamPacket::setRtmpStatusCallback(void *context,
                                              AudioStreamPacket::RtmpStatusCallback callback) {
    mContext = context;
    mStatusCallback = callback;
}

void
AudioStreamPacket::setAudioCallback(void *context, AudioStreamPacket::AudioCallback audioCallback) {
    mContext = context;
    mAudioCallback = audioCallback;
}

void AudioStreamPacket::callbackStatusMsg(const char *msg, float codeErr) {
    if (mContext != nullptr && mStatusCallback != nullptr)
        mStatusCallback(mContext, msg, 0);
}

AudioStreamPacket::~AudioStreamPacket() {
    // m_buffer 是 new u_char[] 出来的，必须用 delete[] 配对
    delete[] m_buffer;
    m_buffer = nullptr;
    if (m_audioCodec) {
        faacEncClose(m_audioCodec);
        m_audioCodec = nullptr;
    }
}




