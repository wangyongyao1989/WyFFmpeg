//  Author : wangyongyao https://github.com/wangyongyao1989
// Created by MMM on 2025/3/25.
//

#include "Hevc2MP4.h"


Hevc2MP4::Hevc2MP4() {
    hevcNalParse = new HevcNalParse();

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

    configMuxer(inPath);

    writeSampleData(hevcNalParse->bitstream_buffer);

    //写入stream末尾标志位
    AMediaCodecBufferInfo *end_info = new AMediaCodecBufferInfo();
    end_info->offset = 0;
    end_info->size = 0;
    end_info->flags = AMEDIACODEC_BUFFER_FLAG_END_OF_STREAM;
    end_info->presentationTimeUs = computePresentationTime(frameIndex++);
    LOGE("写入末尾==========");

    media_status_t status = AMediaMuxer_writeSampleData(m_AMediaMuxer, videoTrackIndex, end_data,
                                                        reinterpret_cast<const AMediaCodecBufferInfo *>(&end_info));
    LOGE("生成MP4完成:%d",status);

    fclose(m_in_path);
    release();
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
    if (m_in_path != nullptr) {
        fclose(m_in_path);
        m_in_path = nullptr;
    }
    if (m_MediaMuxer_fp != nullptr) {
        fclose(m_MediaMuxer_fp);
        m_MediaMuxer_fp = nullptr;
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
    hevcNalParse->setHevcNalDataPath(inPath);
    std::unique_ptr<h265nal::H265BitstreamParser::BitstreamState> &ptr_bitstream = hevcNalParse->bitstream;

    for (int i = 0; i < ptr_bitstream->nal_units.size(); ++i) {
        m_nal_units.push_back(std::move(ptr_bitstream->nal_units.at(i)));
    }

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

    const std::pair<const void *, size_t> &csd_0 = hevcNalParse->parseH265CSD0(
            hevcNalParse->bitstream_buffer);

    AMediaFormat_setBuffer(m_AMediaFormat, "csd-0", csd_0.first, csd_0.second);


    // 新建一个复合输出
    m_AMediaMuxer = AMediaMuxer_new(m_MediaMuxer_fd, AMEDIAMUXER_OUTPUT_FORMAT_MPEG_4);
    // 开始写入
    videoTrackIndex = AMediaMuxer_addTrack(m_AMediaMuxer, m_AMediaFormat);
    if (videoTrackIndex < 0) {
        LOGE("Failed to add video track");
        release();
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

int Hevc2MP4::writeSampleData(std::vector<uint8_t> *bitstream_buffer) {
    if (!bitstream_buffer) {
        LOGE("传入的比特流缓冲区为空指针。");
        return -1;
    }

    std::vector<uint8_t> &stream = *bitstream_buffer;
    std::vector<uint8_t> csd;
    size_t pos = 0;
    AMediaCodecBufferInfo *info;

    while (pos < stream.size()) {
        // 查找起始码 0x00 0x00 0x00 0x01 或 0x00 0x00 0x01
        size_t startCodeSize = 0;
        if (pos + 3 < stream.size() && stream[pos] == 0x00 && stream[pos + 1] == 0x00 &&
            stream[pos + 2] == 0x01) {
            startCodeSize = 3;
        } else if (pos + 4 < stream.size() && stream[pos] == 0x00 && stream[pos + 1] == 0x00 &&
                   stream[pos + 2] == 0x00 && stream[pos + 3] == 0x01) {
            startCodeSize = 4;
        }
        if (startCodeSize == 0) {
            pos++;
            continue;
        }

        // 查找下一个起始码或流结束
        size_t nextStart = pos + startCodeSize;
        while (nextStart < stream.size()) {
            if (nextStart + 3 < stream.size() && stream[nextStart] == 0x00 &&
                stream[nextStart + 1] == 0x00 && stream[nextStart + 2] == 0x01) {
                break;
            } else if (nextStart + 4 < stream.size() && stream[nextStart] == 0x00 &&
                       stream[nextStart + 1] == 0x00 && stream[nextStart + 2] == 0x00 &&
                       stream[nextStart + 3] == 0x01) {
                break;
            }
            nextStart++;
        }

        // 提取当前 NAL 单元
        std::vector<uint8_t> nalUnit(stream.begin() + pos, stream.begin() + nextStart);
        LOGW("nalUnit======:%d", nalUnit.size());

        info = new AMediaCodecBufferInfo();
        info->offset = 0;
        info->size = nalUnit.size();
        info->flags = 0;
        if (isKeyFrame(nalUnit)) {
            LOGW("关键帧为===================");
            info->flags = AMEDIACODEC_CONFIGURE_FLAG_ENCODE;
        }

        info->presentationTimeUs = computePresentationTime(frameIndex++);
        LOGW("info->presentationTimeUs:%lld", info->presentationTimeUs);

        media_status_t status = AMediaMuxer_writeSampleData(m_AMediaMuxer, videoTrackIndex,
                                                            nalUnit.data(),
                                                            reinterpret_cast<const AMediaCodecBufferInfo *>(&info));
        LOGW("解析csd-AMediaMuxer_writeSampleData=== %d", status);
        pos = nextStart;
    }

    LOGW("完成 AMediaMuxer_writeSampleData :%d", csd.size());

    return 0;
}

// 检查是否为关键帧
bool Hevc2MP4::isKeyFrame(const std::vector<uint8_t> &nalUnit) {
    int nalUnitType = getNalUnitType(nalUnit);
    return (nalUnitType >= 16 && nalUnitType <= 23);
}

// 获取 NAL 单元类型
int Hevc2MP4::getNalUnitType(const std::vector<uint8_t> &nalUnit) {
    if (nalUnit.size() < 5) return -1;
    return (nalUnit[4] & 0x7E) >> 1;
}

