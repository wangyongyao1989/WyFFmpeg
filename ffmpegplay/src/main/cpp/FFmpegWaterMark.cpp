//
// Created by MMM on 2024/4/15.
//

#include "include/FFmpegWaterMark.h"

int FFmpegWaterMark::mp4WaterMark(const char *inputUrl, const char *pngUrl, const char *outputUrl) {
    string fileInput[2];
//    fileInput[0] = "c:\\test\\test.mp4";
//    fileInput[1] = "c:\\test\\logo.png";
//    string fileOutput = "c:\\test\\test_watermark.mp4";
    fileInput[0] = inputUrl;
    fileInput[1] = pngUrl;
    string fileOutput = outputUrl;
    Init();
    for (int i = 0; i < 2; i++) {
        if (OpenInput((char *) fileInput[i].c_str(), i) < 0) {
            LOGD("Open file Input failed!");
            this_thread::sleep_for(chrono::seconds(10));
            return (0);
        }
    }
    LOGD("InitDecodeCodec start========");


    for (int i = 0; i < 2; i++) {
        int ret = -1;
        if (i == 0) {
            video_index = av_find_best_stream(context[i], AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);
            AVStream *st = context[i]->streams[video_index];
            AVCodecID id = st->codecpar->codec_id;
            ret = InitDecodeCodec(id, i);
        } else {
             ret = InitDecodeCodec(context[i]->streams[0]->codec->codec_id, i);
        }

        if (ret < 0) {
            LOGD("InitDecodeCodec failed!");
            this_thread::sleep_for(chrono::seconds(10));
            return (0);
        }
    }
    LOGD("InitDecodeCodec end========");


    int ret = InitEncoderCodec(decoderContext[0]->width, decoderContext[0]->height, 0);
    if (ret < 0) {
        LOGD("open eccoder failed ret is %s", ret);
        LOGD("InitEncoderCodec failed!");
        this_thread::sleep_for(chrono::seconds(10));
        return (0);
    }


    filter_graph = avfilter_graph_alloc();
    if (!filter_graph) {
        LOGD("graph alloc failed");
        ErrorFree();
//        goto End;
    }

    int parse2 = avfilter_graph_parse2(filter_graph, filter_descr, &inputs, &outputs);
    if (parse2 < 0) {
        LOGD("avfilter_graph_parse2 error %d", ret);
        ErrorFree();
    }
    InitInputFilter(inputs, "MainFrame", 0);
    InitInputFilter(inputs->next, "OverlayFrame", 1);
    InitOutputFilter(outputs,
                     "output");
    FreeInout();
    LOGD("InitOutputFilter end");
    ret = avfilter_graph_parse_ptr(filter_graph, filter_descr,
                                   &inputs, &outputs, NULL);
    if (ret < 0) {
        LOGD("avfilter_graph_parse_ptr error %d", ret);
        ErrorFree();
    }

    //????为什么会连接失败？？？？？？？？
    ret = avfilter_graph_config(filter_graph, NULL);
    if (ret < 0) {
        LOGD("avfilter_graph_config error %d", ret);
        ErrorFree();
//        goto End;
    }
    if (OpenOutput((char *) fileOutput.c_str(), 0) < 0) {
        LOGD("Open file Output failed!");
        this_thread::sleep_for(chrono::seconds(10));
        return (0);
    }


    pSrcFrame[0] = av_frame_alloc();
    pSrcFrame[1] = av_frame_alloc();
    inputFrame[0] = av_frame_alloc();
    inputFrame[1] = av_frame_alloc();
    auto filterFrame = av_frame_alloc();
    int got_output = 0;
    int64_t timeRecord = 0;
    int64_t firstPacketTime = 0;
    int64_t outLastTime = av_gettime();
    int64_t inLastTime = av_gettime();
    int64_t videoCount = 0;

    StartDecodingThread();
//    decodeTask.swap(thread([&] {
//        bool ret = true;
//        while (ret) {
//            auto packet = ReadPacketFromSource(1);
//            ret = DecodeVideo(packet.get(), pSrcFrame[1], 1);
//            if (ret)
//                break;
//        }
//    }));
//    decodeTask.join();

    while (true) {
        outLastTime = av_gettime();
        auto packet = ReadPacketFromSource(0);
        if (packet) {
            if (DecodeVideo(packet.get(), pSrcFrame[0], 0)) {
                av_frame_ref(inputFrame[0], pSrcFrame[0]);
                if (av_buffersrc_add_frame_flags(inputFilterContext[0], inputFrame[0],
                                                 AV_BUFFERSRC_FLAG_PUSH) >= 0) {
                    pSrcFrame[1]->pts = pSrcFrame[0]->pts;
                    av_frame_ref(inputFrame[1], pSrcFrame[1]);
                    if (av_buffersrc_add_frame_flags(inputFilterContext[1], pSrcFrame[1],
                                                     AV_BUFFERSRC_FLAG_PUSH) >= 0) {
                        ret = av_buffersink_get_frame_flags(outputFilterContext, filterFrame,
                                                            AV_BUFFERSINK_FLAG_NO_REQUEST);

                        if (ret >= 0) {
                            std::shared_ptr<AVPacket> pTmpPkt(
                                    static_cast<AVPacket *>(av_malloc(sizeof(AVPacket))),
                                    [&](AVPacket *p) {
                                        av_packet_free(&p);
                                        av_freep(&p);
                                    });
                            av_init_packet(pTmpPkt.get());
                            pTmpPkt->data = NULL;
                            pTmpPkt->size = 0;
                            ret = avcodec_encode_video2(outPutEncContext, pTmpPkt.get(),
                                                        filterFrame, &got_output);
                            if (ret >= 0 && got_output) {
                                int ret = av_write_frame(outputContext, pTmpPkt.get());
                            }
                            this_thread::sleep_for(chrono::milliseconds(10));
                        }
                        av_frame_unref(filterFrame);
                    }
                }
            }
        } else break;
    }
//    End:
//    CloseInput(0);
//    CloseInput(1);
//    CloseOutput();
//    LOGD("add watermark end!");
//    this_thread::sleep_for(chrono::hours(10));
//    return (0);
    return 0;
}

