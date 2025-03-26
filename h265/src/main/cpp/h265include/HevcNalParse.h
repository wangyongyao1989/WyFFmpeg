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


class HevcNalParse {


public:
    HevcNalParse();

    ~HevcNalParse();

    int setHevcNalDataPath(const char *dataPath);

    void release();


    struct h265nal::H265BitstreamParserState *bitstream_parser_state;

private:


};


#endif //MYYFFMPEG_HEVCNALPARSE_H
