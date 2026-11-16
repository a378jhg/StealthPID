#include <jni.h>
#include <unistd.h>
#include <fcntl.h>
#include <android/log.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <stdarg.h>
#include "zygisk.hpp"

#define LOG_TAG "SuperHide"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

static pid_t (*orig_getppid)();
static int (*orig_openat)(int, const char*, int, ...);

// 拦截父进程 PID，返回 1
static pid_t my_getppid() {
    return 1;
}

// 拦截文件读取，伪造 /proc/self/status
static int my_openat(int dirfd, const char *pathname, int flags, ...) {
    mode_t mode = 0;
    if (flags & O_CREAT) {
        va_list args;
        va_start(args, flags);
        mode = va_arg(args, mode_t);
        va_end(args);
    }

    if (strcmp(pathname, "/proc/self/status") == 0) {
        int fd = syscall(SYS_memfd_create, "status_spoof", 0);
        if (fd >= 0) {
            const char *fake = "Name:\tapp\nState:\tS (sleeping)\nTgid:\t1\nPid:\t1\nPPid:\t1\nTracerPid:\t0\nSeccomp:\t2\n";
            write(fd, fake, strlen(fake));
            lseek(fd, 0, SEEK_SET);
            return fd;
        }
    }
    return orig_openat(dirfd, pathname, flags, mode);
}

class SuperHideModule : public zygisk::ModuleBase {
public:
    void onLoad(zygisk::Api *api, JNIEnv *env) override {
        this->api = api;
    }

    void postAppSpecialize(const zygisk::AppSpecializeArgs *args) override {
        LOGI("SuperHide Module injected!");
        api->pltHookRegister("libc.so", "getppid", (void*)my_getppid, (void**)&orig_getppid);
        api->pltHookRegister("libc.so", "openat", (void*)my_openat, (void**)&orig_openat);
        if (!api->pltHookCommit()) {
            LOGI("Failed to hook!");
        } else {
            LOGI("Hooked successfully!");
        }
    }
};

REGISTER_ZYGISK_MODULE(SuperHideModule)
