//
// Created by MMM on 2024/4/17.
//

#ifndef MYYFFMPEG_FFMPEGWATERMARKTEST_H
#define MYYFFMPEG_FFMPEGWATERMARKTEST_H

#include "ffmpegpch.h"
#include "string"

static AVFormatContext *fmt_ctx = NULL;
static AVCodecContext *dec_ctx = NULL;
static int64_t last_pts = AV_NOPTS_VALUE;
static int video_stream_index = -1;

static AVFormatContext *pFormatCtx;
static AVCodecContext *pCodecCtx;

class FFmpegWaterMarkTest {

private:

    AVFilterContext *buffersink_ctx = NULL;
    AVFilterContext *buffersrc_ctx = NULL;
    AVFilterGraph *filter_graph = NULL;

    int video_index = -1;

    const char *filter_descr = "movie=logo.jpg[wm];[in][wm]overlay=5:5[out]";

public:
    int mp4WaterMark(const char *inputUrl, const char *pngUrl, const char *outputUrl);

    int open_input_file(const char *filename);
    int init_filters(const char *filters_descr);
    int init_filters_1(const char *filters_descr);

    int mp4WaterMark_leixiaohua(const char *inputUrl, const char *pngUrl, const char *outputUrl);
    int open_input_file_leixiaohua(const char *filename);
    int init_filters_leixiaohua(const char *filters_descr);

};


#endif //MYYFFMPEG_FFMPEGWATERMARKTEST_H
