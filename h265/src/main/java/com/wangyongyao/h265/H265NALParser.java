package com.wangyongyao.h265;

import android.util.Log;

import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.util.BitSet;

/**
 * author : wangyongyao https://github.com/wangyongyao1989
 * Create Time : 2025/3/14 17:22
 * Descibe : MyyFFmpeg com.wangyongyao.h265
 */
public class H265NALParser {
    private static final String TAG = H265NALParser.class.getSimpleName();

    // 解析 VPS
    public static void parseVPS(byte[] buffer) {
        BitstreamReader11 reader = new BitstreamReader11(buffer);
        // 跳过 NAL Header 和 SPS 头部字段 


        // 基本信息
        int vps_video_parameter_set_id = reader.readBits(4);
        int vps_base_layer_internal_flag = reader.readBit() ? 1 : 0;
        int vps_base_layer_available_flag = reader.readBit() ? 1 : 0;
        int vps_max_layers_minus1 = reader.readBits(6);
        int vps_max_sub_layers_minus1 = reader.readBits(3);
        int vps_temporal_id_nesting_flag = reader.readBit() ? 1 : 0;

        Log.e(TAG,"vps_video_parameter_set_id: " + vps_video_parameter_set_id);
        Log.e(TAG,"vps_base_layer_internal_flag: " + vps_base_layer_internal_flag);
        Log.e(TAG,"vps_base_layer_available_flag: " + vps_base_layer_available_flag);
        Log.e(TAG,"vps_max_layers_minus1: " + vps_max_layers_minus1);
        Log.e(TAG,"vps_max_sub_layers_minus1: " + vps_max_sub_layers_minus1);
        Log.e(TAG,"vps_temporal_id_nesting_flag: " + vps_temporal_id_nesting_flag);

        // profile_tier_level 部分
        boolean general_profile_space = reader.readBit();
        int general_tier_flag = reader.readBit() ? 1 : 0;
        int general_profile_idc = reader.readBits(5);
        BitSet general_profile_compatibility_flag = new BitSet(32);
        for (int i = 0; i < 32; i++) {
            general_profile_compatibility_flag.set(i, reader.readBit());
        }
        int general_progressive_source_flag = reader.readBit() ? 1 : 0;
        int general_interlaced_source_flag = reader.readBit() ? 1 : 0;
        int general_non_packed_constraint_flag = reader.readBit() ? 1 : 0;
        int general_frame_only_constraint_flag = reader.readBit() ? 1 : 0;
        // 省略一些标志位的读取，可根据标准继续补充
        int general_level_idc = reader.readBits(8);

        Log.e(TAG,"general_profile_space: " + general_profile_space);
        Log.e(TAG,"general_tier_flag: " + general_tier_flag);
        Log.e(TAG,"general_profile_idc: " + general_profile_idc);
        Log.e(TAG,"general_profile_compatibility_flag: " + general_profile_compatibility_flag);
        Log.e(TAG,"general_progressive_source_flag: " + general_progressive_source_flag);
        Log.e(TAG,"general_interlaced_source_flag: " + general_interlaced_source_flag);
        Log.e(TAG,"general_non_packed_constraint_flag: " + general_non_packed_constraint_flag);
        Log.e(TAG,"general_frame_only_constraint_flag: " + general_frame_only_constraint_flag);
        Log.e(TAG,"general_level_idc: " + general_level_idc);

        // 后续信息
        int vps_sub_layer_ordering_info_present_flag = reader.readBit() ? 1 : 0;
        for (int i = (vps_sub_layer_ordering_info_present_flag != 0)? 0 : vps_max_sub_layers_minus1; i <= vps_max_sub_layers_minus1; i++) {
            int vps_max_dec_pic_buffering_minus1 = reader.readUEG();
            int vps_max_num_reorder_pics = reader.readUEG();
            int vps_max_latency_increase_plus1 = reader.readUEG();
            Log.e(TAG,"For sub - layer " + i + ":");
            Log.e(TAG,"  vps_max_dec_pic_buffering_minus1: " + vps_max_dec_pic_buffering_minus1);
            Log.e(TAG,"  vps_max_num_reorder_pics: " + vps_max_num_reorder_pics);
            Log.e(TAG,"  vps_max_latency_increase_plus1: " + vps_max_latency_increase_plus1);
        }

        int vps_max_layer_id = reader.readBits(6);
        int vps_num_layer_sets_minus1 = reader.readUEG();
        Log.e(TAG,"vps_max_layer_id: " + vps_max_layer_id);
        Log.e(TAG,"vps_num_layer_sets_minus1: " + vps_num_layer_sets_minus1);

        for (int i = 1; i <= vps_num_layer_sets_minus1; i++) {
            for (int j = 0; j <= vps_max_layer_id; j++) {
                int layer_id_included_flag = reader.readBit() ? 1 : 0;
                Log.e(TAG,"For layer set " + i + ", layer " + j + " included flag: " + layer_id_included_flag);
            }
        }

        int vps_timing_info_present_flag = reader.readBit() ? 1 : 0;
        if (vps_timing_info_present_flag != 0) {
            long vps_num_units_in_tick = reader.readBits(32);
            long vps_time_scale = reader.readBits(32);
            int vps_poc_proportional_to_timing_flag = reader.readBit() ? 1 : 0;
            if (vps_poc_proportional_to_timing_flag != 0) {
                int vps_num_ticks_poc_diff_one_minus1 = reader.readUEG();
                Log.e(TAG,"vps_num_ticks_poc_diff_one_minus1: " + vps_num_ticks_poc_diff_one_minus1);
            }
            int vps_num_hrd_parameters = reader.readUEG();
            Log.e(TAG,"vps_num_units_in_tick: " + vps_num_units_in_tick);
            Log.e(TAG,"vps_time_scale: " + vps_time_scale);
            Log.e(TAG,"vps_poc_proportional_to_timing_flag: " + vps_poc_proportional_to_timing_flag);
            Log.e(TAG,"vps_num_hrd_parameters: " + vps_num_hrd_parameters);
            for (int i = 0; i < vps_num_hrd_parameters; i++) {
                int hrd_layer_set_idx = (i == 0)? 0 : reader.readUEG();
                int cprms_present_flag = reader.readBit() ? 1 : 0;
                // 省略 HRD 相关更详细的解析，可根据标准补充
                Log.e(TAG,"For HRD parameter " + i + ":");
                Log.e(TAG,"  hrd_layer_set_idx: " + hrd_layer_set_idx);
                Log.e(TAG,"  cprms_present_flag: " + cprms_present_flag);
            }
        }

        int vps_extension_flag = reader.readBit() ? 1 : 0;
        if (vps_extension_flag != 0) {
            // 处理 VPS 扩展部分，可根据具体扩展内容解析
            Log.e(TAG,"VPS has extension data.");
        }
    }

