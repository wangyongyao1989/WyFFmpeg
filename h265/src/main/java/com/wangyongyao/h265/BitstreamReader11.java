package com.wangyongyao.h265;

/**
 * author : wangyongyao https://github.com/wangyongyao1989
 * Create Time : 2025/3/14 17:22
 * Descibe : MyyFFmpeg com.wangyongyao.h265
 */
public class BitstreamReader11 {
    private byte[] buffer;
    private int byteIndex;
    private int bitIndex;

    public BitstreamReader11(byte[] buffer) {
        this.buffer = buffer;
        this.byteIndex = 0;
        this.bitIndex = 0;
    }

    public boolean readBit() {
        boolean bit = ((buffer[byteIndex] >> (7 - bitIndex)) & 1) == 1;
        bitIndex++;
        if (bitIndex == 8) {
            bitIndex = 0;
            byteIndex++;
        }
        return bit;
    }

    public int readBits(int numBits) {
        int value = 0;
        for (int i = 0; i < numBits; i++) {
            value = (value << 1) | (readBit() ? 1 : 0);
        }
        return value;
    }

    public int readUEG() {
        int leadingZeros = 0;
        while (!readBit()) {
            leadingZeros++;
        }
        return (1 << leadingZeros) - 1 + readBits(leadingZeros);
    }

    public int readSEG() {
        int ueg = readUEG();
        return (ueg % 2 == 0) ? -(ueg / 2) : (ueg + 1) / 2;
    }
}
