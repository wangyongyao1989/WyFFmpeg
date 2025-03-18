package com.wangyongyao.h265.nal;

import java.io.BufferedOutputStream;
import java.io.FileOutputStream;
import java.io.IOException;

/**
 * Create Time : 2025/3/12 15:09
 * Descibe : zc380 com.hyterobo.zc400.utils
 */
public class H265StreamSaver {
    // 默认使用4字节起始码：0x00000001
    private static final byte[] DEFAULT_START_CODE = new byte[]{0x00, 0x00, 0x00, 0x01};

    private BufferedOutputStream outputStream;
    private final boolean autoAddStartCode;
    private boolean isClosed = false;

    /**
     * 构造函数
     *
     * @param filePath         输出文件路径
     * @param autoAddStartCode 是否自动添加起始码
     * @throws IOException 当文件创建失败时抛出
     */
    public H265StreamSaver(String filePath, boolean autoAddStartCode) throws IOException {
        this.autoAddStartCode = autoAddStartCode;
        this.outputStream = new BufferedOutputStream(new FileOutputStream(filePath));
    }

    /**
     * 写入单个NALU单元
     *
     * @param naluData H.265 NALU字节数据（不包含起始码）
     * @throws IOException 写入失败时抛出
     */
    public synchronized void writeNalu(byte[] naluData) throws IOException {
        checkState();

        if (autoAddStartCode) {
            outputStream.write(DEFAULT_START_CODE);
        }
        outputStream.write(naluData);
    }

    /**
     * 写入原始数据（包含起始码的完整数据包）
     *
     * @param rawData 原始字节数据
     * @throws IOException 写入失败时抛出
     */
    public synchronized void writeRawData(byte[] rawData) throws IOException {
        checkState();
        outputStream.write(rawData);
    }

    private void checkState() {
        if (isClosed) {
            throw new IllegalStateException("Writer has been closed");
        }
    }

    /**
     * 关闭写入器并释放资源
     */
    public synchronized void close() {
        if (!isClosed) {
            try {
                outputStream.flush();
                outputStream.close();
            } catch (IOException e) {
                System.err.println("Error closing stream: " + e.getMessage());
            }
            isClosed = true;
        }
    }

    @Override
    protected void finalize() throws Throwable {
        close();
        super.finalize();
    }
}