void FFmpegWaterMark::StartDecodingThread() {
    decodeTask = new thread(decodePacket, this);
    new thread(decodePacket, this);
}

void FFmpegWaterMark::decodePacket(FFmpegWaterMark *ff) {
    bool ret = true;
    while (ret) {
        auto packet = ff->ReadPacketFromSource(1);
        ret = ff->DecodeVideo(packet.get(), ff->pSrcFrame[1], 1);
        if (ret)
            break;
    }
}


int interrupt_cb(void *ctx) {
    return (0);
}


void FFmpegWaterMark::Init() {
    av_register_all();
    avfilter_register_all();
    avformat_network_init();
//    avdevice_register_all();
    avcodec_register_all();
    av_log_set_level(AV_LOG_ERROR);
}


int FFmpegWaterMark::OpenInput(char *fileName, int inputIndex) {
    context[inputIndex] = avformat_alloc_context();
    context[inputIndex]->interrupt_callback.callback = interrupt_cb;
    AVDictionary *format_opts = nullptr;

    int ret = avformat_open_input(&context[inputIndex], fileName, nullptr, &format_opts);
    if (ret < 0) {
        return (ret);
    }
    ret = avformat_find_stream_info(context[inputIndex], nullptr);
    av_dump_format(context[inputIndex], 0, fileName, 0);
    if (ret >= 0) {
        LOGD("open %s input stream successfully", fileName);
    }
    return (ret);
}


