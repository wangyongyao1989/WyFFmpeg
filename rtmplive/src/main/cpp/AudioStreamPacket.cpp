//
// Created by MMM on 2024/7/17.
//

#include "AudioStreamPacket.h"

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

    u_char *buf;
    u_long len;
    faacEncGetDecoderSpecificInfo(m_audioCodec, &buf, &len);
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
    delete m_buffer;
    m_buffer = nullptr;
    if (m_audioCodec) {
        faacEncClose(m_audioCodec);
        m_audioCodec = nullptr;
    }
}




