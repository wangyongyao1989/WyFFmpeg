cmake_minimum_required(VERSION 3.22.1)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11")

project("h264lib")


set(distribution_DIR ${CMAKE_CURRENT_SOURCE_DIR})


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++14")

set(jnilibs ${CMAKE_SOURCE_DIR}/jniLibs)

include_directories(
#        h264include/webrtc

)

add_library(
        h264lib
        SHARED

        H264JniCall.cpp


)

link_directories(
        ${CMAKE_CURRENT_LIST_DIR}/libs/${ANDROID_ABI})

message(${CMAKE_CURRENT_LIST_DIR}/libs/${ANDROID_ABI}===ssss==)


target_include_directories(
        h264lib
        PRIVATE
        ${distribution_DIR}/h265include)


set(third-party-libs
        #        ${jnilibs}/${ANDROID_ABI}/libgperf.so

)


target_link_libraries(
        h264lib
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