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

class SuperHideModule : public zygisk::ModuleBase {
private:
    zygisk::Api *api;
public:
    void onLoad(zygisk::Api *api, JNIEnv *env) override {
        this->api = api;
    }

    void postAppSpecialize(const zygisk::AppSpecializeArgs *args) override {
        // 只要能在 logcat 看到这句话，说明 C++ 底层注入成功！
        LOGI("SuperHide Module injected successfully!");
    }
};

REGISTER_ZYGISK_MODULE(SuperHideModule)
