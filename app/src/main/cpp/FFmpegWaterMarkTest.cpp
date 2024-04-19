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
                    LOGE("Error while feeding the filtergraph\n");
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
    int ret;
    AVFilterContext *buffersrc_ctx;
    AVFilterContext *buffersink_ctx;
    const AVFilter *buffersrc = avfilter_get_by_name("buffer");
    const AVFilter *buffersink = avfilter_get_by_name("buffersink");
    AVFilterInOut *outputs = avfilter_inout_alloc();
    AVFilterInOut *inputs = avfilter_inout_alloc();
    enum AVPixelFormat pix_fmts[] = {AV_PIX_FMT_YUV420P, AV_PIX_FMT_NONE};

    AVFilterGraph *filter_graph = avfilter_graph_alloc();
    if (!outputs || !inputs || !filter_graph) {
        ret = AVERROR(ENOMEM);
        goto end;
    }
    /* buffer video source: the decoded frames from the decoder will be inserted here. */
    LOGI("video_size=%dx%d:pix_fmt=%d:time_base=%d/%d:pixel_aspect=%d/%d",
         dec_ctx->width, dec_ctx->height, dec_ctx->pix_fmt,
         dec_ctx->time_base.num, dec_ctx->time_base.den,
         dec_ctx->sample_aspect_ratio.num, dec_ctx->sample_aspect_ratio.den);
    ret = avfilter_graph_create_filter(&buffersrc_ctx, buffersrc, "in",
                                       args, NULL, filter_graph);
    if (ret < 0) {
        LOGE("Cannot create buffer source ret:%d",ret);
        goto end;
    }
    /* buffer video sink: to terminate the filter chain. */
    ret = avfilter_graph_create_filter(&buffersink_ctx, buffersink, "out",
                                       NULL, NULL, filter_graph);
    if (ret < 0) {
        LOGE( "Cannot create buffer sink\n");
        goto end;
    }
    ret = av_opt_set_int_list(buffersink_ctx, "pix_fmts", pix_fmts,
                              AV_PIX_FMT_NONE, AV_OPT_SEARCH_CHILDREN);
    if (ret < 0) {
        LOGE("Cannot set output pixel format\n");
        goto end;
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
                                        &inputs, &outputs, NULL)) < 0)
        goto end;
    if ((ret = avfilter_graph_config(filter_graph, NULL)) < 0)
        goto end;

//    *graph = filter_graph;
//    *src   = buffersrc_ctx;
//    *sink  = buffersink_ctx;
    end:
    avfilter_inout_free(&inputs);
    avfilter_inout_free(&outputs);
    return ret;
}

int FFmpegWaterMarkTest::open_input_file_leixiaohua(const char *filename) {
    int ret;
    AVCodec *dec;

    if ((ret = avformat_open_input(&pFormatCtx, filename, NULL, NULL)) < 0) {
        LOGD("Cannot open input file\n");
        return ret;
    }

    if ((ret = avformat_find_stream_info(pFormatCtx, NULL)) < 0) {
        LOGD("Cannot find stream information\n");
        return ret;
    }

    /* select the video stream */
    ret = av_find_best_stream(pFormatCtx, AVMEDIA_TYPE_VIDEO, -1, -1, &dec, 0);
    if (ret < 0) {
        LOGD("Cannot find a video stream in the input file\n");
        return ret;
    }
    video_stream_index = ret;
    pCodecCtx = pFormatCtx->streams[video_stream_index]->codec;

    /* init the video decoder */
    if ((ret = avcodec_open2(pCodecCtx, dec, NULL)) < 0) {
        LOGD("Cannot open video decoder\n");
        return ret;
    }

    return 0;
}

