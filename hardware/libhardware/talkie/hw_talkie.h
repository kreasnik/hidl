
#ifndef HW_TALKIE_H
#define HW_TALKIE_H

#include <stdint.h>
#include <sys/cdefs.h>
#include <hardware/hardware.h>
#include <hardware/hwcomposer_defs.h>

#define TALKIE_MODULE_API_VERSION_1_1 HARDWARE_MODULE_API_VERSION(1, 1)
#define TALKIE_HARDWARE_MODULE_ID "talkie"

typedef struct talkie_module {
    struct hw_module_t common;
} talkie_module_t;

typedef struct talkie {
    struct hw_device_t common;
    int (*talkie_init)(struct talkie *dev);
    int (*talkie_deinit)(struct talkie *dev);
    int (*talkie_app_start)(struct talkie *dev);
    int (*talkie_app_exit)(struct talkie *dev);
    // ppt down
    int (*talkie_ppt_down)(struct talkie *dev, int arg);
    // ppt up
    int (*talkie_ppt_up)(struct talkie *dev, int arg);
    // 1 - press
    // 0 - up
    // default 0
    int (*talkie_speak_enable)(struct talkie *dev);
    // not used
    int (*talkie_speak_disable)(struct talkie *dev);
    void* reserved[8 - 6];
} talkie_t;

#endif /* HW_TALKIE_H */
