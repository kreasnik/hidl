#ifndef VENDOR_HCT_HARDWARE_GESTUREWAKE_V1_0_GESTUREWAKE_H
#define VENDOR_HCT_HARDWARE_GESTUREWAKE_V1_0_GESTUREWAKE_H

#include <vendor/hct/hardware/gesturewake/1.0/IGestureWake.h>
#include <vendor/hct/hardware/gesturewake/1.0/IGestureWakeCallback.h>
#include <hidl/MQDescriptor.h>
#include <hidl/Status.h>

#include <cutils/log.h>
#include <pthread.h>
#include <unistd.h>
#include <../../../../libhardware/gesturewake/hw_gesturewake.h>

namespace vendor {
namespace hct {
namespace hardware {
namespace gesturewake {
namespace V1_0 {
namespace implementation {

using ::vendor::hct::hardware::gesturewake::V1_0::IGestureWake;
using ::vendor::hct::hardware::gesturewake::V1_0::IGestureWakeCallback;
using ::android::hidl::base::V1_0::DebugInfo;
using ::android::hidl::base::V1_0::IBase;
using ::android::hardware::hidl_array;
using ::android::hardware::hidl_memory;
using ::android::hardware::hidl_string;
using ::android::hardware::hidl_vec;
using ::android::hardware::Return;
using ::android::hardware::Void;
using ::android::sp;

struct GestureWake : public IGestureWake {
public:
    GestureWake();
    ~GestureWake();
    hw_device_t *mDevice;
    // Method to wrap legacy HAL with GestureWake class
    static IGestureWake* getInstance();

    Return<int32_t> startGesture(bool state) override;
    Return<void> readGesture(const sp<IGestureWakeCallback>& cb)  override;
private:
    static hw_device_t* openHal();
    static GestureWake* sInstance;
};

}  // namespace implementation
}  // namespace V1_0
}  // namespace gesturewake
}  // namespace hardware
}  // namespace hct
}  // namespace vendor

#endif  // VENDOR_HCT_HARDWARE_GESTUREWAKE_V1_0_GESTUREWAKE_H
