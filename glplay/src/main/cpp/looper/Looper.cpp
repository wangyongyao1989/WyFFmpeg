
#include "../includeopengl/Looper.h"
#include <jni.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <semaphore.h>


struct LooperMessage;
typedef struct LooperMessage LooperMessage;

void *Looper::trampoline(void *p) {
    ((Looper *) p)->loop();
    return NULL;
}

Looper::Looper() {
    head = NULL;

    sem_init(&headDataAvailable, 0, 0);
    sem_init(&headWriteProtect, 0, 1);
    pthread_attr_t attr;
    pthread_attr_init(&attr);

    pthread_create(&worker, &attr, trampoline, this);
    running = true;
}

Looper::~Looper() {
    if (running) {
        LOGE("Looper deleted while still running. Some messages will not be processed");
        quit();
    }
}

void Looper::postMessage(int what, bool flush) {
    postMessage(what, 0, 0, NULL, flush);
}

void Looper::postMessage(int what, void *obj, bool flush) {
    postMessage(what, 0, 0, obj, flush);
}

void Looper::postMessage(int what, int arg1, int arg2, bool flush) {
    postMessage(what, arg1, arg2, NULL, flush);
}

void Looper::postMessage(int what, int arg1, int arg2, void *obj, bool flush) {
    LooperMessage *msg = new LooperMessage();
    msg->what = what;
    msg->obj = obj;
    msg->arg1 = arg1;
    msg->arg2 = arg2;
    msg->next = NULL;
    msg->quit = false;
    addMessage(msg, flush);
}

void Looper::addMessage(LooperMessage *msg, bool flush) {
    sem_wait(&headWriteProtect);
    LooperMessage *h = head;

    if (flush) {
        while (h) {
            LooperMessage *next = h->next;
            delete h;
            h = next;
        }
        h = NULL;
    }
    if (h) {
        while (h->next) {
            h = h->next;
        }
        h->next = msg;
    } else {
        head = msg;
    }
//    LOGE("Looper::addMessage msg->what=%d", msg->what);
    sem_post(&headWriteProtect);
    sem_post(&headDataAvailable);
}

/**
 * Looper 线程主循环
 */
void Looper::loop() {
    LOGD("Looper::loop() - Starting message loop on thread: %ld", pthread_self());
    while (true) {
        // 1. 等待消息到达
        sem_wait(&headDataAvailable);

        // 2. 线程安全地从队列头部取出消息
        sem_wait(&headWriteProtect);
        LooperMessage *msg = head;
        if (msg == NULL) {
            LOGW("Looper::loop() - Spurious wakeup, no message found");
            sem_post(&headWriteProtect);
            continue;
        }
        head = msg->next;
        sem_post(&headWriteProtect);

        // 3. 检查是否是退出指令
        if (msg->quit) {
            LOGI("Looper::loop() - Received quit message, exiting loop");
            delete msg;
            return;
        }

        // 4. 回调子类实现的 handleMessage 处理具体业务逻辑
        handleMessage(msg);
        delete msg;
    }
}

void Looper::quit() {
    LOGE("Looper::quit()");
    if (!running) return;

    LooperMessage *msg = new LooperMessage();
    msg->what = 0;
    msg->obj = NULL;
    msg->next = NULL;
    msg->quit = true;
    addMessage(msg, false);

    if (pthread_equal(pthread_self(), worker)) {
        return;
    }

    void *retval;
    pthread_join(worker, &retval);
    sem_destroy(&headDataAvailable);
    sem_destroy(&headWriteProtect);
    running = false;
}

void Looper::handleMessage(LooperMessage *msg) {
//    LOGE("Looper::handleMessage [what, obj]=[%d, %p]", msg->what, msg->obj);
}