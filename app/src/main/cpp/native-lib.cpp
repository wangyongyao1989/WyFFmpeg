#include <jni.h>
#include <string>
#include "getopt.h"
#include "gperf.h"

extern "C" JNIEXPORT jstring JNICALL
Java_com_example_myyffmpeg_MainActivity_stringFromJNI(
        JNIEnv* env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    uint64_t i = GetTicks();
    return env->NewStringUTF(hello.c_str());
}