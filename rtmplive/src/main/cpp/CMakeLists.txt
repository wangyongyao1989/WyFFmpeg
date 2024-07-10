cmake_minimum_required(VERSION 3.22.1)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11")

project("myyffmpeg")


set(distribution_DIR ${CMAKE_CURRENT_SOURCE_DIR})


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11")


set(jnilibs ${CMAKE_SOURCE_DIR}/jniLibs)

add_library(
        rtmplive
        SHARED
        rtmplivelib.cpp
        RtmpPusherManger.cpp
        RtmpInit.cpp
)

link_directories(
        ${CMAKE_CURRENT_LIST_DIR}/libs/${ANDROID_ABI})

message(${CMAKE_CURRENT_LIST_DIR}/libs/${ANDROID_ABI}===ssss==)

#添加rtmp文件到项目中
add_subdirectory(${CMAKE_SOURCE_DIR}/rtmp)

include_directories(${CMAKE_SOURCE_DIR}/rtmp)

target_include_directories(
        rtmplive
        PRIVATE
        ${distribution_DIR}/include)


set(third-party-libs
        #        ${jnilibs}/${ANDROID_ABI}/libgperf.so

)


target_link_libraries(
        rtmplive
        # List libraries link to the target library
        android
        EGL
        GLESv3
        rtmp
        OpenSLES
        log
        ${third-party-libs}

)