int FFmpegWaterMarkTest::init_filters_leixiaohua(const char *filters_descr) {
    char args[512];
    int ret;
    AVFilterContext *buffersrc_ctx;
    AVFilterContext *buffersink_ctx;
    AVFilter *buffersrc = const_cast<AVFilter *>(avfilter_get_by_name("buffer"));
    AVFilter *buffersink = const_cast<AVFilter *>(avfilter_get_by_name("buffersink"));
    AVFilterInOut *outputs = avfilter_inout_alloc();
    AVFilterInOut *inputs = avfilter_inout_alloc();
    enum AVPixelFormat pix_fmts[] = {AV_PIX_FMT_YUV420P, AV_PIX_FMT_NONE};
    AVBufferSinkParams *buffersink_params;

    filter_graph = avfilter_graph_alloc();

    /* buffer video source: the decoded frames from the decoder will be inserted here. */
    LOGD(
            "video_size=%dx%d:pix_fmt=%d:time_base=%d/%d:pixel_aspect=%d/%d",
            pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt,
            pCodecCtx->time_base.num, pCodecCtx->time_base.den,
            pCodecCtx->sample_aspect_ratio.num, pCodecCtx->sample_aspect_ratio.den);

    ret = avfilter_graph_create_filter(&buffersrc_ctx, buffersrc, "in",
                                       args, NULL, filter_graph);
    if (ret < 0) {
        LOGD("Cannot create buffer source\n");
        return ret;
    }

    /* buffer video sink: to terminate the filter chain. */
    buffersink_params = av_buffersink_params_alloc();
    buffersink_params->pixel_fmts = pix_fmts;
    ret = avfilter_graph_create_filter(&buffersink_ctx, buffersink, "out",
                                       NULL, buffersink_params, filter_graph);
    av_free(buffersink_params);
    if (ret < 0) {
        LOGD("Cannot create buffer sink\n");
        return ret;
    }
    ret = av_opt_set_int_list(buffersink_ctx, "pix_fmts", pix_fmts,
                              AV_PIX_FMT_NONE, AV_OPT_SEARCH_CHILDREN);
    if (ret < 0) {
        LOGE("Cannot av_opt_set_int_list\n");
        return ret;
    }

    /* Endpoints for the filter graph. */
    outputs->name = av_strdup("in");
    outputs->filter_ctx = buffersrc_ctx;
    outputs->pad_idx = 0;
    outputs->next = NULL;

    inputs->name = av_strdup("out");
    inputs->filter_ctx = buffersink_ctx;
    inputs->pad_idx = 0;
    inputs->next = NULL;

    if ((ret = avfilter_graph_parse_ptr(filter_graph, filters_descr,
                                        &inputs, &outputs, NULL)) < 0)
        return ret;

    if ((ret = avfilter_graph_config(filter_graph, NULL)) < 0)
        return ret;
    return 0;
}

int init_filters_1(const char *filters_descr, AVRational time_base, AVCodecContext *codecCtx,
                 AVFilterGraph **graph, AVFilterContext **src, AVFilterContext **sink) {
    char args[512];
    int ret;
    AVFilterContext *buffersrc_ctx;
    AVFilterContext *buffersink_ctx;
    const AVFilter *buffersrc = avfilter_get_by_name("buffer");
    const AVFilter *buffersink = avfilter_get_by_name("buffersink");
    AVFilterInOut *outputs = avfilter_inout_alloc();
    AVFilterInOut *inputs = avfilter_inout_alloc();
    enum AVPixelFormat pix_fmts[] = {AV_PIX_FMT_YUV420P, AV_PIX_FMT_NONE};

    AVFilterGraph *filter_graph = avfilter_graph_alloc();
    if (!outputs || !inputs || !filter_graph) {
        ret = AVERROR(ENOMEM);
        goto end;
    }
    /* buffer video source: the decoded frames from the decoder will be inserted here. */

    LOGI("video_size=%dx%d:pix_fmt=%d:time_base=%d/%d:pixel_aspect=%d/%d",
         codecCtx->width, codecCtx->height, codecCtx->pix_fmt,
         time_base.num, time_base.den,
         codecCtx->sample_aspect_ratio.num, codecCtx->sample_aspect_ratio.den);
    ret = avfilter_graph_create_filter(&buffersrc_ctx, buffersrc, "in",
                                       args, NULL, filter_graph);
    if (ret < 0) {
        LOGE("Cannot create buffer source ret:%d",ret);
        goto end;
    }
    /* buffer video sink: to terminate the filter chain. */
    ret = avfilter_graph_create_filter(&buffersink_ctx, buffersink, "out",
                                       NULL, NULL, filter_graph);
    if (ret < 0) {
        LOGE("Cannot create buffer sink\n");
        goto end;
    }
    ret = av_opt_set_int_list(buffersink_ctx, "pix_fmts", pix_fmts,
                              AV_PIX_FMT_NONE, AV_OPT_SEARCH_CHILDREN);
    if (ret < 0) {
        LOGE("Cannot set output pixel format\n");
        goto end;
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
                                        &inputs, &outputs, NULL)) < 0)
        goto end;
    if ((ret = avfilter_graph_config(filter_graph, NULL)) < 0)
        goto end;

    *graph = filter_graph;
    *src   = buffersrc_ctx;
    *sink  = buffersink_ctx;
    end:
    avfilter_inout_free(&inputs);
    avfilter_inout_free(&outputs);
    return ret;
}

/**
 * 雷霄骅github
 * https://github.com/leixiaohua1020/simplest_ffmpeg_video_filter/blob/master/simplest_ffmpeg_video_filter/simplest_ffmpeg_video_filter.cpp
 * @param inputUrl
 * @param pngUrl
 * @param outputUrl
 * @return
 */
