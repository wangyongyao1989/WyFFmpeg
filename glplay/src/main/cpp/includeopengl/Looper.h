//  Author : wangyongyao https://github.com/wangyongyao1989
// Created by MMM on 2024/10/14.
//

#ifndef MYYFFMPEG_LOOPER_H
#define MYYFFMPEG_LOOPER_H

#include <pthread.h>
#include <sys/types.h>
#include <semaphore.h>
#include "LogUtils.h"

struct LooperMessage {
    int what;
    int arg1;
    int arg2;
    void *obj;
    LooperMessage *next;
    bool quit;
};


class Looper {

public:
    Looper();

    Looper &operator=(const Looper &) = delete;

    Looper(Looper &) = delete;

    virtual ~Looper();

    void postMessage(int what, bool flush = false);
    void postMessage(int what, void *obj, bool flush = false);
    void postMessage(int what, int arg1, int arg2, bool flush = false);
    void postMessage(int what, int arg1, int arg2, void *obj, bool flush = false);

    void quit();

    virtual void handleMessage(LooperMessage *msg);

private:
    void addMessage(LooperMessage *msg, bool flush);

    /**
     * 投递消息前确保渲染线程存活：
     * 单例渲染器退出页面时会 quit() 掉线程但对象常驻进程，再次进入需要重新拉起线程。
     */
    void ensureThread();

    /** 回收已退出但尚未 join 的线程，避免重复 pthread_create 泄漏线程对象 */
    void joinFinishedThread();

    /** 创建渲染线程并置 running/threadAlive */
    void spawnThread();

    static void *trampoline(void *p);

    void loop(void);

    LooperMessage *head;
    pthread_t worker;
    sem_t headWriteProtect;
    sem_t headDataAvailable;
    /** 保护 ensureThread/spawnThread，避免两个投递线程同时重建线程 */
    pthread_mutex_t startLock;
    /** 跨线程读写（loop 线程退出时置 false），用 volatile 防止被寄存器缓存 */
    volatile bool running;
    /** worker 线程是否尚未 join（仅在 startLock 保护下读写） */
    bool threadAlive;

};

#endif //MYYFFMPEG_LOOPER_H
