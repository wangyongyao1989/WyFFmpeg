package com.example.myyffmpeg.utils;

import java.nio.ByteBuffer;
import java.nio.charset.StandardCharsets;

public class StringToIntConverter {
    /**
     * z字符串转换Int
     * @param str
     * @return
     * @throws NumberFormatException
     */
    public static int stringToInt(String str) throws NumberFormatException {
        if (str == null || str.isEmpty()) {
            throw new IllegalArgumentException("输入字符串不能为空");
        }

        // 提取字符串中的数字部分
        String numericPart = str.replaceAll("[^\\d]", "");
        if (numericPart.isEmpty()) {
            throw new NumberFormatException("输入字符串不包含有效的数字: " + str);
        }

        return Integer.parseInt(numericPart);
    }

    /**
     * 将 int 转换为 4 字节的 byte 数组
     * @param value
     * @return
     */
    public static byte[] intToByteArray(int value) {
        return ByteBuffer.allocate(4).putInt(value).array();
    }

    /**
     * 将字符串转为 byte[]，并填充到 32 位数组
     * @param str
     * @return
     */
    public static byte[] stringToDeviceNameBytes(String str) {
        byte[] deviceName = new byte[32];

        // 将字符串转换为 byte[]，并使用 UTF-8 编码
        byte[] stringBytes = str.getBytes(StandardCharsets.UTF_8);

        // 将字符串的 byte[] 填充到 deviceName 数组中
        System.arraycopy(stringBytes, 0, deviceName, 0,
                Math.min(stringBytes.length, deviceName.length));

        return deviceName;
    }

    public static String bytes2hex(byte[] bytes) {
        StringBuilder sb = new StringBuilder();
        String tmp;
        sb.append("[");
        for (byte b : bytes) {
            // 将每个字节与0xFF进行与运算，然后转化为10进制，然后借助于Integer再转化为16进制
            tmp = Integer.toHexString(0xFF & b);
            if (tmp.length() == 1) {
                tmp = "0" + tmp;//只有一位的前面补个0
            }
            sb.append(tmp).append(" ");//每个字节用空格断开
        }
        sb.delete(sb.length() - 1, sb.length());//删除最后一个字节后面对于的空格
        sb.append("]");
        return sb.toString();

    }
}