int FFmpegWaterMarkTest::mp4WaterMark_leixiaohua(const char *inputUrl, const char *pngUrl,
                                                 const char *outputUrl) {

    int ret;
    AVPacket packet;
    AVFrame *pFrame;
    AVFrame *pFrame_out;

    int got_frame;

    av_register_all();
    avfilter_register_all();

    if ((ret = open_input_file_leixiaohua(inputUrl)) < 0)
        goto end;
    if ((ret = init_filters_leixiaohua(filter_descr)) < 0)
        goto end;

#if ENABLE_YUVFILE
    FILE *fp_yuv=fopen("test.yuv","wb+");
#endif
#if ENABLE_SDL
    SDL_Surface *screen; 
    SDL_Overlay *bmp; 
    SDL_Rect rect;
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER)) {  
        printf( "Could not initialize SDL - %s\n", SDL_GetError()); 
        return -1;
    } 
    screen = SDL_SetVideoMode(pCodecCtx->width, pCodecCtx->height, 0, 0);
    if(!screen) {  
        LOGD("SDL: could not set video mode - exiting\n");  
        return -1;
    }
    bmp = SDL_CreateYUVOverlay(pCodecCtx->width, pCodecCtx->height,SDL_YV12_OVERLAY, screen); 

    SDL_WM_SetCaption("Simplest FFmpeg Video Filter",NULL);
#endif

    pFrame = av_frame_alloc();
    pFrame_out = av_frame_alloc();

    /* read all packets */
    while (1) {

        ret = av_read_frame(pFormatCtx, &packet);
        if (ret < 0)
            break;

        if (packet.stream_index == video_stream_index) {
            got_frame = 0;
            ret = avcodec_decode_video2(pCodecCtx, pFrame, &got_frame, &packet);
            if (ret < 0) {
                LOGD("Error decoding video\n");
                break;
            }

            if (got_frame) {
                pFrame->pts = av_frame_get_best_effort_timestamp(pFrame);

                /* push the decoded frame into the filtergraph */
                if (av_buffersrc_add_frame(buffersrc_ctx, pFrame) < 0) {
                    LOGD("Error while feeding the filtergraph\n");
                    break;
                }

                /* pull filtered pictures from the filtergraph */
                while (1) {

                    ret = av_buffersink_get_frame(buffersink_ctx, pFrame_out);
                    if (ret < 0)
                        break;

                    LOGD("Process 1 frame!\n");

                    if (pFrame_out->format == AV_PIX_FMT_YUV420P) {
#if ENABLE_YUVFILE
                        //Y, U, V
                        for(int i=0;i<pFrame_out->height;i++){
                            fwrite(pFrame_out->data[0]+pFrame_out->linesize[0]*i,1,pFrame_out->width,fp_yuv);
                        }
                        for(int i=0;i<pFrame_out->height/2;i++){
                            fwrite(pFrame_out->data[1]+pFrame_out->linesize[1]*i,1,pFrame_out->width/2,fp_yuv);
                        }
                        for(int i=0;i<pFrame_out->height/2;i++){
                            fwrite(pFrame_out->data[2]+pFrame_out->linesize[2]*i,1,pFrame_out->width/2,fp_yuv);
                        }
#endif

#if ENABLE_SDL
                        SDL_LockYUVOverlay(bmp);
                        int y_size=pFrame_out->width*pFrame_out->height;
                        memcpy(bmp->pixels[0],pFrame_out->data[0],y_size);   //Y
                        memcpy(bmp->pixels[2],pFrame_out->data[1],y_size/4); //U
                        memcpy(bmp->pixels[1],pFrame_out->data[2],y_size/4); //V 
                        bmp->pitches[0]=pFrame_out->linesize[0];
                        bmp->pitches[2]=pFrame_out->linesize[1];   
                        bmp->pitches[1]=pFrame_out->linesize[2];
                        SDL_UnlockYUVOverlay(bmp); 
                        rect.x = 0;    
                        rect.y = 0;    
                        rect.w = pFrame_out->width;    
                        rect.h = pFrame_out->height;    
                        SDL_DisplayYUVOverlay(bmp, &rect); 
                        //Delay 40ms
                        SDL_Delay(40);
#endif
                    }
                    av_frame_unref(pFrame_out);
                }
            }
            av_frame_unref(pFrame);
        }
        av_free_packet(&packet);
    }
#if ENABLE_YUVFILE
    fclose(fp_yuv);
#endif

    end:
    avfilter_graph_free(&filter_graph);
    if (pCodecCtx)
        avcodec_close(pCodecCtx);
    avformat_close_input(&pFormatCtx);


    if (ret < 0 && ret != AVERROR_EOF) {
        char buf[1024];
        av_strerror(ret, buf, sizeof(buf));
        LOGD("Error occurred: %s\n", buf);
        return -1;
    }

    return 0;
}
