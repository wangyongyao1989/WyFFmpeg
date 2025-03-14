package com.wangyongyao.h265;

import android.media.MediaCodec;
import android.media.MediaFormat;
import android.media.MediaMuxer;
import android.util.Log;

import java.io.IOException;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;

/**
 * Create Time : 2025/3/12 14:35
 * Descibe : zc380 com.hyterobo.zc400.utils
 */
public class H265ToMp4Muxer {
    private static final String TAG = "H265Muxer";
    private static final int NALU_START_CODE_LONG = 0x00000001;
    private static final int NALU_START_CODE_SHORT = 0x000001;

    private MediaMuxer mediaMuxer;
    private int mVideoTrackIndex = -1;
    private boolean mMuxerStarted = false;

    private static final int FRAME_RATE = 25;
    private long frameIndex = 0;


    // HEVC参数集缓存
    private byte[] mVps;
    private byte[] mSps;
    private byte[] mPps;

    /**
     * 初始化MP4封装器
     *
     * @param outputPath 输出文件路径
     */
    public H265ToMp4Muxer(String outputPath) {
        try {
            // 创建 MediaMuxer 封装器，用于将编码后的数据封装成 MP4 文件
            mediaMuxer = new MediaMuxer(outputPath, MediaMuxer.OutputFormat.MUXER_OUTPUT_MPEG_4);
        } catch (IOException e) {
            e.printStackTrace();
            Log.e(TAG, "H265ToMp4Muxer: IOException :" + e);
        }

    }

    /**
     * 写入H265裸流数据
     *
     * @param data H265数据块（可能包含多个NALU）
     */
    public void writeSample(byte[] data) {
        int offset = 0;
        while (offset < data.length) {
            // 查找NALU起始码
            int[] startCode = findStartCode(data, offset);
            if (startCode == null) break;

            int naluStart = startCode[0];
            int naluEnd = findNextStartCode(data, naluStart + 1);
            if (naluEnd == -1) naluEnd = data.length;

            processNalu(data, naluStart, naluEnd);

            if (mSps != null && mSps.length > 0) {
                HEVCParser.VideoParams videoParams = new HEVCParser.VideoParams();
                ByteBuffer spsData = ByteBuffer.wrap(mSps, 0, mSps.length)
                        .asReadOnlyBuffer()
                        .order(ByteOrder.BIG_ENDIAN);

                HEVCParser.parseSPS(spsData, videoParams);
                Log.e(TAG, "videoParams: " + videoParams);
                mSps = null;
            }

            offset = naluEnd;
        }
    }

    /**
     * 处理单个NALU单元
     */
    private void processNalu(byte[] data, int start, int end) {
        int naluType = (data[start] & 0x7E) >> 1; // HEVC NALU类型

        switch (naluType) {
            case 32: // VPS : 00 00 01 40  --> (0x40 & 0x7E) >> 1 = 32
                mVps = extractNalu(data, start, end);
                break;
            case 33: // SPS : 00 00 01 42  --> (0x42 & 0x7E) >> 1 = 33
                mSps = extractNalu(data, start, end);
                break;
            case 34: // PPS : 00 00 01 44 --> (0x44 & 0x7E) >> 1 = 34
                mPps = extractNalu(data, start, end);
                break;
            case 19: // IDR帧 : 00 00 01 26 --> (0x26 & 0x7E) >> 1 = 19
            case 1:  // 非关键帧
                if (prepareMuxer()) {
                    Log.e(TAG, "processNalu: writeFrame IDR naluType:" + naluType);
                    writeFrame(data, start, end, naluType);
                }
                break;
        }
    }

    /**
     * 准备复用器（首次写入帧数据时触发）
     */
    private synchronized boolean prepareMuxer() {
        if (mMuxerStarted) return true;
        if (mVps == null || mSps == null || mPps == null) return false;

        try {
            MediaFormat format = createMediaFormat();
            mVideoTrackIndex = mediaMuxer.addTrack(format);
            mediaMuxer.start();
            mMuxerStarted = true;
            return true;
        } catch (Exception e) {
            e.printStackTrace();
            return false;
        }
    }

