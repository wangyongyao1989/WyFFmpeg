package com.wangyongyao.h265.nal;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

/**
 * author : wangyongyao https://github.com/wangyongyao1989
 * Create Time : 2025/3/18 9:35
 * Descibe : MyyFFmpeg com.wangyongyao.h265
 */
public class NalUnitParser {
    private static final int NAL_START_CODE_3 = 0x000001;    // 3字节起始码
    private static final int NAL_START_CODE_4 = 0x00000001;  // 4字节起始码

    public static List<NalUnit> parse(byte[] data) {
        List<NalUnit> nalUnits = new ArrayList<>();
        int start = 0;
        while (start < data.length - 4) {
            if (start < 0) break;
            if (isStartCode(data, start)) {
                int prefixLength = getPrefixLength(data, start);
                int nalStart = start + prefixLength;
                int nalType = (data[nalStart] >> 1) & 0x3F;

                int nextStart = findNextStartCode(data, nalStart);
                int length = (nextStart == -1) ? data.length - nalStart : nextStart - nalStart;

                byte[] nalData = Arrays.copyOfRange(data, nalStart, nalStart + length);
                nalUnits.add(new NalUnit(nalType, nalData));

                start = nextStart;
            } else {
                start++;
            }
        }
        return nalUnits;
    }

    private static boolean isStartCode(byte[] data, int pos) {
        return (data[pos] == 0x00 && data[pos + 1] == 0x00 && data[pos + 2] == 0x01) ||
                (data[pos] == 0x00 && data[pos + 1] == 0x00 && data[pos + 2] == 0x00 && data[pos + 3] == 0x01);
    }

    private static int getPrefixLength(byte[] data, int pos) {
        return (data[pos + 2] == 0x01) ? 3 : 4;
    }

    private static int findNextStartCode(byte[] data, int start) {
        for (int i = start; i < data.length - 4; i++) {
            if (isStartCode(data, i)) {
                return i;
            }
        }
        return -1;
    }

    public static class NalUnit {
        public final int type;
        public final byte[] data;

        public NalUnit(int type, byte[] data) {
            this.type = type;
            this.data = data;
        }
    }
}
