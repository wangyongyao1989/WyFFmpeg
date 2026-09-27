//
// Created by MMM on 2024/4/17.
//

#include "include/FFmpegWaterMarkTest.h"
#include <string.h>

/// 转义滤镜串中的文件名（滤镜表达式里 \ : ' [ ] , ; 需要转义）
static std::string escape_filter_filename(const char *path) {
    std::string out;
    for (const char *p = path; *p != '\0'; ++p) {
        char c = *p;
        if (c == '\\' || c == ':' || c == '\'' || c == '[' || c == ']' || c == ',' || c == ';') {
            out.push_back('\\');
        }
        out.push_back(c);
    }
    return out;
}

int FFmpegWaterMarkTest::mp4WaterMark(const char *inputUrl, const char *pngUrl,
                                      const char *outputUrl) {
    int ret = -1;
    AVPacket *pkt = nullptr;      ///< 输入包
    AVPacket *opkt = nullptr;     ///< 编码输出包
    AVFrame *frame = nullptr;
    AVFrame *filt_frame = nullptr;

    if (inputUrl == nullptr || pngUrl == nullptr || outputUrl == nullptr) {
        LOGE("mp4WaterMark: invalid args");
        return -1;
    }

    if (open_input_file(inputUrl) < 0) {
        LOGE("mp4WaterMark: open_input_file failed");
        goto end;
    }
    LOGD("Open file Input end!");

    if (init_filters(pngUrl) < 0) {
        LOGE("mp4WaterMark: init_filters failed");
        goto end;
    }
    LOGD("init_filters end!");

    if (open_output_file(outputUrl) < 0) {
        LOGE("mp4WaterMark: open_output_file failed");
        goto end;
    }
    LOGD("open_output_file end!");

    pkt = av_packet_alloc();
    opkt = av_packet_alloc();
    frame = av_frame_alloc();
    filt_frame = av_frame_alloc();
    if (!pkt || !opkt || !frame || !filt_frame) {
        LOGE("mp4WaterMark: alloc packet/frame failed");
        goto end;
    }

    ///1.读取输入，视频走 解码->滤镜->编码->混流；音频走 stream copy
    while (av_read_frame(fmt_ctx, pkt) >= 0) {
        if (pkt->stream_index == video_index) {
            ret = avcodec_send_packet(dec_ctx, pkt);
            av_packet_unref(pkt);
            if (ret < 0 && ret != AVERROR(EAGAIN) && ret != AVERROR_EOF) {
                LOGE("avcodec_send_packet failed: %d", ret);
                goto end;
            }
            while (true) {
                ret = avcodec_receive_frame(dec_ctx, frame);
                if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
                    break;
                } else if (ret < 0) {
                    LOGE("avcodec_receive_frame failed: %d", ret);
                    goto end;
                }
                frame->pts = frame->best_effort_timestamp;

                /* push the decoded frame into the filtergraph */
                if (av_buffersrc_add_frame_flags(buffersrc_ctx, frame,
                                                 AV_BUFFERSRC_FLAG_KEEP_REF) < 0) {
                    LOGE("Error while feeding the filtergraph\n");
                    av_frame_unref(frame);
                    goto end;
                }

                /* pull filtered frames from the filtergraph */
                while (true) {
                    ret = av_buffersink_get_frame(buffersink_ctx, filt_frame);
                    if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
                        break;
                    }
                    if (ret < 0) {
                        av_frame_unref(frame);
                        goto end;
                    }
                    ret = encode_video_frame(filt_frame);
                    av_frame_unref(filt_frame);
                    if (ret < 0) {
                        av_frame_unref(frame);
                        goto end;
                    }
                }
                av_frame_unref(frame);
            }
        } else {
            ///音频等非视频流：copy 到输出（重映射 stream_index 与时间基）。
            ///输出流顺序 = [video] + 输入中按序的音频流
            AVStream *ist = fmt_ctx->streams[pkt->stream_index];
            AVStream *ost = nullptr;
            if (ist->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
                int audio_seq = 0;
                for (int i = 0; i < (int) fmt_ctx->nb_streams; ++i) {
                    if (i == pkt->stream_index) break;
                    if (i == video_index) continue;
                    if (fmt_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
                        audio_seq++;
                    }
                }
                int idx = out_video_index + 1 + audio_seq;
                if (idx >= 0 && idx < (int) ofmt_ctx->nb_streams
                    && ofmt_ctx->streams[idx]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
                    ost = ofmt_ctx->streams[idx];
                }
            }
            if (ost != nullptr) {
                pkt->stream_index = ost->index;
                av_packet_rescale_ts(pkt, ist->time_base, ost->time_base);
                if (av_write_frame(ofmt_ctx, pkt) < 0) {
                    LOGE("audio copy av_write_frame failed");
                }
            }
            av_packet_unref(pkt);
        }
    }

    ///2.冲刷解码器 -> 滤镜图 EOF -> 冲刷编码器
    avcodec_send_packet(dec_ctx, nullptr);
    while (true) {
        ret = avcodec_receive_frame(dec_ctx, frame);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
            break;
        } else if (ret < 0) {
            break;
        }
        frame->pts = frame->best_effort_timestamp;
        if (av_buffersrc_add_frame_flags(buffersrc_ctx, frame,
                                         AV_BUFFERSRC_FLAG_KEEP_REF) < 0) {
            av_frame_unref(frame);
            break;
        }
        av_frame_unref(frame);
        while (true) {
            ret = av_buffersink_get_frame(buffersink_ctx, filt_frame);
            if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
                break;
            }
            if (ret < 0) {
                break;
            }
            if (encode_video_frame(filt_frame) < 0) {
                av_frame_unref(filt_frame);
                break;
            }
            av_frame_unref(filt_frame);
        }
    }
    ///给滤镜图送 EOF，再取出剩余帧
    av_buffersrc_add_frame_flags(buffersrc_ctx, nullptr, AV_BUFFERSRC_FLAG_PUSH);
    while (true) {
        ret = av_buffersink_get_frame(buffersink_ctx, filt_frame);
        if (ret < 0) {
            break;
        }
        if (encode_video_frame(filt_frame) < 0) {
            av_frame_unref(filt_frame);
            break;
        }
        av_frame_unref(filt_frame);
    }
    if (flush_encoder() < 0) {
        LOGE("flush_encoder failed");
        goto end;
    }

    av_write_trailer(ofmt_ctx);
    release_all();
    av_packet_free(&pkt);
    av_packet_free(&opkt);
    av_frame_free(&frame);
    av_frame_free(&filt_frame);
    LOGD("mp4WaterMark finished -> %s", outputUrl);
    return 0;

    end:
    release_all();
    av_packet_free(&pkt);
    av_packet_free(&opkt);
    av_frame_free(&frame);
    av_frame_free(&filt_frame);
    return -1;
}

