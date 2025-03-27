//  Author : wangyongyao https://github.com/wangyongyao1989
// Created by MMM on 2025/3/20.
//

#include "h264include/H264NaluParse.h"
#include "h264include/h264nal/h264_bitstream_parser.h"

H264NaluParse::H264NaluParse() {
    bitstream_parser_state = new h264nal::H264BitstreamParserState;
}

H264NaluParse::~H264NaluParse() {
    release();
}


int H264NaluParse::setHevcNalDataPath(const char *dataPath) {
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
    std::vector<uint8_t> buffer(size);
    fread(reinterpret_cast<char *>(buffer.data()), 1, size, infp);

    fclose(infp);

    LOGE("buffer.data() :%d", buffer.size());

    // parse bitstream
    std::unique_ptr<h264nal::H264BitstreamParser::BitstreamState> bitstream =
            h264nal::H264BitstreamParser::ParseBitstream(
                    buffer.data(), buffer.size(), bitstream_parser_state, false);

    uint32_t pic_height_in_map_units_minus1 = bitstream_parser_state->GetSps(0)
            ->sps_data
            ->pic_height_in_map_units_minus1;

    uint32_t pic_width_in_mbs_minus1 = bitstream_parser_state->GetSps(0)
            ->sps_data
            ->pic_width_in_mbs_minus1;
    uint32_t frame_mbs_only_flag = bitstream_parser_state->GetSps(0)
            ->sps_data->frame_mbs_only_flag;
    // 计算宽高
    int width = (pic_width_in_mbs_minus1 + 1) * 16;
    int height = (pic_height_in_map_units_minus1 + 1) * 16 * (2 - frame_mbs_only_flag);
    //宽高
    LOGE("SPS:height %d", height);
    LOGE("SPS:width %d", width);
    //色度格式
    LOGE("SPS:chroma_format_idc %d",
         bitstream_parser_state->GetSps(0)
                 ->sps_data->chroma_format_idc);

    uint32_t frameRate;
    uint32_t vuiParametersPresentFlag = bitstream_parser_state->GetSps(0)->sps_data
            ->vui_parameters->time_scale;
    if (vuiParametersPresentFlag) {
        uint32_t numUnitsInTick = bitstream_parser_state->GetSps(0)->sps_data
                ->vui_parameters->num_units_in_tick;
        uint32_t timeScale = bitstream_parser_state->GetSps(0)->sps_data
                ->vui_parameters->time_scale;
        if (numUnitsInTick > 0) {
            frameRate = timeScale / (2.0 * numUnitsInTick); // 帧率公式
            LOGE("SPS:========== frameRate%d", frameRate);
        } else {
            frameRate = 30;
        }
    } else {
        frameRate = 30;
    }

    //帧率
    LOGE("SPS:KEY_FRAME_RATE %d", frameRate);

    return 0;
}

void H264NaluParse::release() {
    if (bitstream_parser_state) {
        delete bitstream_parser_state;
        bitstream_parser_state = nullptr;
    }
}
