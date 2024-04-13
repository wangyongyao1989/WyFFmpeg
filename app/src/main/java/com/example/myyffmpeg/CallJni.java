package com.example.myyffmpeg;

public class CallJni {

    static {
        System.loadLibrary("myyffmpeg");
    }

    public String callStringForJNI() {
        return stringFromJNI();
    }

    public String callIntForJNI() {
        return intFromJNI(55) +"岁";
    }

    private native String stringFromJNI();
    private native int intFromJNI(int age);
}