shared_ptr<AVPacket> FFmpegWaterMark::ReadPacketFromSource(int inputIndex) {
    std::shared_ptr<AVPacket> packet(static_cast<AVPacket *>(av_malloc(sizeof(AVPacket))),
                                     [&](AVPacket *p) {
                                         av_packet_free(&p);
                                         av_freep(&p);
                                     });
    av_init_packet(packet.get());
    int ret = av_read_frame(context[inputIndex], packet.get());
    if (ret >= 0) {
        return (packet);
    } else {
        return (nullptr);
    }
}


int FFmpegWaterMark::OpenOutput(char *fileName, int inputIndex) {
    int ret = 0;
    ret = avformat_alloc_output_context2(&outputContext, nullptr, "mpegts", fileName);
    if (ret < 0) {
        goto Error;
    }
    ret = avio_open2(&outputContext->pb, fileName, AVIO_FLAG_READ_WRITE, nullptr, nullptr);
    if (ret < 0) {
        goto Error;
    }

    for (int i = 0; i < context[inputIndex]->nb_streams; i++) {
        AVStream *stream = avformat_new_stream(outputContext, outPutEncContext->codec);
        stream->codec = outPutEncContext;
        if (ret < 0) {
            goto Error;
        }
    }
    av_dump_format(outputContext, 0, fileName, 1);
    ret = avformat_write_header(outputContext, nullptr);
    if (ret < 0) {
        goto Error;
    }
    if (ret >= 0)
        LOGD("open output stream successfully");
    return (ret);
    Error:
    if (outputContext) {
        avformat_close_input(&outputContext);
    }
    return (ret);
}


void FFmpegWaterMark::CloseInput(int inputIndex) {
    if (context != nullptr) {
        avformat_close_input(&context[inputIndex]);
    }
}


void FFmpegWaterMark::CloseOutput() {
    if (outputContext != nullptr) {
        for (int i = 0; i < outputContext->nb_streams; i++) {
            AVCodecContext *codecContext = outputContext->streams[i]->codec;
            avcodec_close(codecContext);
        }
        avformat_close_input(&outputContext);
    }
}


int FFmpegWaterMark::InitEncoderCodec(int iWidth, int iHeight, int inputIndex) {
    LOGD("InitEncoderCodec==============iWidth:%d===iHeight:%d", iWidth, iHeight);
    AVCodec *pH264Codec = avcodec_find_encoder(context[0]->streams[video_index]->codecpar->codec_id);
    LOGD("InitEncoderCodec==============AV_CODEC_ID_H264:%d", AV_CODEC_ID_H264);
//    AVCodec *pH264Codec = avcodec_find_encoder(videoCodecId);

    if (NULL == pH264Codec) {
        LOGD("avcodec_find_encoder failed");
        return (-1);
    }
    outPutEncContext = avcodec_alloc_context3(pH264Codec);
    outPutEncContext = context[0]->streams[0]->codec;
    LOGD("InitEncoderCodec:%p", outPutEncContext);
    outPutEncContext->gop_size = 30;
    outPutEncContext->has_b_frames = 0;
    outPutEncContext->max_b_frames = 0;
    outPutEncContext->codec_id = pH264Codec->id;
    outPutEncContext->time_base.num = context[inputIndex]->streams[0]->codec->time_base.num;
    outPutEncContext->time_base.den = context[inputIndex]->streams[0]->codec->time_base.den;
    LOGD("InitEncoderCodec:%p", outPutEncContext);
    outPutEncContext->pix_fmt = AV_PIX_FMT_YUV420P;
//    outPutEncContext->pix_fmt = *pH264Codec->pix_fmts;
//    LOGD("InitEncoderCodec pix_fmt:%p", outPutEncContext->pix_fmt);
    outPutEncContext->width = iWidth;
    outPutEncContext->height = iHeight;

    outPutEncContext->me_subpel_quality = 0;
    outPutEncContext->refs = 1;
    outPutEncContext->scenechange_threshold = 0;
    outPutEncContext->trellis = 0;

    AVDictionary *options = nullptr;
    outPutEncContext->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
    int ret = avcodec_open2(outPutEncContext, pH264Codec, &options);
    if (ret < 0) {
        LOGD("open codec failed");
        return (ret);
    }
    return (1);
}


