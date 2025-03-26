//  Author : wangyongyao https://github.com/wangyongyao1989
// Created by MMM on 2025/3/25.
//

#include "Hevc2MP4.h"


Hevc2MP4::Hevc2MP4() {
    if (hevcNalParse == nullptr) {
        hevcNalParse = new HevcNalParse();
    }

    bitstream_parser_state = new H265BitstreamParserState;

}

Hevc2MP4::~Hevc2MP4() {
    release();
}

int Hevc2MP4::hevcConverterMp4(const char *inPath, const char *outPath) {
    m_in_path = fopen(inPath, "rb");
    if (!m_in_path) {
        LOGE("Failed to open input file: %s", inPath);
        return -1;
    }

    m_MediaMuxer_fp = fopen(outPath, "wb+");// 打开新建一个文件。
    if (m_MediaMuxer_fp == nullptr) {
        LOGE("MediaCodecMuxer:: outPath file fopen err!");
        return -1;
    }

    // 由于muexr的原因，这里需要转换一下。
    m_MediaMuxer_fd = fileno(m_MediaMuxer_fp);
    if (m_MediaMuxer_fd < 0) {
        LOGE("MediaCodecMuxer:: Mp4 file open err! = %d", m_MediaMuxer_fd);
        return -1;
    }


    LOGD(" AMediaMuxer_new OK");
    const int bufferSize = 1024 * 1024;
    uint8_t *buffer = new uint8_t[bufferSize];
    size_t bytesRead;
    int presentationTimeUs = 0;


    while ((bytesRead = fread(buffer, 1, bufferSize, m_in_path)) > 0) {
        LOGE("bytesRead %d:", bytesRead);

        if (!isGetCsd) {
            LOGE("configMuxer");
            configMuxer(inPath);
        }



        AMediaCodecBufferInfo *info = new AMediaCodecBufferInfo();
        info->offset = 0;
        info->size = 12;
        info->flags = 0;

        info->presentationTimeUs = computePresentationTime(frameIndex++);

        uint8_t *encodeData = (buffer);

        AMediaMuxer_writeSampleData(m_AMediaMuxer, videoTrackIndex, encodeData,
                                    reinterpret_cast<const AMediaCodecBufferInfo *>(&info));
    }

    delete[] buffer;
    LOGE("AMEDIACODEC_BUFFER_FLAG_END_OF_STREAM");

    //写入stream末尾标志位
    AMediaCodecBufferInfo *info = new AMediaCodecBufferInfo();
    info->offset = 0;
    info->size = 0;
    info->flags = AMEDIACODEC_BUFFER_FLAG_END_OF_STREAM;
    info->presentationTimeUs = computePresentationTime(frameIndex++);

    AMediaMuxer_writeSampleData(m_AMediaMuxer, videoTrackIndex, 0,
                                reinterpret_cast<const AMediaCodecBufferInfo *>(&info));
    LOGE("生成MP4完成");


    return 0;
}


void Hevc2MP4::release() {
    if (m_AMediaMuxer != nullptr) {
        AMediaMuxer_stop(m_AMediaMuxer);
    }

    if (m_AMediaMuxer != nullptr) {
        AMediaMuxer_delete(m_AMediaMuxer);
        m_AMediaMuxer = nullptr;
    }

    if (m_MediaMuxer_fp != nullptr) {
        delete m_MediaMuxer_fp;
        m_MediaMuxer_fp = nullptr;
    }

    if (hevcNalParse != nullptr) {
        hevcNalParse->release();
        hevcNalParse = nullptr;
    }

    if (bitstream_parser_state) {
        delete bitstream_parser_state;
        bitstream_parser_state = nullptr;
    }
}


bool Hevc2MP4::isVps(const uint8_t *data,
                     size_t length) {


    std::vector<H265BitstreamParser::NaluIndex> nalu_indices
            = H265BitstreamParser::FindNaluIndices(data, length);

    // 读取NAL头字节
    H265BitstreamParser::NaluIndex &naluIndex = nalu_indices.front();
    auto nal_unit = H265NalUnitParser::ParseNalUnit(
            &data[naluIndex.payload_start_offset], naluIndex.payload_size,
            bitstream_parser_state, false);
    LOGE("isVps:%d", nal_unit->nal_unit_header->nal_unit_type);
    if (nal_unit->nal_unit_header->nal_unit_type == VPS_NUT) {
        return true;
    } else {
        return false;
    }
}

