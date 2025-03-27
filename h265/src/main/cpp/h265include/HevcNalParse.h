//  Author : wangyongyao https://github.com/wangyongyao1989
// Created by MMM on 2025/3/18.
//

#ifndef MYYFFMPEG_HEVCNALPARSE_H
#define MYYFFMPEG_HEVCNALPARSE_H

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>

#include <climits>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>

#include "../h265include/h265nal/h265_bitstream_parser_state.h"


#include "HevcLogUtils.h"
#include "h265_bitstream_parser.h"


class HevcNalParse {


public:
    HevcNalParse();

    ~HevcNalParse();

    int setHevcNalDataPath(const char *dataPath);

    void release();

    int getNalUnitType(const std::vector<uint8_t>& nalUnit);

    std::pair<const void*, size_t> parseH265CSD0(std::vector<uint8_t> *bitstream_buffer);

    struct h265nal::H265BitstreamParserState *bitstream_parser_state;

    std::unique_ptr<h265nal::H265BitstreamParser::BitstreamState> bitstream;

    std::vector<uint8_t> *bitstream_buffer;
private:


    bool isGetVsp = false;
    bool isGetSps = false;
    bool isGetPsp = false;



};


#endif //MYYFFMPEG_HEVCNALPARSE_H
