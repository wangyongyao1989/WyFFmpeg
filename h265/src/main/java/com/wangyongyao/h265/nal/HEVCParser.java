package com.wangyongyao.h265.nal;

import android.util.Log;

import java.io.FileInputStream;
import java.io.IOException;
import java.nio.ByteBuffer;
import java.util.ArrayList;
import java.util.List;

/**
 * author : wangyongyao https://github.com/wangyongyao1989
 * Create Time : 2025/3/13 17:28
 * Descibe : zc380 com.hyterobo.zc400.utils
 */
public class HEVCParser {
    private static final int NAL_UNIT_TYPE_VPS = 32;
    private static final int NAL_UNIT_TYPE_SPS = 33;
    private static final int NAL_UNIT_TYPE_PPS = 34;
    private static final String TAG = HEVCParser.class.getSimpleName();

    // 解析结果容器
    public static class VideoParams {
        public int width;
        public int height;
        public String colorFormat;
        public int bitDepth;
        public double frameRate;
        public int iFrameInterval;
        public long bitRate;

        // 新增字段：Profile/Tier/Level
        public String profile;   // 如 "Main", "Main 10"
        public String tier;      // 如 "Main", "High"
        public String level;     // 如 "4.1", "5.0"

        @Override
        public String toString() {
            return "VideoParams{" +
                    "width=" + width +
                    ", height=" + height +
                    ", colorFormat='" + colorFormat + '\'' +
                    ", bitDepth=" + bitDepth +
                    ", frameRate=" + frameRate +
                    ", iFrameInterval=" + iFrameInterval +
                    ", bitRate=" + bitRate +
                    ", profile='" + profile + '\'' +
                    ", tier='" + tier + '\'' +
                    ", level='" + level + '\'' +
                    '}';
        }
    }

    public static VideoParams parse(String filePath) throws IOException {
        byte[] data = readFile(filePath);
        List<NalUnit> nalUnits = findNalUnits(data);
        VideoParams params = new VideoParams();

        for (NalUnit nal : nalUnits) {
            if (nal.type == NAL_UNIT_TYPE_SPS) {
                parseSPS(nal.data, params);
            }
        }

        // 补充计算其他参数（比特率、I帧间隔）
        calculateDynamicParams(nalUnits, params, data.length);
        return params;
    }

    public static VideoParams parse(byte[] data) throws IOException {
        List<NalUnit> nalUnits = findNalUnits(data);
        VideoParams params = new VideoParams();

        for (NalUnit nal : nalUnits) {
            if (nal.type == NAL_UNIT_TYPE_SPS) {
                parseSPS(nal.data, params);
            }
        }

        // 补充计算其他参数（比特率、I帧间隔）
        calculateDynamicParams(nalUnits, params, data.length);
        return params;
    }

    private static byte[] readFile(String path) throws IOException {
        FileInputStream fis = new FileInputStream(path);
        byte[] data = new byte[fis.available()];
        fis.read(data);
        fis.close();
        return data;
    }

    // 查找所有 NAL 单元
    private static List<NalUnit> findNalUnits(byte[] data) {
        List<NalUnit> units = new ArrayList<>();
        int start = 0;
        Log.e(TAG, "findNalUnits: data"+data.length);
        while (start < data.length - 4) {
            if (data[start] == 0x00
                    && data[start + 1] == 0x00
                    && data[start + 2] == 0x01) {
                int nalType = (data[start + 3] >> 1) & 0x3F; // H.265 NAL 类型在第二个字节的高6位
                int nextStart = findNextStartCode(data, start + 3);
                int length = (nextStart == -1) ? data.length - start : nextStart - start;
                units.add(new NalUnit(nalType, data, start + 3, length - 3));
                start = nextStart;
            } else {
                start++;
            }
        }
        return units;
    }

