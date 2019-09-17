//
// Created by xuejin on 18-1-5.
//
#define LOG_TAG "gesturewake-hal"

#include "hw_gesturewake.h"
#include <errno.h>
#include <string.h>
#include <cutils/log.h>


#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))
int open_dev(){
    __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, "open_dev\n");
    fd = open(NODE,O_RDWR);
    if (fd < 0){
        __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, "Open Dev %s Error", NODE);
        return -1;
    }
    return 0;
}

int close_dev(){
    __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, "close_dev\n");
    if(fd < 0){
        return -1;
    }else{
        close(fd);
    }
    return 0;
}

int start_dev(int arg){
    __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, "start_dev arg = %d\n", arg);
    if (ioctl(fd, GESTUREWAKE_START, &arg) < 0){
        __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, "Call cmd GESTUREWAKE_START fail\n");
        return -1;
    }
    return 0;
}

char* read_dev(){
    __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, "read_dev waiting\n");
    if (ioctl(fd, GESTUREWAKE_READ, data) < 0){
        __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, "Call cmd GESTUREWAKE_READ fail\n");
        return NULL;
    }
    __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, "read_dev data = %s\n", data);
    return data;
}

static int gesturewake_init(struct gesturewake *dev) {
    return open_dev();
}

static int gesturewake_start(struct gesturewake *dev, int flag) {
    return start_dev(flag);
}

static char* gesturewake_read(struct gesturewake *dev) {
    return read_dev();
}

static int gesturewake_close(hw_device_t *dev) {
    close_dev();
    free(dev);
    return 0;
}

/*
 * Generic device handling
 */
static int gesturewake_open(const hw_module_t* module, const char* name,
        hw_device_t** device) {
    if (device == NULL) {
        __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "NULL device on open");
        return -EINVAL;
    }

    __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, "Device open");
    gesturewake_t *dev = malloc(sizeof(gesturewake_t));
    memset(dev, 0, sizeof(gesturewake_t));

    dev->common.tag = HARDWARE_DEVICE_TAG;
    dev->common.version = 0;
    dev->common.module = (struct hw_module_t*) module;
    dev->common.close = gesturewake_close;

    dev->gesturewake_init = gesturewake_init;
    dev->gesturewake_start = gesturewake_start;
    dev->gesturewake_read = gesturewake_read;

    *device = (hw_device_t*) dev;
    return 0;
}


static struct hw_module_methods_t gesturewake_module_methods = {
        .open = gesturewake_open,
};

gesturewake_module_t HAL_MODULE_INFO_SYM = {
        .common = {
                .tag = HARDWARE_MODULE_TAG,
                .module_api_version = GESTUREWAKE_MODULE_API_VERSION_1_0,
                .hal_api_version = HARDWARE_HAL_API_VERSION,
                .id = GESTUREWAKE_HARDWARE_MODULE_ID,
                .name = "gesturewake",
                .author = "The Android Open Source Project",
                .methods = &gesturewake_module_methods,
        },
};