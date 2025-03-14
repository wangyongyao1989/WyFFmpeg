package com.wangyongyao.h265;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

/**
 * author : wangyongyao https://github.com/wangyongyao1989
 * Create Time : 2025/3/13 17:42
 * Descibe : zc380 com.hyterobo.zc400.utils
 */
public class NalUnit {
    public final int type;         // NAL 单元类型（如 VPS=32, SPS=33）
    public final ByteBuffer data;  // NAL 单元负载数据（不含起始码）

    /**
     * 构造方法
     * @param type      NAL 单元类型
     * @param rawData   原始字节数组（整个码流）
     * @param offset    NAL 负载数据的起始位置（跳过起始码后的位置）
     * @param length    NAL 负载数据的长度
     */
    public NalUnit(int type, byte[] rawData, int offset, int length) {
        this.type = type;
        // 将数据封装为只读 ByteBuffer（大端序）
        this.data = ByteBuffer.wrap(rawData, offset, length)
                .asReadOnlyBuffer()
                .order(ByteOrder.BIG_ENDIAN);
    }
}
