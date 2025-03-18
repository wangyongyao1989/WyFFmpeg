cmake_minimum_required(VERSION 3.22.1)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11")

project("h265lib")


set(distribution_DIR ${CMAKE_CURRENT_SOURCE_DIR})


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++14")

set(jnilibs ${CMAKE_SOURCE_DIR}/jniLibs)

include_directories(
        h265include/webrtc
        h265include/h265nal

)

add_library(
        h265lib
        SHARED

        H265JniCall.cpp

        h265/h265_aud_parser.cc
        h265/h265_bitstream_parser.cc
        h265/h265_bitstream_parser_state.cc
        h265/h265_common.cc
        h265/h265_hrd_parameters_parser.cc
        h265/h265_nal_unit_header_parser.cc
        h265/h265_nal_unit_parser.cc
        h265/h265_nal_unit_payload_parser.cc
        h265/h265_pps_parser.cc
        h265/h265_pps_scc_extension_parser.cc
        h265/h265_pred_weight_table_parser.cc
        h265/h265_profile_tier_level_parser.cc
        h265/h265_rtp_ap_parser.cc
        h265/h265_rtp_fu_parser.cc
        h265/h265_rtp_parser.cc
        h265/h265_rtp_single_parser.cc
        h265/h265_scaling_list_data_parser.cc
        h265/h265_slice_parser.cc
        h265/h265_sps_3d_extension_parser.cc
        h265/h265_sps_multilayer_extension_parser.cc
        h265/h265_sps_parser.cc
        h265/h265_sps_range_extension_parser.cc
        h265/h265_sps_scc_extension_parser.cc
        h265/h265_st_ref_pic_set_parser.cc
        h265/h265_sub_layer_hrd_parameters_parser.cc
        h265/h265_utils.cc
        h265/h265_vps_parser.cc
        h265/h265_vui_parameters_parser.cc

        h265/bit_buffer.cc

        h265nal.cc


)

link_directories(
        ${CMAKE_CURRENT_LIST_DIR}/libs/${ANDROID_ABI})

message(${CMAKE_CURRENT_LIST_DIR}/libs/${ANDROID_ABI}===ssss==)


target_include_directories(
        h265lib
        PRIVATE
        ${distribution_DIR}/h265include)


set(third-party-libs
        #        ${jnilibs}/${ANDROID_ABI}/libgperf.so

)


target_link_libraries(
        h265lib
        # List libraries link to the target library
        android
        EGL
        GLESv3
        OpenSLES
        vulkan
        mediandk
        log
        ${third-party-libs}

)