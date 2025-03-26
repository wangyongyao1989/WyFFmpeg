package com.wangyongyao.h265;

/**
 * author : wangyongyao https://github.com/wangyongyao1989
 * Create Time : 2025/3/18 15:28
 * Descibe : MyyFFmpeg com.wangyongyao.h265
 */
public class H265CallJni {

    static {
        System.loadLibrary("h265lib");
    }


    public void testH265(String path) {
        n_test_h265(path);
    }

    public void Hevc2MP4(String inPath, String outPath) {
        n_hevc_to_mp4(inPath, outPath);
    }

    private native void n_test_h265(String path);

    private native void n_hevc_to_mp4(String inPath, String outPath);

}
