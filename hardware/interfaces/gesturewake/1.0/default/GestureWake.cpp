#define LOG_TAG "GestureWakeService"
#define LOG_VERBOSE "GestureWakeService"

#include "GestureWake.h"

namespace vendor {
namespace hct {
namespace hardware {
namespace gesturewake {
namespace V1_0 {
namespace implementation {

pthread_t tid;
volatile bool destroyThread;
GestureWake *GestureWake::sInstance = nullptr;
sp<IGestureWakeCallback> mGestureWakeCallback = nullptr;

GestureWake::GestureWake() : mDevice(nullptr){
    ALOGV("~GestureWake()");
    sInstance = this; // keep track of the most recent instance
    mDevice = openHal();
    gesturewake_t* dev = reinterpret_cast<gesturewake_t*>(mDevice);
    dev->gesturewake_init(dev);
}

GestureWake::~GestureWake() {
    ALOGV("~GestureWake()");
    if (mDevice == nullptr) {
        ALOGE("No valid device");
        return;
    }
    int err;
    gesturewake_t* dev = reinterpret_cast<gesturewake_t*>(mDevice);
    if (0 != (err = dev->common.close(mDevice))) {
        ALOGE("Can't close GestureWake module, error: %d", err);
        return;
    }
    mDevice = nullptr;
}

IGestureWake* GestureWake::getInstance() {
    if (!sInstance) {
      sInstance = new GestureWake();
    }
    return sInstance;
}

void* thread_start(void *arg){
    char* data = nullptr;
    GestureWake* thisPtr = static_cast<GestureWake*>(GestureWake::getInstance());
    while(!destroyThread){
        //hidl_string data = "c";
        gesturewake_t* dev = reinterpret_cast<gesturewake_t*>(thisPtr->mDevice);
        data = dev->gesturewake_read(dev);
        ALOGD("thread_start, data: %s", data);
        if(mGestureWakeCallback != nullptr && data != nullptr){
            mGestureWakeCallback->reportResults(data);
        }
    }
    return NULL;
}

void sighandler(int sig){
    if (sig == SIGUSR1) {
        destroyThread = true;
        ALOGI("destroy set");
        return;
    }
    signal(SIGUSR1, sighandler);
}

Return<int32_t> GestureWake::startGesture(bool state){
    int32_t result = -1;
    signal(SIGUSR1, sighandler);
    gesturewake_t* dev = reinterpret_cast<gesturewake_t*>(mDevice);
    if(state){
        result = dev->gesturewake_start(dev, 1);
        destroyThread = false;
        if(result == 0){
            pthread_create(&tid,NULL,thread_start,NULL);
        }
    }else{
        result = dev->gesturewake_start(dev, 0);
        pthread_kill(tid, SIGUSR1);
    }
    return result;
}

Return<void> GestureWake::readGesture(const sp<IGestureWakeCallback>& cb){
    ALOGV("readGesture()");
    mGestureWakeCallback = cb;
    return Void();
}

hw_device_t* GestureWake::openHal(){
    hw_device_t* dev = nullptr;
    hw_module_t const* module;
    int err = hw_get_module(GESTUREWAKE_HARDWARE_MODULE_ID, &module);
    if (err != 0) {
        ALOGE("Can't open gesturewake detect HW Module, error: %d", err);
        return nullptr;
    }

    err = module->methods->open(module, "gesturewake", &dev);
    if (err < 0) {
        ALOGE("Can't open gesturewake Detect, error: %d", err);
        return nullptr;
    }
    ALOGD("Open gesturewake hal");
    return dev;
}

} //namespace implementation
} //namespace V1_0
} //namespace gesturewake
} //namespace hardware
} //namespace hct
} //namespace vendor