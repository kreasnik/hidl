#ifndef IOCTL_WIFI_PROBE_H
#define IOCTL_WIFI_PROBE_H
#include <cutils/log.h>
#include <utils/Log.h>
#include "../../include/hardware/wifi_probe.h"

#define NODE "dev/esp_8266"
#define LOG_TAG "wifi_probe"

static int fd = 0;

extern int open_dev();
extern int close_dev();
extern int start_search(message_t *msg);
extern int stop_search(message_t *msg);
extern int get_version(message_t *msg);
extern int start_firmware_upgrade(message_t *msg);
extern int set_rate(message_t *msg);
extern int get_data(message_t *msg);
extern int get_eint(int eint);
extern int set_firmware_size(message_t *msg);
extern int set_firmware_done(message_t *msg);
#endif