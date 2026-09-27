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

    /** 起始码后的 NAL 载荷下标：3 字节码返回 3、4 字节码返回 4，识别失败返回 -1 */
    int payloadIndex(const std::vector<uint8_t>& nalUnit);

    /** 把带 3/4 字节起始码的 NAL 规范化为 4 字节起始码 + 去尾部 0x00，追加到 out */
    void appendNormalizedNal(std::vector<uint8_t>& out, const std::vector<uint8_t>& nalUnit);

    std::pair<const void*, size_t> parseH265CSD0(std::vector<uint8_t> *bitstream_buffer);

    struct h265nal::H265BitstreamParserState *bitstream_parser_state;

    std::unique_ptr<h265nal::H265BitstreamParser::BitstreamState> bitstream;

    std::vector<uint8_t> *bitstream_buffer;
private:

    // parseH265CSD0 的结果缓冲区：作为成员保证返回的指针在对象生命周期内有效
    //（原实现返回局部 vector 的 data()，函数返回后即悬垂）
    std::vector<uint8_t> m_csd0;

    bool isGetVsp = false;
    bool isGetSps = false;
    bool isGetPsp = false;



};


#endif //MYYFFMPEG_HEVCNALPARSE_H
