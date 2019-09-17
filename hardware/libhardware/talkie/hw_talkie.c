/*
 AUTHOR : Lattice Semiconductor co.
 Date : 8-Dec-2014
 Project : hct Voice detection
 */

#define LOG_TAG "talkie-hal"

#include "ioctl_talkie.h"
#include "hw_talkie.h"
#include <errno.h>
#include <string.h>
#include <cutils/log.h>


#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))

static int talkie_init(struct talkie *dev) {

    __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, "talkie_init");
    return open_dev();
}

static int talkie_deinit(struct talkie *dev) {

    __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, "talkie_exit");
    return close_dev();
}

static int talkie_app_start(struct talkie *dev) {

    __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, "talkie_app_start");
    return app_start();
}

static int talkie_app_exit(struct talkie *dev) {

    __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, "talkie_app_exit");
    return app_exit();
}

static int talkie_ppt_down(struct talkie *dev, int arg) {

    __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, "talkie_ppt_down");
    return ppt_down(arg);
}

static int talkie_ppt_up(struct talkie *dev, int arg) {

    __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, "talkie_ppt_up");
    return ppt_up(arg);
}

static int talkie_speak_enable(struct talkie *dev) {

    __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, "talkie_speak_enable");
    return speak_enable();
}

static int talkie_speak_disable(struct talkie *dev) {

    __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, "talkie_speak_disable");
    return speak_disable();
}

static int talkie_close(hw_device_t *dev) {
    free(dev);
    return 0;
}

/*
 * Generic device handling
 */
static int talkie_open(const hw_module_t* module, const char* name,
        hw_device_t** device) {
    if (device == NULL) {
        __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "NULL device on open");
        return -EINVAL;
    }

    __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, "Device open");
    talkie_t *dev = malloc(sizeof(talkie_t));
    memset(dev, 0, sizeof(talkie_t));

    dev->common.tag = HARDWARE_DEVICE_TAG;
    dev->common.version = 0;
    dev->common.module = (struct hw_module_t*) module;
    dev->common.close = talkie_close;
#if 1   
    dev->talkie_init = talkie_init;
    dev->talkie_deinit = talkie_deinit;
    dev->talkie_app_start = talkie_app_start;
    dev->talkie_app_exit = talkie_app_exit;
    dev->talkie_ppt_down = talkie_ppt_down;
    dev->talkie_ppt_up = talkie_ppt_up;
    dev->talkie_speak_enable = talkie_speak_enable;
    dev->talkie_speak_disable = talkie_speak_disable;
#endif
    *device = (hw_device_t*) dev;
    return 0;
}


static struct hw_module_methods_t talkie_module_methods = {
        .open = talkie_open,
};

talkie_module_t HAL_MODULE_INFO_SYM = {
        .common = {
                .tag = HARDWARE_MODULE_TAG,
                .module_api_version = TALKIE_MODULE_API_VERSION_1_1,
                .hal_api_version = HARDWARE_HAL_API_VERSION,
                .id = TALKIE_HARDWARE_MODULE_ID,
                .name = "talkie",
                .author = "The Android Open Source Project",
                .methods = &talkie_module_methods,
        },
};
