#ifndef VENDOR_HCT_HARDWARE_TALKIE_V1_0_TALKIE_H
#define VENDOR_HCT_HARDWARE_TALKIE_V1_0_TALKIE_H

#include <vendor/hct/hardware/talkie/1.0/ITalkiePort.h>
#include <vendor/hct/hardware/talkie/1.0/ITalkiePortCallback.h>
#include <hidl/MQDescriptor.h>
#include <hidl/Status.h>

#include <cutils/log.h>
#include <pthread.h>
#include <unistd.h>
#include "hw_talkie.h"
#include <utils/Log.h>

static const char *TAG="talkie-SerialPort";
#define LOGI(fmt, args...) __android_log_print(ANDROID_LOG_INFO,  TAG, fmt, ##args)
#define LOGD(fmt, args...) __android_log_print(ANDROID_LOG_DEBUG, TAG, fmt, ##args)
#define LOGE(fmt, args...) __android_log_print(ANDROID_LOG_ERROR, TAG, fmt, ##args)


namespace vendor {
namespace hct {
namespace hardware {
namespace talkie {
namespace V1_0 {
namespace implementation {

using ::vendor::hct::hardware::talkie::V1_0::ITalkiePort;
using ::android::hidl::base::V1_0::DebugInfo;
using ::android::hidl::base::V1_0::IBase;
using ::android::hardware::hidl_array;
using ::android::hardware::hidl_memory;
using ::android::hardware::hidl_string;
using ::android::hardware::hidl_vec;
using ::android::hardware::Return;
using ::android::hardware::Void;
using ::android::sp;

struct TalkiePort : public ITalkiePort {
public:
    TalkiePort();
    ~TalkiePort();
    hw_device_t *mDevice;
    // Method to wrap legacy HAL with GestureWake class
    static ITalkiePort* getInstance();

    Return<int32_t> talkie_init();
    Return<int32_t> talkie_deinit();
    Return<int32_t> talkie_app_start();
    Return<int32_t> talkie_app_exit();
    Return<int32_t> talkie_ppt_down(int32_t arg);
    Return<int32_t> talkie_ppt_up(int32_t arg);
    Return<void> talkie_speak_enable(const sp<ITalkiePortCallback>& cb);
    Return<int32_t> talkie_speak_disable();
private:
    static hw_device_t* openHal();
    static TalkiePort* sInstance;
};

}  // namespace implementation
}  // namespace V1_0
}  // namespace talkie
}  // namespace hardware
}  // namespace hct
}  // namespace vendor

#endif  // VENDOR_HCT_HARDWARE_TALKIE_V1_0_TALKIE_H