    // 解析 SPS
    public static void parseSPS(byte[] buffer) {
        BitstreamReader11 reader = new BitstreamReader11(buffer);
        int sps_video_parameter_set_id = reader.readBits(4);
        int sps_max_sub_layers_minus1 = reader.readBits(3);
        int sps_temporal_id_nesting_flag = reader.readBit() ? 1 : 0;
        int profile_tier_level = reader.readBits(24); // 简化处理，实际更复杂
        int sps_seq_parameter_set_id = reader.readUEG();
        int chroma_format_idc = reader.readUEG();
        int separate_colour_plane_flag = (chroma_format_idc == 3) ? reader.readBit() ? 1 : 0 : 0;
        int pic_width_in_luma_samples = reader.readUEG();
        int pic_height_in_luma_samples = reader.readUEG();
        int conformance_window_flag = reader.readBit() ? 1 : 0;
        int conf_win_left_offset = conformance_window_flag == 1 ? reader.readUEG() : 0;
        int conf_win_right_offset = conformance_window_flag == 1 ? reader.readUEG() : 0;
        int conf_win_top_offset = conformance_window_flag == 1 ? reader.readUEG() : 0;
        int conf_win_bottom_offset = conformance_window_flag == 1 ? reader.readUEG() : 0;

         Log.e(TAG, "SPS Attributes:");
         Log.e(TAG, "  sps_video_parameter_set_id: " + sps_video_parameter_set_id);
         Log.e(TAG, "  sps_max_sub_layers_minus1: " + sps_max_sub_layers_minus1);
         Log.e(TAG, "  sps_temporal_id_nesting_flag: " + sps_temporal_id_nesting_flag);
         Log.e(TAG, "  profile_tier_level: " + profile_tier_level);
         Log.e(TAG, "  sps_seq_parameter_set_id: " + sps_seq_parameter_set_id);
         Log.e(TAG, "  chroma_format_idc: " + chroma_format_idc);
         Log.e(TAG, "  separate_colour_plane_flag: " + separate_colour_plane_flag);
         Log.e(TAG, "  pic_width_in_luma_samples: " + pic_width_in_luma_samples);
         Log.e(TAG, "  pic_height_in_luma_samples: " + pic_height_in_luma_samples);
         Log.e(TAG, "  conformance_window_flag: " + conformance_window_flag);
         Log.e(TAG, "  conf_win_left_offset: " + conf_win_left_offset);
         Log.e(TAG, "  conf_win_right_offset: " + conf_win_right_offset);
         Log.e(TAG, "  conf_win_top_offset: " + conf_win_top_offset);
         Log.e(TAG, "  conf_win_bottom_offset: " + conf_win_bottom_offset);
    }