void Hevc2MP4::configMuxer(const char *inPath) {
    //
    hevcNalParse->setHevcNalDataPath(inPath);

//    std::unique_ptr<h265nal::H265BitstreamParser::BitstreamState> &ptr_bitstream = hevcNalParse->bitstream;
//
//    for (int i = 0; i < ptr_bitstream->nal_units.size(); ++i) {
//        m_nal_units.push_back(std::move(ptr_bitstream->nal_units.at(i)));
//    }
//    LOGE("m_nal_units %d!",m_nal_units.size());


    
    if (hevcNalParse->bitstream_parser_state == nullptr) {
        LOGE("bitstream_parser_state err!");
        return;
    }
    H265SpsParser::SpsState *pSpsState = hevcNalParse->bitstream_parser_state
            ->GetSps(0).get();


    uint32_t width = pSpsState->pic_width_in_luma_samples;
    uint32_t height = pSpsState->pic_height_in_luma_samples;

    uint32_t frameRate;
    uint32_t vuiParametersPresentFlag = pSpsState->vui_parameters->vui_timing_info_present_flag;
    if (vuiParametersPresentFlag) {
        uint32_t numUnitsInTick = pSpsState->vui_parameters->vui_num_units_in_tick;
        uint32_t timeScale = pSpsState->vui_parameters->vui_time_scale;
        if (numUnitsInTick > 0) {
            frameRate = timeScale / (1.0 * numUnitsInTick); // 帧率公式
        } else {
            frameRate = 30;
        }
    } else {
        frameRate = 30;
    }

    LOGE("bitstream_parser_state的大小:%d",hevcNalParse->bitstream_parser_state->vps.size());



    m_AMediaFormat = AMediaFormat_new();
    // H.265 Advanced Video Coding
    AMediaFormat_setString(m_AMediaFormat, AMEDIAFORMAT_KEY_MIME, MIME_TYPE);
    AMediaFormat_setInt32(m_AMediaFormat, AMEDIAFORMAT_KEY_WIDTH, width);
    AMediaFormat_setInt32(m_AMediaFormat, AMEDIAFORMAT_KEY_HEIGHT, height);
    AMediaFormat_setInt32(m_AMediaFormat, AMEDIAFORMAT_KEY_COLOR_FORMAT,
                          COLOR_FormatYUV420Flexible);
    AMediaFormat_setInt32(m_AMediaFormat, AMEDIAFORMAT_KEY_BIT_RATE, BIT_RATE);
    // 30fps
    AMediaFormat_setInt32(m_AMediaFormat, AMEDIAFORMAT_KEY_FRAME_RATE, frameRate);
    // 5 seconds between I-frames
    AMediaFormat_setInt32(m_AMediaFormat, AMEDIAFORMAT_KEY_I_FRAME_INTERVAL, 5);




//    for (NalUnit nal : nalUnits) {
//        switch (nal.type) {
//            case NAL_UNIT_TYPE_VPS:
//                vps = ByteBuffer.wrap(nal.data);
//                byte[] array = vps.array();
//                Log.d(TAG, "NAL_UNIT_TYPE_VPS ===:" + Arrays.toString(array));
//                break;
//            case NAL_UNIT_TYPE_SPS:
//                sps = ByteBuffer.wrap(nal.data);
//                break;
//            case NAL_UNIT_TYPE_PPS:
//                pps = ByteBuffer.wrap(nal.data);
//                break;
//        }
//    }

//    AMediaFormat_setBuffer(m_AMediaFormat, AMEDIAFORMAT_KEY_CSD_0, buffer, buffer);

    // 新建一个复合输出
    m_AMediaMuxer = AMediaMuxer_new(m_MediaMuxer_fd, AMEDIAMUXER_OUTPUT_FORMAT_MPEG_4);
    // 开始写入
    videoTrackIndex = AMediaMuxer_addTrack(m_AMediaMuxer, m_AMediaFormat);
    if (videoTrackIndex < 0) {
        LOGE("Failed to add video track");
        release();
        fclose(m_in_path);
        return;
    }

    // 创建 MediaMuxer 封装器，用于将编码后的数据封装成 MP4 文件
    isGetCsd = true;
    AMediaMuxer_start(m_AMediaMuxer);
}

long Hevc2MP4::computePresentationTime(long frameIndex) {
    long frameIndexTime = frameIndex * 1000000 / FRAME_RATE;
    long time = 42 + frameIndexTime;
    return time;
}
