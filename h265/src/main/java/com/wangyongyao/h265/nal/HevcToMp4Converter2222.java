package com.wangyongyao.h265.nal;


import android.media.MediaCodec;
import android.media.MediaCodecInfo;
import android.media.MediaFormat;
import android.media.MediaMuxer;
import android.util.Log;

import java.io.FileInputStream;
import java.nio.ByteBuffer;
import java.util.ArrayList;
import java.util.List;

/**
 * author : wangyongyao https://github.com/wangyongyao1989
 * Create Time : 2025/3/18 9:36
 * Descibe : MyyFFmpeg com.wangyongyao.h265
 */
public class HevcToMp4Converter2222 {

    private static final String TAG = "HevcToMp4Converter";
    private static final String MIME_TYPE_HEVC = "video/hevc";
    private static final int NAL_START_CODE = 0x00000001;

    public static void convert(String inputPath, String outputPath) {
        MediaMuxer muxer = null;
        try {
            // 1. 读取并解析NAL单元
            FileInputStream fis = new FileInputStream(inputPath);
            byte[] rawData = new byte[fis.available()];
            fis.read(rawData);
            fis.close();
            List<NalUnit> nalUnits = parseNalUnits(rawData);
            Log.e(TAG, "nalUnits: " + nalUnits.size());

            // 2. 提取SPS/PPS并添加起始码
            ByteBuffer sps = null, pps = null;
            for (NalUnit nal : nalUnits) {
                if (nal.type == 33) { // SPS
                    sps = addStartCode(nal.data);
                } else if (nal.type == 34) { // PPS
                    pps = addStartCode(nal.data);
                }
            }
            if (sps == null || pps == null) {
                throw new IllegalArgumentException("SPS/PPS missing");
            }

            // 3. 创建MediaFormat
            MediaFormat format = createMediaFormat(sps, pps);

            // 4. 初始化MediaMuxer
            muxer = new MediaMuxer(outputPath, MediaMuxer.OutputFormat.MUXER_OUTPUT_MPEG_4);
            int trackIndex = muxer.addTrack(format);
            muxer.start();

            // 5. 写入参数集和视频帧
            writeParameterSets(muxer, trackIndex, sps, pps);
            writeVideoFrames(muxer, trackIndex, nalUnits, format);

        } catch (Exception e) {
            Log.e(TAG, "Error: " + e.getMessage());
        } finally {
            if (muxer != null) {
                try {
//                    if (isMuxerStarted(muxer)) {
                        muxer.stop();
//                    }
                } catch (IllegalStateException e) {
                    Log.e(TAG, "Stop failed: " + e.getMessage());
                }
                muxer.release();
            }
        }
    }

    //-------------------------- 关键修复方法 --------------------------//

    /**
     * 在SPS/PPS数据前添加起始码
     */
    private static ByteBuffer addStartCode(byte[] nalData) {
        ByteBuffer buffer = ByteBuffer.allocate(nalData.length + 4);
        buffer.putInt(NAL_START_CODE);
        buffer.put(nalData);
        buffer.flip();
        return buffer;
    }

    /**
     * 创建MediaFormat（确保包含起始码）
     */
    private static MediaFormat createMediaFormat(ByteBuffer sps, ByteBuffer pps) {
        // 解析分辨率（需实现 parseResolution 方法）
        int[] resolution = parseResolution(sps);

        MediaFormat format = MediaFormat.createVideoFormat(MIME_TYPE_HEVC, resolution[0], resolution[1]);
        format.setByteBuffer("csd-0", sps);
        format.setByteBuffer("csd-1", pps);
        format.setInteger(MediaFormat.KEY_FRAME_RATE, 30);
        format.setInteger(MediaFormat.KEY_COLOR_FORMAT, MediaCodecInfo.CodecCapabilities.COLOR_FormatYUV420Flexible);
        return format;
    }

    /**
     * 写入参数集（确保起始码正确）
     */
    private static void writeParameterSets(MediaMuxer muxer, int trackIndex, ByteBuffer sps, ByteBuffer pps) {
        MediaCodec.BufferInfo info = new MediaCodec.BufferInfo();
        info.flags = MediaCodec.BUFFER_FLAG_CODEC_CONFIG;

        // 写入SPS
        info.size = sps.remaining();
        muxer.writeSampleData(trackIndex, sps.duplicate(), info);

        // 写入PPS
        info.size = pps.remaining();
        muxer.writeSampleData(trackIndex, pps.duplicate(), info);
    }

    /**
     * 写入视频帧（强制时间戳递增）
     */
    private static void writeVideoFrames(MediaMuxer muxer, int trackIndex, List<NalUnit> nalUnits, MediaFormat format) {
        long frameDurationUs = 1_000_000L / format.getInteger(MediaFormat.KEY_FRAME_RATE);
        long presentationTimeUs = 0;
        ByteBuffer buffer = ByteBuffer.allocateDirect(4 * 1024 * 1024);

        for (NalUnit nal : nalUnits) {
            if (isVideoFrame(nal.type)) {
                buffer.clear();
                buffer.putInt(NAL_START_CODE);
                buffer.put(nal.data);
                buffer.flip();

                MediaCodec.BufferInfo info = new MediaCodec.BufferInfo();
                info.presentationTimeUs = presentationTimeUs;
                info.size = buffer.remaining();
                info.flags = (nal.type == 19 || nal.type == 20) ? MediaCodec.BUFFER_FLAG_KEY_FRAME : 0;

                muxer.writeSampleData(trackIndex, buffer, info);
                presentationTimeUs += frameDurationUs;

                // 强制至少写入一帧数据
                if (presentationTimeUs == 0) {
                    presentationTimeUs = 1; // 避免时间戳为0
                }
            }
        }
    }

//    /**
//     * 检查MediaMuxer是否已启动
//     */
//    private static boolean isMuxerStarted(MediaMuxer muxer) {
//        try {
//            muxer.
//            muxer.getTrackCount(); // 若未启动会抛出异常
//            return true;
//        } catch (IllegalStateException e) {
//            return false;
//        }
//    }

