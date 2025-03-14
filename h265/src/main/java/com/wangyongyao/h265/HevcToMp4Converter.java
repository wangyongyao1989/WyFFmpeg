package com.wangyongyao.h265;


import android.media.MediaCodec;
import android.media.MediaFormat;
import android.media.MediaMuxer;
import android.util.Log;

import java.io.FileInputStream;
import java.io.IOException;
import java.nio.ByteBuffer;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

/**
 * author : wangyongyao https://github.com/wangyongyao1989
 * Create Time : 2025/3/14 14:31
 * Descibe : MyyFFmpeg com.wangyongyao.h265
 */
public class HevcToMp4Converter {
    private final String TAG = "HevcToMp4Converter";
    private final String MIME_TYPE_HEVC = "video/hevc";
    private final int NAL_UNIT_TYPE_VPS = 32;
    private final int NAL_UNIT_TYPE_SPS = 33;
    private final int NAL_UNIT_TYPE_PPS = 34;
    private final int NAL_UNIT_TYPE_IDR_N_LP = 19;
    private final int NAL_UNIT_TYPE_IDR_W_RADL = 20;

    private long frameIndex = 0;
    private final int FRAME_RATE = 25;

    public void convert(String inputPath, String outputPath) {
        MediaMuxer muxer = null;
        FileInputStream fis = null;
        try {
            // 1. 读取H.265裸流文件
            fis = new FileInputStream(inputPath);
            byte[] rawData = new byte[fis.available()];
            fis.read(rawData);
            fis.close();

            // 2. 解析NAL单元
            List<NalUnit> nalUnits = parseNalUnits(rawData);

            // 3. 提取VPS/SPS/PPS并创建MediaFormat
            MediaFormat format = createMediaFormat(nalUnits);
            if (format == null) {
                Log.e(TAG, "Failed to create MediaFormat: SPS/PPS missing");
                return;
            }

            // 4. 初始化MediaMuxer
            muxer = new MediaMuxer(outputPath, MediaMuxer.OutputFormat.MUXER_OUTPUT_MPEG_4);
            int trackIndex = muxer.addTrack(format);
            muxer.start();

            // 5. 写入视频数据
            ByteBuffer buffer = ByteBuffer.allocateDirect(4 * 1024 * 1024); // 4MB缓冲区
            long presentationTimeUs = 0;
            long frameDurationUs = calculateFrameDuration(frameIndex); // 动态计算帧间隔
            boolean firstFrame = true;

            for (NalUnit nal : nalUnits) {
                if (isVideoData(nal.type)) {
                    // 添加起始码0x00000001
                    buffer.clear();
                    buffer.putInt(0x00000001);
                    buffer.put(nal.data);
                    buffer.flip();

                    MediaCodec.BufferInfo info = new MediaCodec.BufferInfo();
                    info.presentationTimeUs = presentationTimeUs;
                    info.size = buffer.remaining();
                    info.offset = 0;

                    // 标记关键帧
                    if (nal.type == NAL_UNIT_TYPE_IDR_N_LP
                            || nal.type == NAL_UNIT_TYPE_IDR_W_RADL) {
                        frameIndex++;
                        info.flags = MediaCodec.BUFFER_FLAG_KEY_FRAME;
                        Log.d(TAG, "Key frame detected at " + presentationTimeUs + "μs");
                    }

                    // 首次写入需包含SPS/PPS
                    if (firstFrame) {
                        writeParameterSets(muxer, trackIndex, buffer, info, nalUnits);
                        firstFrame = false;
                    }

                    muxer.writeSampleData(trackIndex, buffer, info);
                    presentationTimeUs += frameDurationUs;
                }
            }

            Log.d(TAG, "Successfully wrote " + nalUnits.size() + " NAL units");

            //写文件末尾标志
            MediaCodec.BufferInfo bufferInfo = new MediaCodec.BufferInfo();
            bufferInfo.offset = 0;
            bufferInfo.size = 0;
            bufferInfo.flags = MediaCodec.BUFFER_FLAG_END_OF_STREAM;
            bufferInfo.presentationTimeUs = calculateFrameDuration(frameIndex);
            muxer.writeSampleData(
                    trackIndex,
                    ByteBuffer.allocateDirect(0), bufferInfo);
        } catch (IOException e) {
            Log.e(TAG, "IO error: " + e.getMessage());
        } finally {
            if (muxer != null) {
                try {
                    muxer.stop();
                } catch (IllegalStateException e) {
                    Log.e(TAG, "Failed to stop muxer: " + e.getMessage());
                }
                muxer.release();
            }
            if (fis != null) {
                try {
                    fis.close();
                } catch (IOException e) {
                    Log.e(TAG, "Failed to close input stream: " + e.getMessage());
                }
            }
        }
    }

