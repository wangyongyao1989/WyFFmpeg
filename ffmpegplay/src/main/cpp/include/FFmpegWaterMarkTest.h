//
// Created by MMM on 2024/4/17.
//

#ifndef MYYFFMPEG_FFMPEGWATERMARKTEST_H
#define MYYFFMPEG_FFMPEGWATERMARKTEST_H

#include "ffmpegpch.h"
#include "string"

/**
 * 给 MP4 视频叠加图片水印：解码 -> overlay 滤镜 -> mpeg4 编码 -> mp4 混流（音频流 copy）。
 * 注：内置 FFmpeg 未编译 libx264，故使用内置 mpeg4 编码器。
 */
class FFmpegWaterMarkTest {

private:
    AVFormatContext *fmt_ctx = nullptr;      ///< 输入
    AVCodecContext *dec_ctx = nullptr;
    int video_index = -1;

    AVFilterGraph *filter_graph = nullptr;
    AVFilterContext *buffersrc_ctx = nullptr;
    AVFilterContext *buffersink_ctx = nullptr;

    AVFormatContext *ofmt_ctx = nullptr;     ///< 输出
    AVCodecContext *enc_ctx = nullptr;
    int out_video_index = -1;

public:
    int mp4WaterMark(const char *inputUrl, const char *pngUrl, const char *outputUrl);

private:
    int open_input_file(const char *filename);

    int init_filters(const char *pngUrl);

    int open_output_file(const char *outputUrl);

    int encode_video_frame(AVFrame *frame);

    int flush_encoder();

    void release_all();
};


#endif //MYYFFMPEG_FFMPEGWATERMARKTEST_H
