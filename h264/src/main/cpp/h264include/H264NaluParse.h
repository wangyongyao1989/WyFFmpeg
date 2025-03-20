//  Author : wangyongyao https://github.com/wangyongyao1989
// Created by MMM on 2025/3/20.
//

#ifndef MYYFFMPEG_H264NALUPARSE_H
#define MYYFFMPEG_H264NALUPARSE_H

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>

#include <climits>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>

#include "H264LogUtils.h"
#include "h264nal/h264_common.h"
#include "../webrtc/bit_buffer.h"
#include "h264nal/h264_bitstream_parser_state.h"

class H264NaluParse {
public:
    H264NaluParse();

    ~H264NaluParse();

    int setHevcNalDataPath(const char *dataPath);

private:
    struct h264nal::H264BitstreamParserState bitstream_parser_state;
};


#endif //MYYFFMPEG_H264NALUPARSE_H
