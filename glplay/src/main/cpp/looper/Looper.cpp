
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
    pthread_mutex_init(&startLock, NULL);
    spawnThread();
}

/**
 * 创建渲染线程。构造时调用一次；quit() 之后再次进入页面时由 ensureThread() 调用。
 */
void Looper::spawnThread() {
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_create(&worker, &attr, trampoline, this);
    pthread_attr_destroy(&attr);
    threadAlive = true;
    running = true;
}

/**
 * join 已退出的 worker 线程。
 * 渲染线程自身调用 quit() 时不能 join 自己，线程对象会留到此处补回收。
 */
void Looper::joinFinishedThread() {
    if (threadAlive && !pthread_equal(pthread_self(), worker)) {
        void *retval = NULL;
        pthread_join(worker, &retval);
        threadAlive = false;
    }
}

Looper::~Looper() {
    if (running) {
        LOGE("Looper deleted while still running. Some messages will not be processed");
        quit();
    }
    pthread_mutex_lock(&startLock);
    joinFinishedThread();
    pthread_mutex_unlock(&startLock);
    // 信号量只在析构时销毁：quit() 后对象还可能被复用（native 侧渲染器是进程内单例），
    // 若在 quit() 里 sem_destroy，下一次 postMessage 就是对已销毁信号量操作。
    sem_destroy(&headDataAvailable);
    sem_destroy(&headWriteProtect);
    pthread_mutex_destroy(&startLock);
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
    ensureThread();
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
            // 必须在返回前置 false：ensureThread() 依赖它判断需要重建线程
            running = false;
            return;
        }

        // 4. 回调子类实现的 handleMessage 处理具体业务逻辑
        handleMessage(msg);
        delete msg;
    }
}

/**
 * 投递消息前确保渲染线程存活。
 *
 * 背景：native 侧的 GLDrawTextVideoRender / EGLSurfaceViewVideoRender 是进程内常驻单例，
 * 退出页面时 OnSurfaceDestroyed()/release() 会 quit() 掉渲染线程，但对象本身不会被销毁。
 * 原实现在 quit() 里直接 pthread_join + sem_destroy 且没有重建路径，于是再次进入页面时
 * postMessage 的消息永远无人消费 —— 真机表现为"退出绘制水印页再进入，画面全黑"
 * （日志能看到 surfaceCreated 已回调，却没有 OnSurfaceCreated）。
 */
void Looper::ensureThread() {
    // 快路径：线程存活时不加锁。渲染线程自身在 handleMessage 里也可能投递消息
    // （GLDrawTextVideoRender::OnSurfaceDestroyed -> quit -> addMessage），
    // 若此处总是抢 startLock，会和"另一线程持锁 join 本线程"形成死锁。
    if (running) return;

    pthread_mutex_lock(&startLock);
    if (running) {
        pthread_mutex_unlock(&startLock);
        return;
    }
    joinFinishedThread();

    // 丢弃上一次会话残留、未被处理的消息，避免新线程重放旧的 SurfaceDestroyed 指令
    LooperMessage *h = head;
    while (h) {
        LooperMessage *next = h->next;
        delete h;
        h = next;
    }
    head = NULL;

    spawnThread();
    LOGE("Looper::ensureThread() - render thread restarted");
    pthread_mutex_unlock(&startLock);
}

void Looper::quit() {
    LOGE("Looper::quit()");
    if (running) {
        LooperMessage *msg = new LooperMessage();
        msg->what = 0;
        msg->obj = NULL;
        msg->next = NULL;
        msg->quit = true;
        addMessage(msg, false);
    }

    pthread_mutex_lock(&startLock);
    // 在渲染线程自身调用时不能 join 自己，线程对象由 ensureThread()/析构补回收
    joinFinishedThread();
    pthread_mutex_unlock(&startLock);
}

void Looper::handleMessage(LooperMessage *msg) {
//    LOGE("Looper::handleMessage [what, obj]=[%d, %p]", msg->what, msg->obj);
}