int FFmpegWaterMark::InitDecodeCodec(AVCodecID codecId, int inputIndex) {
    auto codec = avcodec_find_decoder(codecId);
    if (!codec) {
        return (-1);
    }
    decoderContext[inputIndex] = context[inputIndex]->streams[0]->codec;
    if (!decoderContext) {
        LOGD("Could not allocate video codec context\n");
        return (-1);
    }

    if (codec->capabilities & AV_CODEC_CAP_TRUNCATED)
        decoderContext[inputIndex]->flags |= AV_CODEC_FLAG_TRUNCATED;
    int ret = avcodec_open2(decoderContext[inputIndex], codec, NULL);
    return (ret);
}


bool FFmpegWaterMark::DecodeVideo(AVPacket *packet, AVFrame *frame, int inputIndex) {
    int gotFrame = 0;
    auto hr = avcodec_decode_video2(decoderContext[inputIndex], frame, &gotFrame, packet);
    if (hr >= 0 && gotFrame != 0) {
        frame->pts = packet->pts;
        return (true);
    }
    return (false);
}


int FFmpegWaterMark::InitInputFilter(AVFilterInOut *input, const char *filterName, int inputIndex) {
    char args[512];
    memset(args, 0, sizeof(args));
    AVFilterContext *padFilterContext = input->filter_ctx;

    auto filter = avfilter_get_by_name("buffer");
    auto codecContext = context[inputIndex]->streams[0]->codec;
    LOGD("video_size=%dx%d:pix_fmt=%d:time_base=%d/%d:pixel_aspect=%d/%d",
         codecContext->width, codecContext->height, codecContext->pix_fmt,
         codecContext->time_base.num,
         codecContext->time_base.den / codecContext->ticks_per_frame,
         codecContext->sample_aspect_ratio.num, codecContext->sample_aspect_ratio.den);

    int ret = avfilter_graph_create_filter(&inputFilterContext[inputIndex], filter, filterName,
                                           args,
                                           NULL, filter_graph);
    if (ret < 0)
        return (ret);
    ret = avfilter_link(inputFilterContext[inputIndex], 0, padFilterContext, input->pad_idx);
    return (ret);
}


int FFmpegWaterMark::InitOutputFilter(AVFilterInOut *output, const char *filterName) {
    AVFilterContext *padFilterContext = output->filter_ctx;
    auto filter = avfilter_get_by_name("buffersink");
    int ret = avfilter_graph_create_filter(&outputFilterContext, filter, filterName, NULL,
                                           NULL, filter_graph);
    if (ret < 0)
        return (ret);
    ret = avfilter_link(padFilterContext, output->pad_idx, outputFilterContext, 0);

    return (ret);
}


void FFmpegWaterMark::FreeInout() {
    avfilter_inout_free(&inputs->next);
    avfilter_inout_free(&inputs);
    avfilter_inout_free(&outputs);
}

FFmpegWaterMark::~FFmpegWaterMark() {
    CloseInput(0);
    CloseInput(1);
    CloseOutput();
    if (decodeTask) {
        decodeTask->join();
        delete decodeTask;
        decodeTask = nullptr;
    }
    LOGD("add watermark end!");
    this_thread::sleep_for(chrono::hours(10));
}

void FFmpegWaterMark::ErrorFree() {
    CloseInput(0);
    CloseInput(1);
    CloseOutput();
    LOGD("ErrorFree add watermark end!");
    this_thread::sleep_for(chrono::hours(10));
    if (decodeTask) {
        decodeTask->join();
        delete decodeTask;
        decodeTask = nullptr;
    }
}