    // 解析 PPS
    public static void parsePPS(byte[] buffer) {
        BitstreamReader11 reader = new BitstreamReader11(buffer);
        int pps_pic_parameter_set_id = reader.readUEG();
        int pps_seq_parameter_set_id = reader.readUEG();
        int dependent_slice_segments_enabled_flag = reader.readBit() ? 1 : 0;
        int output_flag_present_flag = reader.readBit() ? 1 : 0;
        int num_extra_slice_header_bits = reader.readBits(3);
        int sign_data_hiding_enabled_flag = reader.readBit() ? 1 : 0;
        int cabac_init_present_flag = reader.readBit() ? 1 : 0;

         Log.e(TAG, "PPS Attributes:");
         Log.e(TAG, "  pps_pic_parameter_set_id: " + pps_pic_parameter_set_id);
         Log.e(TAG, "  pps_seq_parameter_set_id: " + pps_seq_parameter_set_id);
         Log.e(TAG, "  dependent_slice_segments_enabled_flag: " + dependent_slice_segments_enabled_flag);
         Log.e(TAG, "  output_flag_present_flag: " + output_flag_present_flag);
         Log.e(TAG, "  num_extra_slice_header_bits: " + num_extra_slice_header_bits);
         Log.e(TAG, "  sign_data_hiding_enabled_flag: " + sign_data_hiding_enabled_flag);
         Log.e(TAG, "  cabac_init_present_flag: " + cabac_init_present_flag);
    }

    public static void main(String[] args) {
        String filePath = "path/to/your/h265_stream.h265";
        try (InputStream inputStream = new FileInputStream(filePath)) {
            byte[] buffer = new byte[1024 * 1024]; // 1MB 缓冲区
            int bytesRead = inputStream.read(buffer);
            int start = 0;
            while (start < bytesRead) {
                int nalUnitType = (buffer[start] & 0x7E) >> 1;
                int nextStart = start + 1;
                while (nextStart < bytesRead - 3 &&!(buffer[nextStart] == 0 && buffer[nextStart + 1] == 0 && (buffer[nextStart + 2] == 0 && buffer[nextStart + 3] == 1 || buffer[nextStart + 2] == 1))) {
                    nextStart++;
                }
                byte[] nalUnit = new byte[nextStart - start];
                System.arraycopy(buffer, start, nalUnit, 0, nalUnit.length);
                switch (nalUnitType) {
                    case 32: // VPS
                        parseVPS(nalUnit);
                        break;
                    case 33: // SPS
                        parseSPS(nalUnit);
                        break;
                    case 34: // PPS
                        parsePPS(nalUnit);
                        break;
                }
                start = nextStart;
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}
