package com.wangyongyao.h265.nal;

import android.os.Environment;

import java.io.FileOutputStream;
import java.io.IOException;

/**
 * Create Time : 2025/3/12 15:14
 * Descibe : zc380 com.hyterobo.zc400.utils
 */
public class H265FileUtils {

    public static void writeBytes(byte[] array) {
        FileOutputStream writer = null;
        try {
            // 打开一个写文件器，构造函数中的第二个参数true表示以追加形式写文件
            writer = new FileOutputStream(Environment.getExternalStorageDirectory() + "/codec1.h265", true);
            writer.write(array);
            writer.write('\n');

        } catch (IOException e) {
            e.printStackTrace();
        } finally {
            try {
                if (writer != null) {
                    writer.close();
                }
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
    }
}
