//
// Created by MMM on 2024/4/17.
//

#include "include/FFmpegWaterMarkTest.h"

int
FFmpegWaterMarkTest::mp4WaterMark(const char *inputUrl, const char *pngUrl, const char *outputUrl) {
    int ret = -1;
    if ((ret = open_input_file(inputUrl) < 0)) {
        return -1;
    }
    LOGD("Open file Input end!");


    ///2.初始化滤镜
    if ((ret = init_filters(filter_descr)) < 0) {
        return -1;
    }
    LOGD("init_filters end!");


    AVPacket *pkt = av_packet_alloc();
    //av_init_packet(pkt);

    AVFrame *frame = av_frame_alloc();
    AVFrame *filt_frame = av_frame_alloc();

    while (av_read_frame(fmt_ctx, pkt) >= 0) {
        if (pkt->stream_index == video_index) {
            int ret = avcodec_send_packet(dec_ctx, pkt);
            if (ret >= 0) {
                ret = avcodec_receive_frame(dec_ctx, frame);
                if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
                    continue;
                } else if (ret < 0) {
                    continue;
                }

                frame->pts = frame->best_effort_timestamp;

                /* push the decoded frame into the filtergraph */
                if (av_buffersrc_add_frame_flags(buffersrc_ctx, frame, AV_BUFFERSRC_FLAG_KEEP_REF) <
                    0) {
                    av_log(NULL, AV_LOG_ERROR, "Error while feeding the filtergraph\n");
                    break;
                }

                /* pull filtered frames from the filtergraph */
                while (1) {
                    ret = av_buffersink_get_frame(buffersink_ctx, filt_frame);
                    if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
                        break;
                    if (ret < 0)
                        break;

                    switch (dec_ctx->pix_fmt) {
                        case AV_PIX_FMT_YUV420P: {

                            int size = av_image_get_buffer_size(AV_PIX_FMT_YUV420P,
                                                                filt_frame->width,
                                                                filt_frame->height, 1);

                            char fileName[20] = {0};
                            sprintf(fileName, "img2/%d.yuv", dec_ctx->frame_number);

                            FILE *fp;
                            fp = fopen(fileName, "wb");

                            for (int i = 0; i < filt_frame->height; i++) {
                                fwrite(filt_frame->data[0] + filt_frame->linesize[0] * i, 1,
                                       filt_frame->width, fp);
                            }
                            for (int i = 0; i < filt_frame->height / 2; i++) {
                                fwrite(filt_frame->data[1] + filt_frame->linesize[1] * i, 1,
                                       filt_frame->width / 2, fp);
                            }
                            for (int i = 0; i < filt_frame->height / 2; i++) {
                                fwrite(filt_frame->data[2] + filt_frame->linesize[2] * i, 1,
                                       filt_frame->width / 2, fp);
                            }

                            fclose(fp);
                        }
                            break;
                        default:
                            return -1;
                    }

                    av_frame_unref(filt_frame);

                }
                av_frame_unref(frame);
            }
        }
    }

    avfilter_graph_free(&filter_graph);
    avcodec_close(dec_ctx);
    avcodec_free_context(&dec_ctx);
    avformat_close_input(&fmt_ctx);
    av_frame_free(&frame);
    av_frame_free(&filt_frame);
    av_packet_free(&pkt);

    return 0;
}

int FFmpegWaterMarkTest::open_input_file(const char *filename) {
    //av_register_all();
    avformat_network_init();

    ///打开输入的流
    int ret = avformat_open_input(&fmt_ctx, filename, NULL, NULL);
    if (ret != 0) {
        LOGD("Couldn't open input stream.\n");
        return -1;
    }

    //查找流信息
    if (avformat_find_stream_info(fmt_ctx, NULL) < 0) {
        LOGD("Couldn't find stream information.\n");
        return -1;
    }

    //找到视频流索引
    video_index = av_find_best_stream(fmt_ctx, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);

    AVStream *st = fmt_ctx->streams[video_index];

    AVCodec *codec = nullptr;

    //找到解码器
    codec = avcodec_find_decoder(st->codecpar->codec_id);
    if (!codec) {
        LOGD("Codec not found codec");
        return -1;
    }

    //申请AVCodecContext
    dec_ctx = avcodec_alloc_context3(codec);
    if (!dec_ctx) {
        return -1;
    }

    avcodec_parameters_to_context(dec_ctx, fmt_ctx->streams[video_index]->codecpar);

    //打开解码器
    if ((ret = avcodec_open2(dec_ctx, codec, NULL) < 0)) {
        return -1;
    }

    return 0;
}

int FFmpegWaterMarkTest::init_filters(const char *filters_descr) {
    char args[512];
    int ret = 0;
    const AVFilter *buffersrc = avfilter_get_by_name("buffer");
    const AVFilter *buffersink = avfilter_get_by_name("buffersink");
    AVFilterInOut *outputs = avfilter_inout_alloc();
    AVFilterInOut *inputs = avfilter_inout_alloc();
    AVRational time_base = fmt_ctx->streams[video_index]->time_base;
    enum AVPixelFormat pix_fmts[] = {AV_PIX_FMT_YUV420P, AV_PIX_FMT_NONE};

    filter_graph = avfilter_graph_alloc();
    if (!outputs || !inputs || !filter_graph) {
        ret = AVERROR(ENOMEM);
        return ret;
    }

    /* buffer video source: the decoded frames from the decoder will be inserted here. */
    LOGD(
            "video_size=%dx%d:pix_fmt=%d:time_base=%d/%d:pixel_aspect=%d/%d",
            dec_ctx->width, dec_ctx->height, dec_ctx->pix_fmt,
            time_base.num, time_base.den,
            dec_ctx->sample_aspect_ratio.num, dec_ctx->sample_aspect_ratio.den);

    ret = avfilter_graph_create_filter(&buffersrc_ctx, buffersrc, "in",
                                       args, NULL, filter_graph);
    if (ret < 0) {
        LOGD("Cannot create buffer source  ret:%d",ret);
        return ret;
    }

    /* buffer video sink: to terminate the filter chain. */
    ret = avfilter_graph_create_filter(&buffersink_ctx, buffersink, "out",
                                       NULL, NULL, filter_graph);
    if (ret < 0) {
        LOGD("Cannot create buffer sink  ret:%d",ret);
        return ret;
    }

    ret = av_opt_set_int_list(buffersink_ctx, "pix_fmts", pix_fmts,
                              AV_PIX_FMT_NONE, AV_OPT_SEARCH_CHILDREN);
    if (ret < 0) {
        LOGD("Cannot set output pixel format  ret:%d",ret);
        return ret;
    }

    outputs->name = av_strdup("in");
    outputs->filter_ctx = buffersrc_ctx;
    outputs->pad_idx = 0;
    outputs->next = NULL;

    inputs->name = av_strdup("out");
    inputs->filter_ctx = buffersink_ctx;
    inputs->pad_idx = 0;
    inputs->next = NULL;

    if ((ret = avfilter_graph_parse_ptr(filter_graph, filters_descr,
                                        &inputs, &outputs, NULL)) < 0) {
        LOGD("avfilter_graph_parse_ptr fail  ret:%d",AVERROR_FILTER_NOT_FOUND);
        return ret;
    }

    if ((ret = avfilter_graph_config(filter_graph, NULL)) < 0) {
        LOGD("avfilter_graph_config fail ret:%d",ret);
        return ret;
    }

    avfilter_inout_free(&inputs);
    avfilter_inout_free(&outputs);

    return ret;
}
