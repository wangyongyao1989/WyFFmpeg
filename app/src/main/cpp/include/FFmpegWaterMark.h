//
// Created by MMM on 2024/4/15.
//

#ifndef MYYFFMPEG_FFMPEGWATERMARK_H
#define MYYFFMPEG_FFMPEGWATERMARK_H

#include "ffmpegpch.h"
#include "string"
#include <thread>


#define SrcWidth    1920
#define SrcHeight    1080
#define DstWidth    640
#define DstHeight    480

using namespace std;

class FFmpegWaterMark {
private:
    int video_index = -1;
    AVFormatContext *context[2];
    AVFormatContext *outputContext;
    int64_t lastPts = 0;
    int64_t lastDts = 0;
    int64_t lastFrameRealtime = 0;

    int64_t firstPts = AV_NOPTS_VALUE;
    int64_t startTime = 0;

    AVCodecContext *outPutEncContext = nullptr;
    AVCodecContext *decoderContext[2];


    struct SwsContext *pSwsContext;
    AVFrame *pSrcFrame[2];
    AVFrame *inputFrame[2];

    AVFilterInOut *inputs = avfilter_inout_alloc();
    AVFilterInOut *outputs = avfilter_inout_alloc();
    AVFilterGraph *filter_graph = nullptr;

    AVFilterContext *inputFilterContext[2];
    AVFilterContext *outputFilterContext = nullptr;
    const char *filter_descr = "overlay=100:100";

    std::thread *decodeTask = nullptr;
//    std::thread *m_Thread = nullptr;
private:
    int InitOutputFilter(AVFilterInOut *output, const char *filterName);

    void FreeInout();

    int InitInputFilter(AVFilterInOut *input, const char *filterName, int inputIndex);

    bool DecodeVideo(AVPacket *packet, AVFrame *frame, int inputIndex);

    int InitDecodeCodec(AVCodecID codecId, int inputIndex);

    int InitEncoderCodec(int iWidth, int iHeight, int inputIndex);

    void CloseOutput();

    void CloseInput(int inputIndex);

    int OpenOutput(char *fileName, int inputIndex);

    shared_ptr<AVPacket> ReadPacketFromSource(int inputIndex);

    int OpenInput(char *fileName, int inputIndex);

    void Init();


    void ErrorFree();
    void StartDecodingThread();
    static void decodePacket(FFmpegWaterMark *ff);


public:
    int mp4WaterMark(const char *inputUrl, const char *pngUrl, const char *outputUrl);


    ~FFmpegWaterMark();

};


#endif //MYYFFMPEG_FFMPEGWATERMARK_H
