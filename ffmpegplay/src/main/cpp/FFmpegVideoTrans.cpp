//
// Created by MMM on 2024/4/15.
//

#include "include/FFmpegVideoTrans.h"


int FFmpegVideoTrans::mp4ConversionAvi(const char *in_filename, const char *out_filename) {
    AVOutputFormat *ofmt = NULL;
    AVBitStreamFilterContext *vbsf = NULL;
    //定义输入、输出AVFormatContext
    AVFormatContext *ifmt_ctx = NULL, *ofmt_ctx = NULL;
    AVPacket pkt;
//    const char *in_filename, *out_filename;
    int ret, i;
    int frame_index = 0;
//    in_filename = "C:\\test\\test.mp4";//Input file URL
//    out_filename = "C:\\test\\test.avi";//Output file URL
    LOGD("in_filename:%s", in_filename);
    LOGD("out_filename:%s", out_filename);

//    av_register_all();
    //输入
    if ((ret = avformat_open_input(&ifmt_ctx, in_filename, 0, 0)) < 0) {//打开媒体文件
        LOGD("Could not open input file.");
        goto end;
    }
    if ((ret = avformat_find_stream_info(ifmt_ctx, 0)) < 0) {//获取视频信息
        LOGD("Failed to retrieve input stream information.");
        goto end;
    }
    //MP4中使用的是H.264编码，而H.264编码有两种封装模式
    //一种是annexb模式，它是传统模式，有startcode，SPS和PPS在ES中
    //另一种是mp4模式，一般MP4、MKV、AVI都没有startcode，SPS和PPS以及其他信
    //息被封装在容器中
    //每一帧前面是这一帧的长度值，很多解码器只支持annexb模式，因此需要对MP4做转换
    //在FFmpeg中用h264_mp4toannexb_filter可以进行模式转换；使用命令 - bsf
    //h264_mp4toannexb就可实现转换

    vbsf = av_bitstream_filter_init("h264_mp4toannexb");

    av_dump_format(ifmt_ctx, 0, in_filename, 0);
    //初始化输出视频码流的AVFormatContext
    avformat_alloc_output_context2(&ofmt_ctx, NULL, NULL, out_filename);
    if (!ofmt_ctx) {
        LOGD("Could not create output context\n");
        ret = AVERROR_UNKNOWN;
        goto end;
    }
    ofmt = ofmt_ctx->oformat;
    for (i = 0; i < ifmt_ctx->nb_streams; i++) {
        //通过输入的AVStream创建输出的AVStream
        AVStream *in_stream = ifmt_ctx->streams[i];
        AVStream *out_stream = avformat_new_stream(ofmt_ctx, in_stream->codec->codec);//初始化AVStream
        if (!out_stream) {
            LOGD("Could not create output context\n");
            ret = AVERROR_UNKNOWN;
            goto end;
        }
        //关键：复制AVCodecContext的设置属性
        if (avcodec_copy_context(out_stream->codec, in_stream->codec) < 0) {
            LOGD("Failed to copy context from input to output stream codec context\n");
            goto end;
        }
        out_stream->codec->codec_tag = 0;
        if (ofmt_ctx->oformat->flags & AVFMT_GLOBALHEADER)
            out_stream->codec->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
    }
    //输出信息
    av_dump_format(ofmt_ctx, 0, out_filename, 1);
    //打开输出文件
    if (!(ofmt->flags & AVFMT_NOFILE)) {
        ret = avio_open(&ofmt_ctx->pb, out_filename, AVIO_FLAG_WRITE);
        //打开输出文件
        if (ret < 0) {
            LOGD("Could not open output file '%s'", out_filename);
            goto end;
        }
    }
    //写文件头
    if (avformat_write_header(ofmt_ctx, NULL) < 0) {
        LOGD("Error occurred when opening output file\n");
        goto end;
    }

    while (1) {
        AVStream *in_stream, *out_stream;
        //得到一个AVPacket
        ret = av_read_frame(ifmt_ctx, &pkt);
        if (ret < 0)
            break;
        in_stream = ifmt_ctx->streams[pkt.stream_index];
        out_stream = ofmt_ctx->streams[pkt.stream_index];

        //=转换PTS/DTS
        pkt.pts = av_rescale_q_rnd(pkt.pts, in_stream->time_base, out_stream->time_base,
                                   (AVRounding) (AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
        pkt.dts = av_rescale_q_rnd(pkt.dts, in_stream->time_base, out_stream->time_base,
                                   (AVRounding) (AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
        pkt.duration = av_rescale_q(pkt.duration, in_stream->time_base, out_stream->time_base);
        pkt.pos = -1;

        if (pkt.stream_index == 0) {
            AVPacket fpkt = pkt;
            int a = av_bitstream_filter_filter(vbsf,
                                               out_stream->codec, nullptr
                    , &fpkt.data, &fpkt.size,
                                               pkt.data, pkt.size
                    , pkt.flags & AV_PKT_FLAG_KEY);
            pkt.data = fpkt.data;
            pkt.size = fpkt.size;
        }
        //写AVPacket
        if (av_write_frame(ofmt_ctx, &pkt) < 0) {
            //将AVPacket（存储视频压缩码流数据）写入文件
            LOGD("Error muxing packet\n");
            break;
        }
        LOGD("Write %8d frames to output file\n", frame_index);
        av_packet_unref(&pkt);
        frame_index++;
    }

    //写文件尾
    av_write_trailer(ofmt_ctx);
    LOGD("Write av_write_trailer\n");

    end:
    avformat_close_input(&ifmt_ctx);
    /*关闭输出*/
    if (ofmt_ctx && !(ofmt->flags & AVFMT_NOFILE))
        avio_close(ofmt_ctx->pb);
    avformat_free_context(ofmt_ctx);
//    system("pause");
    return 0;
}
