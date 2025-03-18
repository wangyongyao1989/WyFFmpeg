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

    private native void n_test_h265(String path);

}