    /**
     * 创建HEVC媒体格式
     */
    private MediaFormat createMediaFormat() {
        MediaFormat format = MediaFormat.createVideoFormat(MediaFormat.MIMETYPE_VIDEO_HEVC, 0, 0);

        // HEVC参数集
        ByteBuffer csd = ByteBuffer.allocate(mVps.length + mSps.length + mPps.length + 12);
        csd.put((byte) 0x01); // configurationVersion
        csd.put(mVps);        // vps
        csd.put((byte) 0x01); // numOfSps
        csd.putShort((short) mSps.length);
        csd.put(mSps);        // sps
        csd.put((byte) 0x01); // numOfPps
        csd.putShort((short) mPps.length);
        csd.put(mPps);        // pps
        csd.flip();

        format.setByteBuffer("csd-0", csd);
        format.setInteger(MediaFormat.KEY_FRAME_RATE, FRAME_RATE);
        return format;
    }

    /**
     * 写入视频帧数据
     */
    private void writeFrame(byte[] data, int start, int end, int naluType) {
        ByteBuffer buffer = ByteBuffer.wrap(data, start, end - start);
        MediaCodec.BufferInfo info = new MediaCodec.BufferInfo();

        info.offset = 0;
        info.size = end - start;
        info.presentationTimeUs = computePresentationTime(frameIndex);
        info.flags = (naluType == 19) ? MediaCodec.BUFFER_FLAG_KEY_FRAME : 0;

        mediaMuxer.writeSampleData(mVideoTrackIndex, buffer, info);
    }

    /**
     * 释放资源
     */
    public void release() {
        if (mediaMuxer != null) {
            try {
                if (mMuxerStarted) {
                    if (mVideoTrackIndex < 0) return;
                    MediaCodec.BufferInfo bufferInfo = new MediaCodec.BufferInfo();
                    bufferInfo.offset = 0;
                    bufferInfo.size = 0;
                    bufferInfo.flags = MediaCodec.BUFFER_FLAG_END_OF_STREAM;
                    bufferInfo.presentationTimeUs = computePresentationTime(frameIndex);
                    mediaMuxer.writeSampleData(
                            mVideoTrackIndex,
                            ByteBuffer.allocateDirect(0), bufferInfo);
                    Log.e(TAG, "mediaMuxer.stop()========================== ");
                    mediaMuxer.stop();
                }
                mediaMuxer.release();
            } catch (Exception e) {
                e.printStackTrace();
                Log.e(TAG, "H265ToMp4Muxer: release() Exception:" + e);
            }
            mediaMuxer = null;
        }
    }

    // 辅助方法：查找起始码
    private int[] findStartCode(byte[] data, int offset) {
        for (int i = offset; i < data.length - 3; i++) {
            if ((data[i] == 0x00 && data[i + 1] == 0x00
                    && data[i + 2] == 0x00
                    && data[i + 3] == 0x01)) {
                return new int[]{i + 4, 4}; // 返回起始码结束位置和长度
            } else if ((data[i] == 0x00
                    && data[i + 1] == 0x00
                    && data[i + 2] == 0x01)) {
                return new int[]{i + 3, 3};
            }
        }
        return null;
    }

    // 辅助方法：提取NALU内容（去除起始码）
    private byte[] extractNalu(byte[] data, int start, int end) {
        byte[] nalu = new byte[end - start];
        System.arraycopy(data, start, nalu, 0, nalu.length);
        return nalu;
    }

    /**
     * 查找下一个NALU起始码位置
     *
     * @param data        原始数据数组
     * @param startOffset 开始搜索的位置
     * @return 下一个起始码的起始位置（含起始码），未找到返回-1
     */
    private int findNextStartCode(byte[] data, int startOffset) {
        // 从指定偏移开始遍历（至少需要3字节空间）
        for (int i = startOffset; i < data.length - 3; i++) {
            // 检查4字节起始码：00 00 00 01
            if (data[i] == 0x00 &&
                    data[i + 1] == 0x00 &&
                    data[i + 2] == 0x00 &&
                    data[i + 3] == 0x01) {
                return i; // 返回起始码的第一个字节位置
            }

            // 检查3字节起始码：00 00 01
            if (data[i] == 0x00 &&
                    data[i + 1] == 0x00 &&
                    data[i + 2] == 0x01) {
                return i; // 返回起始码的第一个字节位置
            }
        }

        // 处理末尾可能的3字节情况（最后3个字节）
        if (data.length - startOffset >= 3) {
            int i = data.length - 3;
            if (data[i] == 0x00 &&
                    data[i + 1] == 0x00 &&
                    data[i + 2] == 0x01) {
                return i;
            }
        }

        return -1; // 未找到下一个起始码
    }

    private static long computePresentationTime(long frameIndex) {
        long frameIndexTime = frameIndex * 1000000 / FRAME_RATE;
        long time = 42 + frameIndexTime;
        return time;
    }

}