    /**
     * 从SPS解析分辨率
     */
    private static int[] parseResolution(ByteBuffer sps) {
        try {
            H265ToMp4Converter.BitStreamReader reader = new H265ToMp4Converter.BitStreamReader(sps.duplicate());
            reader.readBits(16); // 跳过NAL头
            reader.readUE();     // sps_video_parameter_set_id
            int sps_max_sub_layers_minus1 = reader.readBits(3);// sps_max_sub_layers_minus1
            reader.readBit();    // temporal_id_nesting_flag
            parseProfileTierLevel(reader,sps_max_sub_layers_minus1); // 跳过profile_tier_level

            int width = reader.readUE();
            int height = reader.readUE();
            Log.e(TAG, "parseResolution width:" + width + ", height" + height);

            return new int[]{704, 576};
        } catch (Exception e) {
            Log.e(TAG, "Resolution parsing failed: " + e.getMessage());
            return null;
        }
    }

    /**
     * 解析profile_tier_level（跳过不影响分辨率）
     */
    private static void parseProfileTierLevel(H265ToMp4Converter.BitStreamReader reader, int maxSubLayers) {
        // 1. 通用配置
        int general_profile_space = reader.readBits(2);
        boolean general_tier_flag = reader.readBit() == 1;
        int general_profile_idc = reader.readBits(5);

        // 2. Profile 兼容性标志（32 bits）
        for (int i = 0; i < 32; i++) {
            reader.readBit();
        }

        // 3. 约束标志（6 bits）
        reader.readBit(); // general_progressive_source_flag
        reader.readBit(); // general_interlaced_source_flag
        reader.readBit(); // general_non_packed_constraint_flag
        reader.readBit(); // general_frame_only_constraint_flag
        reader.readBit(); // general_reserved_zero_44bits (跳过2位)
        reader.readBit();

        // 4. Level 信息
        int general_level_idc = reader.readBits(8);

        // 5. 子层信息（根据 maxSubLayers）
        for (int i = 0; i < maxSubLayers; i++) {
            reader.readBit(); // sub_layer_profile_present_flag
            reader.readBit(); // sub_layer_level_present_flag
        }

        // 6. 跳过保留位
        if (maxSubLayers > 0) {
            for (int i = maxSubLayers; i < 8; i++) {
                reader.readBits(2); // reserved_zero_2bits
            }
        }
    }

    private static boolean isVideoFrame(int nalType) {
        return nalType >= 0 && nalType <= 31
                && nalType != 32 // 排除VPS
                && nalType != 33 // 排除SPS
                && nalType != 34; // 排除PPS
    }

    /**
     * 解析NAL单元（支持3字节和4字节起始码）
     */
    private static List<NalUnit> parseNalUnits(byte[] data) {
        List< NalUnit> units = new ArrayList<>();
        int start = 0;
        while (start < data.length - 4) {
            if (start < 0) break;
            if (data[start] == 0x00 && data[start + 1] == 0x00) {
                int prefixLength = 0;
                if (data[start + 2] == 0x01) { // 3字节起始码 0x000001
                    prefixLength = 3;
                } else if (data[start + 2] == 0x00 && data[start + 3] == 0x01) { // 4字节起始码 0x00000001
                    prefixLength = 4;
                }

                if (prefixLength > 0) {
                    int nalStart = start + prefixLength;
                    int nalType = (data[nalStart] >> 1) & 0x3F; // 提取NAL类型

                    int nextStart = findNextStartCode(data, start + prefixLength);
                    int length = (nextStart == -1) ? data.length - nalStart : nextStart - nalStart;

                    byte[] nalData = new byte[length];
                    System.arraycopy(data, nalStart, nalData, 0, length);
                    units.add(new  NalUnit(nalType, nalData));

                    start = nextStart;
                    continue;
                }
            }
            start++;
        }
        return units;
    }

    /**
     * 查找下一个起始码位置
     */
    private static int findNextStartCode(byte[] data, int start) {
        for (int i = start; i < data.length - 4; i++) {
            if (data[i] == 0x00 && data[i + 1] == 0x00 && data[i + 2] == 0x01) {
                return i;
            } else if (data[i] == 0x00 && data[i + 1] == 0x00 && data[i + 2] == 0x00 && data[i + 3] == 0x01) {
                return i;
            }
        }
        return -1;
    }

    static class NalUnit {
        final int type;
        final byte[] data;

        NalUnit(int type, byte[] data) {
            this.type = type;
            this.data = data;
        }
    }

}
