//  Author : wangyongyao https://github.com/wangyongyao1989
// Created by MMM on 2025/3/25.
//

#include "HevcLogUtils.h"
#include <cstdint>
#include <stdio.h>

#include "media/NdkMediaCrypto.h"
#include "media/NdkMediaCodec.h"
#include "media/NdkMediaError.h"
#include "media/NdkMediaFormat.h"
#include "media/NdkMediaMuxer.h"
#include "media/NdkMediaExtractor.h"


#include "HevcNalParse.h"
#include "h265_bitstream_parser.h"


#ifndef MYYFFMPEG_HEVC2MP4_H
#define MYYFFMPEG_HEVC2MP4_H

#define COLOR_FormatYUV420Flexible 0x7F420888


using namespace h265nal;

class Hevc2MP4 {
public:
    Hevc2MP4();

    ~Hevc2MP4();

    int hevcConverterMp4(const char *inPath, const char *outPath);

    void release();

private:

    long computePresentationTime(long frameIndex);

    bool isVps(const uint8_t *data, size_t length);

    void configMuxer(const char *inPath);

    int writeSampleData(std::vector<uint8_t> *bitstream_buffer);

    int getNalUnitType(const std::vector<uint8_t> &nalUnit);

    bool isKeyFrame(const std::vector<uint8_t>& nalUnit);

    const char *MIME_TYPE = "video/hevc";
    static const size_t VIDEO_WIDTH = 1280;
    static const size_t VIDEO_HEIGHT = 720;
    static const size_t BIT_RATE = 4000000;   // 4Mbps
    long FRAME_RATE = 25;

    long frameIndex = 0;

    bool isGetCsd = false;

    FILE *m_in_path;

    int videoTrackIndex;

    AMediaMuxer *m_AMediaMuxer = nullptr;
    FILE *m_MediaMuxer_fp = nullptr;
    size_t m_MediaMuxer_fd = 0;

    AMediaFormat *m_AMediaFormat = nullptr;

    HevcNalParse *hevcNalParse = nullptr;

    struct H265BitstreamParserState *bitstream_parser_state = nullptr;
    std::vector<std::unique_ptr<struct H265NalUnitParser::NalUnitState>>
            m_nal_units;
    // 创建一个包含 10 个元素且初始值为 0 的 vector
    const uint8_t zeroArray[1] = {0};
    // 让 data 指向 vector 的数据
    const uint8_t *end_data = zeroArray;
//    const uint8_t *end_data;
};


#endif //MYYFFMPEG_HEVC2MP4_H
