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
        delete bitstream_buffer;// 构造函数用的是 new std::vector（非数组），原 delete[] 属于未定义行为
        bitstream_buffer = nullptr;
    }

    m_csd0.clear();

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
    LOGE("parseH265CSD0:%d",stream.size());
    m_csd0.clear();// 使用成员缓冲区，保证返回的数据在对象生命周期内有效
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

        // 根据 NAL 单元类型处理。
        // MPEG4Writer 的 reassembleHVCC 要求 csd-0 内 NAL 间必须是 4 字节起始码
        //（3 字节码会切出带前导 0x00 的 NAL，makeHvcc 失败 → stop 报 Missing codec specific data），
        // 因此统一规范化后再拼接。
        switch (nalUnitType) {
            case 32: // VPS
                LOGD("VPS======");
                appendNormalizedNal(m_csd0, nalUnit);
                isGetVsp = true;
                break;
            case 33: // SPS
                LOGD("SPS======");
                appendNormalizedNal(m_csd0, nalUnit);
                isGetSps = true;
                break;
            case 34: // PPS
                LOGD("PPS======");
                appendNormalizedNal(m_csd0, nalUnit);
                isGetPsp = true;
                break;
            default:
                break;
        }
        pos = nextStart;
        if (isGetVsp && isGetSps && isGetPsp) break;
    }

    LOGW("解析csd-0的大小为:%d", m_csd0.size());

    return {m_csd0.data(), m_csd0.size()};
}


// 起始码后的 NAL 载荷下标：3 字节码返回 3、4 字节码返回 4，识别失败返回 -1
int HevcNalParse::payloadIndex(const std::vector<uint8_t> &nalUnit) {
    if (nalUnit.size() >= 5 && nalUnit[0] == 0x00 && nalUnit[1] == 0x00 &&
        nalUnit[2] == 0x00 && nalUnit[3] == 0x01) {
        return 4;
    }
    if (nalUnit.size() >= 4 && nalUnit[0] == 0x00 && nalUnit[1] == 0x00 &&
        nalUnit[2] == 0x01) {
        return 3;
    }
    return -1;
}

// 规范化为 4 字节起始码并去掉边界切分残留的尾部 0x00
//（4 字节码的第 4 个起始字节会被划入上一 NAL 尾部）
void HevcNalParse::appendNormalizedNal(std::vector<uint8_t> &out,
                                       const std::vector<uint8_t> &nalUnit) {
    int start = payloadIndex(nalUnit);
    if (start < 0) return;
    size_t end = nalUnit.size();
    while (end > (size_t) start && nalUnit[end - 1] == 0x00) end--;
    const uint8_t sc[4] = {0x00, 0x00, 0x00, 0x01};
    out.insert(out.end(), sc, sc + 4);
    out.insert(out.end(), nalUnit.begin() + start, nalUnit.begin() + end);
}


// 获取 NAL 单元类型
int HevcNalParse::getNalUnitType(const std::vector<uint8_t> &nalUnit) {
    int idx = payloadIndex(nalUnit);
    if (idx < 0 || (size_t) idx >= nalUnit.size()) return -1;
    return (nalUnit[idx] & 0x7E) >> 1;
}


