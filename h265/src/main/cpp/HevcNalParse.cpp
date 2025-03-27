//  Author : wangyongyao https://github.com/wangyongyao1989
// Created by MMM on 2025/3/18.
//

#include "h265include/HevcNalParse.h"
#include "h265_bitstream_parser.h"

HevcNalParse::~HevcNalParse() {
    release();
}

HevcNalParse::HevcNalParse() {
    bitstream_parser_state = new h265nal::H265BitstreamParserState;
    bitstream_buffer = new std::vector<uint8_t>();
}


void HevcNalParse::release() {
    if (bitstream_parser_state) {
        delete bitstream_parser_state;
        bitstream_parser_state = nullptr;
    }

    if (bitstream_buffer) {
        delete[] bitstream_buffer;
        bitstream_buffer = nullptr;
    }

}


int HevcNalParse::setHevcNalDataPath(const char *dataPath) {

    // read infile
    // TODO(chemag): read the infile incrementally
    FILE *infp = fopen(dataPath, "rb");
    if (infp == nullptr) {
        // did not work
        fprintf(stderr, "Could not open input file: \"%s\"\n", dataPath);
        return -1;
    }

    fseek(infp, 0, SEEK_END);
    int64_t size = ftell(infp);
    fseek(infp, 0, SEEK_SET);
    // read file into buffer
    bitstream_buffer->resize(size);
    fread(reinterpret_cast<char *>(bitstream_buffer->data()), 1, size, infp);

    fclose(infp);

    LOGD("buffer.data() :%d", bitstream_buffer->size());
    // parse bitstream
    bitstream = h265nal::H265BitstreamParser::ParseBitstream(
            bitstream_buffer->data(), bitstream_buffer->size(), bitstream_parser_state, false);

    LOGD("bitstream->nal_units.size()%d", bitstream->nal_units.size());

    //宽高
    LOGD("SPS:pic_width_in_luma_samples %d", bitstream_parser_state
            ->GetSps(0)->pic_width_in_luma_samples);
    LOGD("SPS:pic_height_in_luma_samples %d",
         bitstream_parser_state->GetSps(0)->pic_height_in_luma_samples);
    //色度格式
    LOGD("SPS:chromaFormatIdc %d",
         bitstream_parser_state->GetSps(0)->chroma_format_idc);

    uint32_t frameRate;
    uint32_t vuiParametersPresentFlag = bitstream_parser_state->GetSps(0)
            ->vui_parameters->vui_timing_info_present_flag;
    if (vuiParametersPresentFlag) {
        uint32_t numUnitsInTick = bitstream_parser_state->GetSps(0)
                ->vui_parameters->vui_num_units_in_tick;
        uint32_t timeScale = bitstream_parser_state->GetSps(0)
                ->vui_parameters->vui_time_scale;
        if (numUnitsInTick > 0) {
            frameRate = timeScale / (1.0 * numUnitsInTick); // 帧率公式
        } else {
            frameRate = 30;
        }
    } else {
        frameRate = 30;
    }

    //帧率
    LOGD("SPS:KEY_FRAME_RATE %d", frameRate);

    uint32_t log2 = bitstream_parser_state->GetSps(0)->log2_max_pic_order_cnt_lsb_minus4;
    LOGD("SPS:log2_max_pic_order_cnt_lsb_minus4: %d", log2);
    return 0;
}

// 解析 H.265 文件流，返回 CSD 数据和其大小
std::pair<const void *, size_t>
HevcNalParse::parseH265CSD0(std::vector<uint8_t> *bitstream_buffer) {
    if (!bitstream_buffer) {
        LOGE("传入的比特流缓冲区为空指针。");
        return {nullptr, 0};
    }

    std::vector<uint8_t> &stream = *bitstream_buffer;
    std::vector<uint8_t> csd;
    size_t pos = 0;
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
        int nalUnitType = getNalUnitType(nalUnit);

        // 根据 NAL 单元类型处理
        switch (nalUnitType) {
            case 32: // VPS
                LOGD("VPS======");
                csd.insert(csd.end(), nalUnit.begin(), nalUnit.end());
                isGetVsp = true;
                break;
            case 33: // SPS
                LOGD("SPS======");
                csd.insert(csd.end(), nalUnit.begin(), nalUnit.end());
                isGetSps = true;
                break;
            case 34: // PPS
                LOGD("PPS======");
                csd.insert(csd.end(), nalUnit.begin(), nalUnit.end());
                isGetPsp = true;
                break;
            default:
                break;
        }
        pos = nextStart;
        if (isGetVsp && isGetSps && isGetPsp) break;
    }

    LOGW("解析csd-0的大小为:%d", csd.size());

    return {csd.data(), csd.size()};
}


// 获取 NAL 单元类型
int HevcNalParse::getNalUnitType(const std::vector<uint8_t> &nalUnit) {
    if (nalUnit.size() < 5) return -1;
    return (nalUnit[4] & 0x7E) >> 1;
}