    // 解析NAL单元（支持4字节起始码0x00 00 00 01）
    private List<NalUnit> parseNalUnits(byte[] data) {
        List<NalUnit> units = new ArrayList<>();
        int start = 0;
        while (start < data.length - 4) {
            if (start < 0) break;
            if (data[start] == 0x00 && data[start + 1] == 0x00) {
                int nalStart = -1;
                int prefixLength = 0;

                // 检测3字节或4字节起始码
                if (data[start + 2] == 0x01) { // 3字节 0x000001
                    nalStart = start + 3;
                    prefixLength = 3;
                } else if (data[start + 2] == 0x00 && data[start + 3] == 0x01) { // 4字节 0x00000001
                    nalStart = start + 4;
                    prefixLength = 4;
                }

                if (nalStart != -1) {
                    int nalType = (data[nalStart] >> 1) & 0x3F;
                    int nextStart = findNextStartCode(data, start + prefixLength);
                    int length = (nextStart == -1) ? data.length - nalStart : nextStart - nalStart;

                    byte[] nalData = new byte[length];
                    System.arraycopy(data, nalStart, nalData, 0, length);
                    units.add(new NalUnit(nalType, nalData));

                    start = nextStart;
                    continue;
                }
            }
            start++;
        }
        return units;
    }

    // 创建包含HEVC参数的MediaFormat
    private MediaFormat createMediaFormat(List<NalUnit> nalUnits) {
        ByteBuffer vps = null, sps = null, pps = null;

        for (NalUnit nal : nalUnits) {
            switch (nal.type) {
                case NAL_UNIT_TYPE_VPS:
                    Log.d(TAG, "Successfully NAL_UNIT_TYPE_VPS ");
                    vps = ByteBuffer.wrap(nal.data);
                    byte[] array = vps.array();
                    Log.d(TAG, "NAL_UNIT_TYPE_VPS ===:"+ Arrays.toString(array));
                    H265NALParser.parseVPS(vps.array());
                    break;
                case NAL_UNIT_TYPE_SPS:
                    Log.d(TAG, "Successfully NAL_UNIT_TYPE_SPS ");
                    sps = ByteBuffer.wrap(nal.data);
                    H265NALParser.parseSPS(vps.array());
                    break;
                case NAL_UNIT_TYPE_PPS:
                    Log.d(TAG, "Successfully NAL_UNIT_TYPE_PPS ");
                    pps = ByteBuffer.wrap(nal.data);
                    H265NALParser.parsePPS(vps.array());
                    break;
            }
        }

        if (sps == null || pps == null) {
            Log.e(TAG, "Critical parameters missing: SPS="
                    + (sps != null) + ", PPS=" + (pps != null));
            return null;
        }

        // 从SPS解析真实分辨率
        int[] resolution = parseResolution(sps);
        Log.e(TAG, "分辨率resolution weight：" + resolution[0] + ",height:" + resolution[1]);
        if (resolution == null) {
            Log.e(TAG, "Failed to parse resolution from SPS");
            return null;
        }

        MediaFormat format = MediaFormat.createVideoFormat(MIME_TYPE_HEVC, resolution[0], resolution[1]);
        format.setByteBuffer("csd-0", sps); // HEVC的SPS
        format.setByteBuffer("csd-1", pps); // HEVC的PPS
        if (vps != null) {
            format.setByteBuffer("csd-2", vps); // 可选VPS
        }

        // 设置帧率（需从VUI解析）
        double frameRate = 30.0; // 默认值
        format.setInteger(MediaFormat.KEY_FRAME_RATE, (int) frameRate);
        Log.d(TAG, String.format("Created format: %dx%d @ %.2f fps"
                , resolution[0], resolution[1], frameRate));

        return format;
    }

