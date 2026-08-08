# WyFFmpeg 项目操作流程与目的概览

本项目是一个基于 FFmpeg 和 OpenGL ES 的 Android 视频处理与播放框架。以下是核心操作流程及其实现目的的总结。

## 1. 初始化阶段 (Initialization)
*   **操作**: Java 层调用 `glSurfaceViewNewInit` -> Native 层创建 `EGLSurfaceViewVideoRender` 实例。
*   **目的**: 准备渲染环境，加载指定的顶点着色器 (Vertex Shader) 和片元着色器 (Fragment Shader)。

## 2. 渲染表面生命周期 (Surface Lifecycle)
*   **操作**: `surfaceCreated` -> `surfaceChanged` -> `surfaceDestroyed`。
*   **目的**: 
    *   **Created**: 初始化 EGL 环境 (`EglCore`, `WindowSurface`)，建立与 Android 原生窗口的链接。
    *   **Changed**: 适配显示区域，根据视频比例 (Aspect Ratio) 计算顶点坐标，防止画面拉伸。
    *   **Destroyed**: 安全释放 OpenGL 资源、关闭渲染线程，防止内存泄漏和非法上下文调用 (SIGSEGV)。

## 3. 视频数据流处理 (Data Flow)
*   **操作**: 相机回调 `onPreviewFrame` (YUV 数据) -> `glSurfaceViewNewDraw` -> `updateFrame`。
*   **目的**: 
    *   **数据传递**: 将原始 YUV 字节数组从 Java 拷贝到 Native 内存。
    *   **步长适配 (Stride Adjustment)**: 处理不同设备相机的对齐宽度，确保 Y、U、V 分量正确对齐以供 OpenGL 使用。

## 4. 核心渲染循环 (Rendering Loop)
*   **操作**: Looper 线程接收 `MSG_DrawFrame` -> `OnDrawFrame`。
*   **目的**:
    *   **纹理上传**: 使用 `glTexImage2D` 将 YUV 数据上传到 GPU 纹理 (`m_textureIdY/U/V`)。
    *   **着色器处理**: 在 GPU 中进行颜色空间转换 (YUV to RGB) 及滤镜处理。
    *   **双缓冲交换**: 通过 `eglSwapBuffers` 将渲染结果显示到屏幕。

## 5. 视频录制流程 (Recording)
*   **操作**: `startEncoder` -> `OnDrawFrame` 中执行 `glBlitFramebuffer`。
*   **目的**: 
    *   **离屏录制**: 在渲染屏幕的同时，将纹理数据通过 `WindowSurface` 传递给 `MediaCodec` 输入端。
    *   **编码保存**: 实现视频预览与 MP4 录制同步进行。

## 6. 线程同步与安全 (Thread Safety)
*   **操作**: 使用自定义 `Looper` 类及 `m_isReleasing` 标志位。
*   **目的**: 确保所有 OpenGL API 调用都在同一个 GL 线程中执行，并处理销毁过程中的竞态条件，保证程序稳定性。

---
*Created by Assistant to help developers understand the project flow.*
