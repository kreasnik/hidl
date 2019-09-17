#ifndef VENDOR_HCT_HARDWARE_WIFI_PROBE_V1_0_PROBE_H
#define VENDOR_HCT_HARDWARE_WIFI_PROBE_V1_0_PROBE_H

#include <vendor/hct/hardware/wifi_probe/1.0/IWifiProbePort.h>
#include <vendor/hct/hardware/wifi_probe/1.0/IWifiProbeCallback.h>
#include <hidl/MQDescriptor.h>
#include <hidl/Status.h>

#include <cutils/log.h>
#include <pthread.h>
#include <unistd.h>
#include <utils/Log.h>
#include "../../../../include/hardware/wifi_probe.h"

#define LOG_TAG "wifi_probe"
static const char *TAG="wifi_probe";

namespace vendor {
namespace hct {
namespace hardware {
namespace wifi_probe {
namespace V1_0 {
namespace implementation {

using ::vendor::hct::hardware::wifi_probe::V1_0::IWifiProbePort;
using ::android::hidl::base::V1_0::DebugInfo;
using ::android::hidl::base::V1_0::IBase;
using ::android::hardware::hidl_array;
using ::android::hardware::hidl_memory;
using ::android::hardware::hidl_string;
using ::android::hardware::hidl_vec;
using ::android::hardware::Return;
using ::android::hardware::Void;
using ::android::sp;

struct WifiProbePort : public IWifiProbePort {
public:
    WifiProbePort();
    ~WifiProbePort();
    hw_device_t *mDevice;
    static IWifiProbePort* getInstance();

    Return<int32_t> wifi_probe_open();
    Return<int32_t> wifi_probe_close();
    Return<int32_t> wifi_probe_start_search();
    Return<int32_t> wifi_probe_stop_search();
    Return<void> wifi_probe_get_version(wifi_probe_get_version_cb _hidl_cb);
    Return<int32_t> wifi_probe_start_firmware_upgrade(const hidl_vec<uint8_t> &upgrade_data, const int32_t upgrade_data_len);
    Return<int32_t> wifi_probe_set_rate(const int32_t rate);
    Return<void> wifi_probe_get_data(wifi_probe_get_data_cb _hidl_cb);
    Return<void> wifi_probe_get_eint(const bool start, const sp<IWifiProbeCallback>& cb);
    Return<int32_t> wifi_probe_set_firmware_size(const int32_t size);
    Return<int32_t> wifi_probe_set_firmware_done(const int32_t size);
private:
    static hw_device_t* openHal();
    static WifiProbePort* sInstance;
};

}  // namespace implementation
}  // namespace V1_0
}  // namespace wifi_probe
}  // namespace hardware
}  // namespace hct
}  // namespace vendor

#endif  // VENDOR_HCT_HARDWARE_WIFI_PROBE_V1_0_PROBE_H