    private int[] parseResolution(ByteBuffer sps) {
        try {
            sps.position(0);
            BitStreamReader reader = new BitStreamReader(sps);

            // 1. 跳过 NAL 头 (2 bytes)
            // nal_unit_header (forbidden_zero_bit + nal_unit_type + nuh_layer_id + nuh_temporal_id_plus1)
//            reader.readBits(16);

            // 2. 解析 SPS 基础字段
            int sps_video_parameter_set_id = reader.readUE();
            int sps_max_sub_layers_minus1 = reader.readBits(3);
            boolean temporal_id_nesting_flag = reader.readBit() == 1;

            // 3. 解析 profile_tier_level (需完整跳过)
            parseProfileTierLevel(reader, sps_max_sub_layers_minus1);

            // 4. 解析分辨率
            int pic_width_in_luma_samples = reader.readUE();
            int pic_height_in_luma_samples = reader.readUE();

            // 5. 处理裁边参数（conformance_window）
            boolean conformance_window_flag = reader.readBit() == 1;
            if (conformance_window_flag) {
                reader.readUE(); // conf_win_left_offset
                reader.readUE(); // conf_win_right_offset
                reader.readUE(); // conf_win_top_offset
                reader.readUE(); // conf_win_bottom_offset
            }

            return new int[]{pic_width_in_luma_samples, pic_height_in_luma_samples};
        } catch (Exception e) {
            Log.e(TAG, "SPS resolution parsing failed: " + e.getMessage());
            return null;
        }
    }

