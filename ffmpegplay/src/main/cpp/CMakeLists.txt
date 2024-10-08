
cmake_minimum_required(VERSION 3.22.1)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11")

project("ffmpegplay")


set(distribution_DIR ${CMAKE_CURRENT_SOURCE_DIR})


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

set(jnilibs ${CMAKE_SOURCE_DIR}/jniLibs)

add_library(
        ffmpegplay
        SHARED
        FFPlayJniCalll.cpp
        FFmpegManger.cpp
        FFmpegVideoTrans.cpp
        FFmpegWaterMark.cpp
        FFmpegPlayAudio.cpp
        FFmpegWaterMarkTest.cpp
        FFmpegVideoPlay.cpp


        ffmpegsrc/api-band-test.c
        ffmpegsrc/api-threadmessage-test.c
        ffmpegsrc/api-seek-test.c
        ffmpegsrc/api-flac-test.c
        ffmpegsrc/api-h264-slice-test.c
        ffmpegsrc/api-h264-test.c
        )

link_directories(
        ${CMAKE_CURRENT_LIST_DIR}/libs/${ANDROID_ABI})

message(${CMAKE_CURRENT_LIST_DIR}/libs/${ANDROID_ABI}===ssss==)
message(${CMAKE_SOURCE_DIR}===ssss==)

target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
        ${distribution_DIR}/include
        ${distribution_DIR}/includeopengl)


set(third-party-libs
        ${jnilibs}/${ANDROID_ABI}/libgperf.so
        ${jnilibs}/${ANDROID_ABI}/libavcodec.so
        ${jnilibs}/${ANDROID_ABI}/libavdevice.so
        ${jnilibs}/${ANDROID_ABI}/libavformat.so
        ${jnilibs}/${ANDROID_ABI}/libavutil.so
        ${jnilibs}/${ANDROID_ABI}/libswresample.so
        ${jnilibs}/${ANDROID_ABI}/libswscale.so
        ${jnilibs}/${ANDROID_ABI}/libavfilter.so
        )


target_link_libraries(
        ffmpegplay
        # List libraries link to the target library
        android
        EGL
        GLESv3
        OpenSLES
        log
        ${third-party-libs}

)