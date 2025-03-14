package com.wangyongyao.h265;


import android.media.MediaCodec;
import android.media.MediaFormat;
import android.media.MediaMuxer;
import android.util.Log;

import java.io.FileInputStream;
import java.io.IOException;
import java.nio.ByteBuffer;
import java.util.ArrayList;
import java.util.List;

/**
 * author : wangyongyao https://github.com/wangyongyao1989
 * Create Time : 2025/3/14 14:31
 * Descibe : MyyFFmpeg com.wangyongyao.h265
 */
public class HevcToMp4Converter {
    private static final String TAG = "HevcToMp4Converter";
    private static final String MIME_TYPE_HEVC = "video/hevc";
    private static final int NAL_UNIT_TYPE_VPS = 32;
    private static final int NAL_UNIT_TYPE_SPS = 33;
    private static final int NAL_UNIT_TYPE_PPS = 34;
    private static final int NAL_UNIT_TYPE_IDR_N_LP = 19;
    private static final int NAL_UNIT_TYPE_IDR_W_RADL = 20;

    public static void convert(String inputPath, String outputPath) {
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
            long frameDurationUs = calculateFrameDuration(nalUnits); // 动态计算帧间隔
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
    private static List<NalUnit> parseNalUnits(byte[] data) {
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
    private static MediaFormat createMediaFormat(List<NalUnit> nalUnits) {
        ByteBuffer vps = null, sps = null, pps = null;

        for (NalUnit nal : nalUnits) {
            switch (nal.type) {
                case NAL_UNIT_TYPE_VPS:
                    vps = ByteBuffer.wrap(nal.data);
                    break;
                case NAL_UNIT_TYPE_SPS:
                    sps = ByteBuffer.wrap(nal.data);
                    break;
                case NAL_UNIT_TYPE_PPS:
                    pps = ByteBuffer.wrap(nal.data);
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

    // 从SPS解析分辨率
    private static int[] parseResolution(ByteBuffer sps) {
        try {
            sps.position(0);
            byte[] spsBytes = new byte[sps.remaining()];
            sps.get(spsBytes);
            BitStreamReader reader = new BitStreamReader(ByteBuffer.wrap(spsBytes));

            // 跳过NAL头和其他字段
            reader.readBits(4); // nal_unit_header
            reader.readUE();    // sps_video_parameter_set_id
            reader.readBits(3); // sps_max_sub_layers_minus1
            reader.readBit();   // temporal_id_nesting_flag
            HEVCParser.VideoParams videoParams = new HEVCParser.VideoParams();
            parseProfileTierLevel(reader, videoParams); // 跳过profile_tier_level

            int width = reader.readUE();
            int height = reader.readUE();
            return new int[]{width, height};
        } catch (Exception e) {
            Log.e(TAG, "Resolution parsing failed: " + e.getMessage());
            return null;
        }
    }

    // 写入SPS/PPS到文件头部
    private static void writeParameterSets(MediaMuxer muxer, int trackIndex,
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
    private static int findNextStartCode(byte[] data, int start) {
        for (int i = start; i < data.length - 4; i++) {
            if (data[i] == 0x00 && data[i + 1] == 0x00 &&
                    (data[i + 2] == 0x01 || (data[i + 2] == 0x00 && data[i + 3] == 0x01))) {
                return i;
            }
        }
        return -1;
    }

    private static boolean isVideoData(int nalType) {
        return nalType >= 0 && nalType <= 31 &&
                nalType != NAL_UNIT_TYPE_VPS &&
                nalType != NAL_UNIT_TYPE_SPS &&
                nalType != NAL_UNIT_TYPE_PPS;
    }

    private static long calculateFrameDuration(List<NalUnit> nalUnits) {
        // 实际项目应从SPS的VUI解析，此处简化
        return 33_333; // 默认30fps (1,000,000μs / 30 ≈ 33333)
    }

    // NAL单元封装类
    static class NalUnit {
        final int type;
        final byte[] data;

        NalUnit(int type, byte[] data) {
            this.type = type;
            this.data = data;
        }
    }

    // 位流读取工具（需完整实现）
    static class BitStreamReader {
        // 实现参考之前的代码，需包含readBits(), readUE(), readUInt()等方法
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

        // 其他方法保持不变（readBit(), readBits(), readUE()）...

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

    private static void parseProfileTierLevel(BitStreamReader reader, HEVCParser.VideoParams params) {
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
}
