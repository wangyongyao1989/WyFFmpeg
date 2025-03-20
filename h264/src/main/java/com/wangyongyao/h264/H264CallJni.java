package com.wangyongyao.h264;

/**
 * author : wangyongyao https://github.com/wangyongyao1989
 * Create Time : 2025/3/20 15:53
 * Descibe : MyyFFmpeg com.wangyongyao.h264
 */
public class H264CallJni {
    static {
        System.loadLibrary("h264lib");
    }


    public void testH264(String path) {
        n_test_h264(path);
    }

    private native void n_test_h264(String path);
}
