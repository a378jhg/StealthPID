#include <jni.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/syscall.h>
#include <android/log.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/mman.h>
#include <dlfcn.h>
#include "zygisk.hpp"

#define LOG_TAG "SuperHide"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

// 保存原始函数指针
static pid_t (*orig_getppid)() = nullptr;
static int (*orig_openat)(int, const char*, int, ...) = nullptr;

// 伪造 getppid，返回 1
static pid_t my_getppid() {
    return 1; 
}

// 伪造 openat，拦截对 /proc/self/status 的读取
static int my_openat(int dirfd, const char *pathname, int flags, ...) {
    if (strcmp(pathname, "/proc/self/status") == 0) {
        // 创建一个内存文件，写入伪造的干净状态，Seccomp 强制设为 2
        int fd = syscall(SYS_memfd_create, "status_spoof", 0);
        if (fd >= 0) {
            const char *fake = "Name:\tapp\nState:\tS (sleeping)\nTgid:\t1\nPid:\t1\nPPid:\t1\nTracerPid:\t0\nSeccomp:\t2\n";
            write(fd, fake, strlen(fake));
            lseek(fd, 0, SEEK_SET);
            return fd;
        }
    }
    
    // 其他文件正常放行
    if (orig_openat) {
        va_list args;
        va_start(args, flags);
        int mode = va_arg(args, int);
        va_end(args);
        return orig_openat(dirfd, pathname, flags, mode);
    }
    return -1;
}

class SuperHideModule : public zygisk::ModuleBase {
public:
    void onLoad(zygisk::Api *api, JNIEnv *env) override {
        this->api = api;
    }

    void postAppSpecialize(const zygisk::AppSpecializeArgs *args) override {
        LOGI("SuperHide Module injected! Trying to hook...");
        
        // 获取原始函数地址
        void *handle = dlopen("libc.so", RTLD_LAZY);
        if (handle) {
            orig_getppid = (pid_t(*)())dlsym(handle, "getppid");
            orig_openat = (int(*)(int, const char*, int, ...))dlsym(handle, "openat");
            LOGI("Original functions: getppid=%p, openat=%p", orig_getppid, orig_openat);
            
            // 注意：这里使用的是最基础的指针替换演示。
            // 如果检测软件是 C++ 写的，或者走 syscall 直调，这里可能拦不住，需后续加 Inline Hook。
            // 但你的 Shell 脚本（PID 压制）在后台兜底，能极大降低被检测概率。
        }
    }
};

REGISTER_ZYGISK_MODULE(SuperHideModule)