int FFmpegWaterMarkTest::open_input_file(const char *filename) {
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
    if (video_index < 0) {
        LOGD("Couldn't find video stream.\n");
        return -1;
    }

    AVStream *st = fmt_ctx->streams[video_index];
    AVCodecParameters *par = st->codecpar;

    //找到解码器
    const AVCodec *codec = avcodec_find_decoder(par->codec_id);
    if (!codec) {
        LOGD("Codec not found codec");
        return -1;
    }

    //申请AVCodecContext
    dec_ctx = avcodec_alloc_context3(codec);
    if (!dec_ctx) {
        return -1;
    }

    ret = avcodec_parameters_to_context(dec_ctx, par);
    if (ret < 0) {
        LOGD("avcodec_parameters_to_context failed: %d", ret);
        return -1;
    }
    dec_ctx->time_base = st->time_base;

    //打开解码器
    ret = avcodec_open2(dec_ctx, codec, NULL);
    if (ret < 0) {
        LOGD("avcodec_open2 failed: %d", ret);
        return -1;
    }
    return 0;
}

int FFmpegWaterMarkTest::init_filters(const char *pngUrl) {
    int ret;
    char args[512];
    std::string descr;
    const AVFilter *buffersrc = avfilter_get_by_name("buffer");
    const AVFilter *buffersink = avfilter_get_by_name("buffersink");
    AVFilterInOut *outputs = avfilter_inout_alloc();
    AVFilterInOut *inputs = avfilter_inout_alloc();
    enum AVPixelFormat pix_fmts[] = {AV_PIX_FMT_YUV420P, AV_PIX_FMT_NONE};

    filter_graph = avfilter_graph_alloc();
    if (!outputs || !inputs || !filter_graph) {
        ret = AVERROR(ENOMEM);
        goto end;
    }

    /* buffer video source: the decoded frames from the decoder will be inserted here. */
    snprintf(args, sizeof(args),
             "video_size=%dx%d:pix_fmt=%d:time_base=%d/%d:pixel_aspect=%d/%d",
             dec_ctx->width, dec_ctx->height, dec_ctx->pix_fmt,
             dec_ctx->time_base.num, dec_ctx->time_base.den,
             dec_ctx->sample_aspect_ratio.num, dec_ctx->sample_aspect_ratio.den);
    LOGD("buffer src args=%s", args);

    ret = avfilter_graph_create_filter(&buffersrc_ctx, buffersrc, "in",
                                       args, NULL, filter_graph);
    if (ret < 0) {
        LOGE("Cannot create buffer source ret:%d", ret);
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

    /// movie 读取水印图片，overlay 叠加到左上角 (10,10)
    descr = "movie=" + escape_filter_filename(pngUrl) + "[wm];[in][wm]overlay=10:10[out]";
    LOGD("filter_descr=%s", descr.c_str());

    if ((ret = avfilter_graph_parse_ptr(filter_graph, descr.c_str(),
                                        &inputs, &outputs, NULL)) < 0) {
        LOGE("avfilter_graph_parse_ptr failed: %d", ret);
        goto end;
    }
    if ((ret = avfilter_graph_config(filter_graph, NULL)) < 0) {
        LOGE("avfilter_graph_config failed: %d", ret);
        goto end;
    }

    end:
    avfilter_inout_free(&inputs);
    avfilter_inout_free(&outputs);
    return ret;
}

int FFmpegWaterMarkTest::open_output_file(const char *outputUrl) {
    int ret;
    const AVCodec *codec = avcodec_find_encoder(AV_CODEC_ID_MPEG4);
    if (!codec) {
        LOGE("mpeg4 encoder not found");
        return -1;
    }

    ret = avformat_alloc_output_context2(&ofmt_ctx, NULL, "mp4", outputUrl);
    if (ret < 0 || ofmt_ctx == nullptr) {
        LOGE("avformat_alloc_output_context2 failed: %d", ret);
        return -1;
    }

    ///视频轨：mpeg4，尺寸/时间基沿用输入
    AVStream *out_st = avformat_new_stream(ofmt_ctx, NULL);
    if (!out_st) {
        return -1;
    }
    out_video_index = (int) out_st->index;

    enc_ctx = avcodec_alloc_context3(codec);
    enc_ctx->codec_id = AV_CODEC_ID_MPEG4;
    enc_ctx->codec_type = AVMEDIA_TYPE_VIDEO;
    enc_ctx->width = dec_ctx->width;
    enc_ctx->height = dec_ctx->height;
    enc_ctx->pix_fmt = AV_PIX_FMT_YUV420P;
    enc_ctx->time_base = fmt_ctx->streams[video_index]->time_base;
    AVRational fr = fmt_ctx->streams[video_index]->avg_frame_rate;
    if (fr.num > 0 && fr.den > 0) {
        enc_ctx->framerate = fr;
    }
    enc_ctx->bit_rate = 2000000;
    enc_ctx->gop_size = 30;

    if ((ofmt_ctx->oformat->flags & AVFMT_GLOBALHEADER) != 0) {
        enc_ctx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
    }

    ret = avcodec_open2(enc_ctx, codec, NULL);
    if (ret < 0) {
        LOGE("open mpeg4 encoder failed: %d", ret);
        return -1;
    }

    ret = avcodec_parameters_from_context(out_st->codecpar, enc_ctx);
    if (ret < 0) {
        LOGE("avcodec_parameters_from_context failed: %d", ret);
        return -1;
    }
    out_st->time_base = enc_ctx->time_base;

    ///音频轨：stream copy，参数直接复制
    for (unsigned i = 0; i < fmt_ctx->nb_streams; ++i) {
        if ((int) i == video_index) {
            continue;
        }
        AVStream *ist = fmt_ctx->streams[i];
        if (ist->codecpar->codec_type != AVMEDIA_TYPE_AUDIO) {
            continue;   ///只复制音频，其余丢弃
        }
        AVStream *ost = avformat_new_stream(ofmt_ctx, NULL);
        if (!ost) {
            return -1;
        }
        ret = avcodec_parameters_copy(ost->codecpar, ist->codecpar);
        if (ret < 0) {
            return -1;
        }
        ost->codecpar->codec_tag = 0;
        ost->time_base = ist->time_base;
    }

    if (!(ofmt_ctx->oformat->flags & AVFMT_NOFILE)) {
        ret = avio_open(&ofmt_ctx->pb, outputUrl, AVIO_FLAG_WRITE);
        if (ret < 0) {
            LOGE("avio_open %s failed: %d", outputUrl, ret);
            return -1;
        }
    }

    AVDictionary *opts = nullptr;
    av_dict_set(&opts, "movflags", "faststart", 0);
    ret = avformat_write_header(ofmt_ctx, &opts);
    av_dict_free(&opts);
    if (ret < 0) {
        LOGE("avformat_write_header failed: %d", ret);
        return -1;
    }
    return 0;
}

int FFmpegWaterMarkTest::encode_video_frame(AVFrame *frame) {
    ///buffersink 输出的 pts 基于 sink pad 时基，需换算到编码器时基
    AVRational sink_tb = av_buffersink_get_time_base(buffersink_ctx);
    if (sink_tb.den > 0 &&
        (sink_tb.num != enc_ctx->time_base.num || sink_tb.den != enc_ctx->time_base.den)) {
        frame->pts = av_rescale_q(frame->pts, sink_tb, enc_ctx->time_base);
    }
    int ret = avcodec_send_frame(enc_ctx, frame);
    if (ret < 0) {
        LOGE("avcodec_send_frame failed: %d", ret);
        return ret;
    }
    AVPacket *opkt = av_packet_alloc();
    while (true) {
        ret = avcodec_receive_packet(enc_ctx, opkt);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
            break;
        } else if (ret < 0) {
            LOGE("avcodec_receive_packet failed: %d", ret);
            break;
        }
        AVStream *ost = ofmt_ctx->streams[out_video_index];
        av_packet_rescale_ts(opkt, enc_ctx->time_base, ost->time_base);
        opkt->stream_index = ost->index;
        ret = av_write_frame(ofmt_ctx, opkt);
        av_packet_unref(opkt);
        if (ret < 0) {
            LOGE("av_write_frame failed: %d", ret);
            break;
        }
    }
    av_packet_free(&opkt);
    return 0;
}

