//
// Created by xuejin on 18-1-5.
//

#ifndef ANDROID_HW_GESTUREWAKE_H
#define ANDROID_HW_GESTUREWAKE_H
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/cdefs.h>
#include <stdint.h>
#include <android/log.h>
#include <hardware/hardware.h>
#include <hardware/hwcomposer_defs.h>

#define NODE "/dev/touch"
#define GESTUREWAKE_MODULE_API_VERSION_1_0 HARDWARE_MODULE_API_VERSION(1, 0)
#define GESTUREWAKE_HARDWARE_MODULE_ID "gesturewake"

#define IOC_MAGIC           'A'

#define GESTUREWAKE_START    _IOW(IOC_MAGIC, 3, int)
#define GESTUREWAKE_READ     _IOR(IOC_MAGIC, 4, char*)

static int fd = 0;
static char data[10] = {0};

typedef struct gesturewake_module {
    struct hw_module_t common;
} gesturewake_module_t;

typedef struct gesturewake {
    struct hw_device_t common;
    int (*gesturewake_init)(struct gesturewake *dev);
    int (*gesturewake_start)(struct gesturewake *dev, int flag);
    char* (*gesturewake_read)(struct gesturewake *dev);
    void* reserved[8 - 6];
} gesturewake_t;

#endif //ANDROID_HW_GESTUREWAKE_H
