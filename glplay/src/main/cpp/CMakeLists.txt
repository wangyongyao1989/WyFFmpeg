cmake_minimum_required(VERSION 3.22.1)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11")

project("glplay")


set(distribution_DIR ${CMAKE_CURRENT_SOURCE_DIR})


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++14")


add_library(
        glplay
        SHARED
        #OpenGL相关
        GLPlayJniCall.cpp


        opengl/OpenGLShader.cpp
        opengl/OpenGLCamera3D.cpp
        opengl/OpenglesFlashLight.cpp
        opengl/OpenGLStdImage.cpp
        opengl/OpenglesCameraPre.cpp
        opengl/GLCommonUtils.cpp
        opengl/OpenglesTexureVideoRender.cpp
        opengl/OpenglesTextureFilterRender.cpp
        opengl/OpenglesSurfaceViewVideoRender.cpp
        opengl/EGLSurfaceViewVideoRender.cpp

        WyyEGL/EglCore.cpp
        WyyEGL/EglSurfaceBase.cpp
        WyyEGL/WindowSurface.cpp

        looper/Looper.cpp


)

link_directories(
        ${CMAKE_CURRENT_LIST_DIR}/libs/${ANDROID_ABI})

message(${CMAKE_CURRENT_LIST_DIR}/libs/${ANDROID_ABI}===ssss==)


target_include_directories(
        glplay
        PRIVATE
        ${distribution_DIR}/includeopengl)


set(third-party-libs
        #        ${jnilibs}/${ANDROID_ABI}/libgperf.so

)


target_link_libraries(
        glplay
        # List libraries link to the target library
        android
        EGL
        GLESv3
        OpenSLES
        vulkan
        log
        ${third-party-libs}

)