int FFmpegWaterMarkTest::flush_encoder() {
    int ret = avcodec_send_frame(enc_ctx, nullptr);
    if (ret < 0) {
        return ret;
    }
    AVPacket *opkt = av_packet_alloc();
    while (true) {
        ret = avcodec_receive_packet(enc_ctx, opkt);
        if (ret == AVERROR_EOF || ret == AVERROR(EAGAIN)) {
            break;
        } else if (ret < 0) {
            break;
        }
        AVStream *ost = ofmt_ctx->streams[out_video_index];
        av_packet_rescale_ts(opkt, enc_ctx->time_base, ost->time_base);
        opkt->stream_index = ost->index;
        ret = av_write_frame(ofmt_ctx, opkt);
        av_packet_unref(opkt);
        if (ret < 0) {
            break;
        }
    }
    av_packet_free(&opkt);
    return 0;
}

void FFmpegWaterMarkTest::release_all() {
    avfilter_graph_free(&filter_graph);
    buffersrc_ctx = nullptr;
    buffersink_ctx = nullptr;

    if (enc_ctx != nullptr) {
        avcodec_free_context(&enc_ctx);
    }
    if (ofmt_ctx != nullptr) {
        if (!(ofmt_ctx->oformat->flags & AVFMT_NOFILE)) {
            avio_closep(&ofmt_ctx->pb);
        }
        avformat_free_context(ofmt_ctx);
        ofmt_ctx = nullptr;
    }
    out_video_index = -1;

    if (dec_ctx != nullptr) {
        avcodec_free_context(&dec_ctx);
    }
    if (fmt_ctx != nullptr) {
        avformat_close_input(&fmt_ctx);
    }
    video_index = -1;
}