    // 解析 SPS 核心参数
    public static void parseSPS(ByteBuffer spsData, VideoParams params) {
        BitStreamReader reader = new BitStreamReader(spsData);

        // 跳过 NAL Header 和 SPS 头部字段
        reader.readBits(4); // nal_unit_header
        int sps_video_parameter_set_id = reader.readUE();
        int sps_max_sub_layers_minus1 = reader.readBits(3);
        reader.readBit(); // temporal_id_nesting_flag

        // 解析 Profile/Tier/Level
        parseProfileTierLevel(reader, params);

        // 解析分辨率
        params.width = reader.readUE();
        params.height = reader.readUE();

        // 色度格式
        int chroma_format_idc = reader.readUE();
        params.colorFormat = getChromaFormatName(chroma_format_idc);

        // 比特深度
        if (chroma_format_idc != 0) {
            params.bitDepth = 8 + reader.readUE(); // bit_depth_luma_minus8
            reader.readUE(); // bit_depth_chroma_minus8
        }

        // 解析 VUI 参数获取帧率
        if (reader.readBit() == 1) { // vui_parameters_present_flag
            parseVUI(reader, params);
        }
    }

    // 解析 VUI 参数
    private static void parseVUI(BitStreamReader reader, VideoParams params) {
        // 跳过无关字段，直接定位到时序信息
        while (reader.readBit() == 1) ; // aspect_ratio_info_present_flag 等

        if (reader.readBit() == 1) { // timing_info_present_flag
            long num_units_in_tick = reader.readUInt(32);
            long time_scale = reader.readUInt(32);
            if (num_units_in_tick > 0) {
                params.frameRate = time_scale / (2.0 * num_units_in_tick);
            }
        }
    }

    // 计算动态参数（比特率、I帧间隔）
    private static void calculateDynamicParams(List<NalUnit> nalUnits, VideoParams params, long fileSizeBytes) {
        List<Integer> iFramePositions = new ArrayList<>();
        for (int i = 0; i < nalUnits.size(); i++) {
            NalUnit nal = nalUnits.get(i);
            if (isSliceNalUnit(nal.type)) {
                int sliceType = getSliceType(nal.data);
                if (sliceType == 2) { // I-Slice
                    iFramePositions.add(i);
                }
            }
        }

        // 计算 I 帧间隔（基于帧数）
        if (iFramePositions.size() > 1) {
            params.iFrameInterval = iFramePositions.get(1) - iFramePositions.get(0);
        }

        // 计算比特率（假设已知时长，实际需结合播放时间）
        long totalBits = fileSizeBytes * 8;
        params.bitRate = (long) (totalBits / estimateDuration(params.frameRate, nalUnits.size()));
    }

    // 工具类：位流读取
    static class BitStreamReader {
        private ByteBuffer buffer;
        private int currentByte;
        private int bitOffset = 8;

        BitStreamReader(ByteBuffer data) {
            this.buffer = data;
            nextByte();
        }

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

        int readUE() { // Exp-Golomb 无符号整数
            int leadingZeros = 0;
            while (readBit() == 0) leadingZeros++;
            return (1 << leadingZeros) - 1 + readBits(leadingZeros);
        }

        /**
         * 读取无符号整数（按大端序，支持 1~32 位）
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

    // 其他辅助方法
    private static String getChromaFormatName(int idc) {
        switch (idc) {
            case 0:
                return "4:0:0";
            case 1:
                return "4:2:0";
            case 2:
                return "4:2:2";
            case 3:
                return "4:4:4";
            default:
                return "Unknown";
        }
    }

    private static boolean isSliceNalUnit(int type) {
        return type >= 0 && type <= 31;
    }

    private static int getSliceType(ByteBuffer sliceData) {
        sliceData.position(0);
        BitStreamReader reader = new BitStreamReader(sliceData);
        reader.readUE(); // first_mb_in_slice
        return reader.readUE(); // slice_type
    }

    /**
     * 查找下一个NALU起始码位置
     *
     * @param data        原始数据数组
     * @param startOffset 开始搜索的位置
     * @return 下一个起始码的起始位置（含起始码），未找到返回-1
     */
    private static int findNextStartCode(byte[] data, int startOffset) {
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

    private static void parseProfileTierLevel(BitStreamReader reader, VideoParams params) {
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
    }

    // 辅助方法：根据 profile_idc 获取 Profile 名称
    private static String getProfileName(int profile_idc) {
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
    private static String getLevelName(int level_idc) {
        double level = level_idc / 30.0; // Level = level_idc / 30 (例如 123 → 4.1)
        int major = (int) level;
        int minor = (int) ((level - major) * 10);
        return major + "." + minor;
    }

    private static double estimateDuration(double frameRate, int totalFrames) {
        return (frameRate > 0) ? totalFrames / frameRate : 0;
    }
}
