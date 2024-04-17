
cmake_minimum_required(VERSION 3.22.1)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11")

project("myyffmpeg")


set(distribution_DIR ${CMAKE_CURRENT_SOURCE_DIR})


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11")

set(jnilibs ${CMAKE_SOURCE_DIR}/jniLibs)

add_library(${CMAKE_PROJECT_NAME}
        SHARED
        native-lib.cpp
        FFmpegManger.cpp
        FFmpegVideoTrans.cpp
        FFmpegWaterMark.cpp
        FFmpegPlayAudio.cpp
        FFmpegWaterMarkTest.cpp
        )

link_directories(
        ${CMAKE_CURRENT_LIST_DIR}/libs/${ANDROID_ABI})

message(${CMAKE_CURRENT_LIST_DIR}/libs/${ANDROID_ABI}===ssss==)
message(${CMAKE_SOURCE_DIR}===ssss==)

target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
        ${distribution_DIR}/include)


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
        ${CMAKE_PROJECT_NAME}
        # List libraries link to the target library
        android
        log
        ${third-party-libs}

)