    // 精确解析 profile_tier_level 结构
    private void parseProfileTierLevel(BitStreamReader reader, int maxSubLayers) {
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

    // 写入SPS/PPS到文件头部
    private void writeParameterSets(MediaMuxer muxer, int trackIndex,
                                    ByteBuffer buffer, MediaCodec.BufferInfo info,
                                    List<NalUnit> nalUnits) {
        for (NalUnit nal : nalUnits) {
            if (nal.type == NAL_UNIT_TYPE_SPS || nal.type == NAL_UNIT_TYPE_PPS) {
                buffer.clear();
                buffer.putInt(0x00000001);
                buffer.put(nal.data);
                buffer.flip();

                info.size = buffer.remaining();
                info.flags = MediaCodec.BUFFER_FLAG_CODEC_CONFIG;
                muxer.writeSampleData(trackIndex, buffer, info);
                Log.d(TAG, "Wrote parameter set: type=" + nal.type);
            }
        }
    }

    // 其他辅助方法
    private int findNextStartCode(byte[] data, int start) {
        for (int i = start; i < data.length - 4; i++) {
            if (data[i] == 0x00 && data[i + 1] == 0x00 &&
                    (data[i + 2] == 0x01 || (data[i + 2] == 0x00 && data[i + 3] == 0x01))) {
                return i;
            }
        }
        return -1;
    }

    private boolean isVideoData(int nalType) {
        return nalType >= 0 && nalType <= 31 &&
                nalType != NAL_UNIT_TYPE_VPS &&
                nalType != NAL_UNIT_TYPE_SPS &&
                nalType != NAL_UNIT_TYPE_PPS;
    }

    private long calculateFrameDuration(long frameIndex) {
//        // 实际项目应从SPS的VUI解析，此处简化
//        return 33_333; // 默认30fps (1,000,000μs / 30 ≈ 33333)
        long frameIndexTime = frameIndex * 1000000 / FRAME_RATE;
        long time = 42 + frameIndexTime;
        return time;
    }

    // NAL单元封装类
    class NalUnit {
        final int type;
        final byte[] data;

        NalUnit(int type, byte[] data) {
            this.type = type;
            this.data = data;
        }
    }

    // 位流读取工具（需完整实现）
    class BitStreamReader {
        // 实现参考之前的代码，需包含readBits(), readUE(), readUInt()等方法
        private ByteBuffer buffer;
        private int currentByte = 0;
        private int bitOffset = 8;  //初始状态无可用位

        BitStreamReader(ByteBuffer data) {
            this.buffer = data;
            nextByte();
        }

        // 读取单个比特
        int readBit() {
            if (bitOffset == 8) nextByte();
            int bit = (currentByte >> (7 - bitOffset)) & 0x01;
            bitOffset++;
            return bit;
        }

        int readBits(int n) {
            int value = 0;
            for (int i = 0; i < n; i++) {
                value = (value << 1) | readBit();
            }
            return value;
        }

        // 读取无符号指数哥伦布码 (UE)
        int readUE() { // Exp-Golomb 无符号整数
            int leadingZeros = 0;
            while (readBit() == 0) {
                leadingZeros++;
            }
            if (leadingZeros == 0) {
                return 0;
            }
            int value = (1 << leadingZeros) - 1;
            for (int i = 0; i < leadingZeros; i++) {
                value += readBit() << (leadingZeros - 1 - i);
            }
            return value;
        }


        /**
         * 读取无符号整数（按大端序，支持 1~32 位）
         *
         * @param bits 要读取的位数（例如 32）
         * @return 无符号整数（以 long 存储，避免溢出）
         */
        public long readUInt(int bits) {
            if (bits < 1 || bits > 32) {
                throw new IllegalArgumentException("bits must be between 1 and 32");
            }

            long value = 0;
            for (int i = 0; i < bits; i++) {
                value = (value << 1) | readBit();
            }
            return value;
        }

        private void nextByte() {
            if (buffer.hasRemaining()) {
                currentByte = buffer.get() & 0xFF;
                bitOffset = 0;
            } else {
                currentByte = 0;
                bitOffset = 8;
            }
        }
    }

    private void parseProfileTierLevel(BitStreamReader reader, HEVCParser.VideoParams params) {
        // 1. 解析 Profile 相关字段
        int general_profile_space = reader.readBits(2);  // 2 bits
        boolean general_tier_flag = (reader.readBit() == 1); // 1 bit
        int general_profile_idc = reader.readBits(5);    // 5 bits

        // 2. 读取 Profile 兼容性标志（32 bits）
        int[] general_profile_compatibility = new int[32];
        for (int i = 0; i < 32; i++) {
            general_profile_compatibility[i] = reader.readBit();
        }

        // 3. 解析约束标志（6 bits）
        reader.readBit(); // general_progressive_source_flag
        reader.readBit(); // general_interlaced_source_flag
        reader.readBit(); // general_non_packed_constraint_flag
        reader.readBit(); // general_frame_only_constraint_flag
        reader.readBit(); // reserved (0)
        reader.readBit(); // reserved (0)

        // 4. 解析 Level
        int general_level_idc = reader.readBits(8); // 8 bits

        // 5. 将数值转换为可读字符串
        params.profile = getProfileName(general_profile_idc);
        params.tier = general_tier_flag ? "High" : "Main";
        params.level = getLevelName(general_level_idc);
        Log.e(TAG, "parseProfileTierLevel: VideoParams:" + params.toString());
    }

    // 辅助方法：根据 profile_idc 获取 Profile 名称
    private String getProfileName(int profile_idc) {
        switch (profile_idc) {
            case 1:
                return "Main";
            case 2:
                return "Main 10";
            case 3:
                return "Main Still Picture";
            case 4:
                return "Rext"; // Range Extensions
            case 5:
                return "Scc";  // Screen Content Coding
            default:
                return "Unknown";
        }
    }

    // 辅助方法：根据 level_idc 获取 Level 名称
    private String getLevelName(int level_idc) {
        double level = level_idc / 30.0; // Level = level_idc / 30 (例如 123 → 4.1)
        int major = (int) level;
        int minor = (int) ((level - major) * 10);
        return major + "." + minor;
    }
}
