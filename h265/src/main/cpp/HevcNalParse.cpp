//  Author : wangyongyao https://github.com/wangyongyao1989
// Created by MMM on 2025/3/18.
//

#include "h265include/HevcNalParse.h"
#include "h265_bitstream_parser.h"

HevcNalParse::~HevcNalParse() {


}

HevcNalParse::HevcNalParse() {

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
    std::vector<uint8_t> buffer(size);
    fread(reinterpret_cast<char *>(buffer.data()), 1, size, infp);

    fclose(infp);

    LOGE("buffer.data() :%d", buffer.size());
    // parse bitstream
    std::unique_ptr<h265nal::H265BitstreamParser::BitstreamState> bitstream =
            h265nal::H265BitstreamParser::ParseBitstream(
                    buffer.data(), buffer.size(), &bitstream_parser_state, false);


    //宽高
    LOGE("SPS:pic_width_in_luma_samples %d", bitstream_parser_state
            .GetSps(0)->pic_width_in_luma_samples);
    LOGE("SPS:pic_height_in_luma_samples %d",
         bitstream_parser_state.GetSps(0)->pic_height_in_luma_samples);
    //色度格式
    LOGE("SPS:chromaFormatIdc %d",
         bitstream_parser_state.GetSps(0)->chroma_format_idc);

    uint32_t frameRate;
    uint32_t vuiParametersPresentFlag = bitstream_parser_state.GetSps(0)
            ->vui_parameters->vui_timing_info_present_flag;
    if (vuiParametersPresentFlag) {
        uint32_t numUnitsInTick = bitstream_parser_state.GetSps(0)
                ->vui_parameters->vui_num_units_in_tick;
        uint32_t timeScale = bitstream_parser_state.GetSps(0)
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
    LOGE("SPS:KEY_FRAME_RATE %d", frameRate);

    uint32_t  log2 = bitstream_parser_state.GetSps(0)->log2_max_pic_order_cnt_lsb_minus4;
    LOGE("SPS:log2_max_pic_order_cnt_lsb_minus4: %d", log2);
    return 0;
}


