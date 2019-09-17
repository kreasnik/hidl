#include "ioctl_wifi_probe.h"

static int wifi_probe_open(struct wifi_probe *dev) {

    ALOGD("wifi_probe_open");
    return open_dev();
}

static int wifi_probe_close(struct wifi_probe *dev) {

    ALOGD("wifi_probe_close");
    return close_dev();
}

static int wifi_probe_start_search(struct wifi_probe *dev, message_t *msg) {

    ALOGD("wifi_probe_start_search");
    return start_search(msg);
}

static int wifi_probe_stop_search(struct wifi_probe *dev, message_t *msg) {

    ALOGD("wifi_probe_stop_search");
    return stop_search(msg);
}

static int wifi_probe_get_version(struct wifi_probe *dev, message_t *msg) {

    ALOGD("wifi_probe_get_version");
    return get_version(msg);
}

static int wifi_probe_start_firmware_upgrade(struct wifi_probe *dev, message_t *msg) {

    ALOGD("wifi_probe_start_firmware_upgrade");
    return start_firmware_upgrade(msg);
}

static int wifi_probe_set_rate(struct wifi_probe *dev, message_t *msg) {

    ALOGD("wifi_probe_set_rate");
    return set_rate(msg);
}

static int wifi_probe_get_data(struct wifi_probe *dev, message_t *msg) {

    ALOGD("wifi_probe_get_data");
    return get_data(msg);
}

static int wifi_probe_get_eint(struct wifi_probe *dev, int eint) {

    ALOGD("wifi_probe_get_eint");
    return get_eint(eint);
}

static int wifi_probe_set_firmware_size(struct wifi_probe *dev, message_t *msg) {

    ALOGD("wifi_probe_set_firmware_size");
    return set_firmware_size(msg);
}

static int wifi_probe_set_firmware_done(struct wifi_probe *dev, message_t *msg) {

    ALOGD("wifi_probe_set_firmware_done");
    return set_firmware_done(msg);
}

static int wifi_close(hw_device_t *dev) {
    free(dev);
    return 0;
}

/*
 * Generic device handling
 */
static int wifi_open(const hw_module_t* module, const char* name,
        hw_device_t** device) {
    if (device == NULL) {
        return -EINVAL;
    }

    wifi_probe_t *dev = malloc(sizeof(wifi_probe_t));
    memset(dev, 0, sizeof(wifi_probe_t));

    dev->common.tag = HARDWARE_DEVICE_TAG;
    dev->common.version = 0;
    dev->common.module = (struct hw_module_t*) module;
    dev->common.close = wifi_close;
#if 1   
    dev->wifi_probe_open = wifi_probe_open;
    dev->wifi_probe_close = wifi_probe_close;
    dev->wifi_probe_start_search = wifi_probe_start_search;
    dev->wifi_probe_stop_search = wifi_probe_stop_search;
    dev->wifi_probe_get_version = wifi_probe_get_version;
    dev->wifi_probe_start_firmware_upgrade = wifi_probe_start_firmware_upgrade;
    dev->wifi_probe_set_rate = wifi_probe_set_rate;
    dev->wifi_probe_get_data = wifi_probe_get_data;
    dev->wifi_probe_get_eint = wifi_probe_get_eint;
    dev->wifi_probe_set_firmware_size = wifi_probe_set_firmware_size;
    dev->wifi_probe_set_firmware_done = wifi_probe_set_firmware_done;
#endif
    *device = (hw_device_t*) dev;
    return 0;
}


static struct hw_module_methods_t wifi_module_methods = {
        .open = wifi_open,
};

wifi_probe_module_t HAL_MODULE_INFO_SYM = {
        .common = {
                .tag = HARDWARE_MODULE_TAG,
                .module_api_version = WIFI_PROBE_MODULE_API_VERSION_1_1,
                .hal_api_version = HARDWARE_HAL_API_VERSION,
                .id = WIFI_PROBE_HARDWARE_MODULE_ID,
                .name = "wifi_probe",
                .author = "The Android Open Source Project",
                .methods = &wifi_module_methods,
        },
};
