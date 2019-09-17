#define LOG_TAG "TalkieService"
#define LOG_VERBOSE "TalkieService"

#include "TalkiePort.h"

namespace vendor {
namespace hct {
namespace hardware {
namespace talkie {
namespace V1_0 {
namespace implementation {

pthread_t tid;
volatile bool destroyThread;
TalkiePort *TalkiePort::sInstance = nullptr;
sp<ITalkiePortCallback> mTalkiePortCallback = nullptr;

TalkiePort::TalkiePort() : mDevice(nullptr){
    LOGD("~TalkiePort()");
    sInstance = this; // keep track of the most recent instance
    mDevice = openHal();
    //talkie_t* dev = reinterpret_cast<talkie_t*>(mDevice);
    //dev->talkie_init(dev);
}

TalkiePort::~TalkiePort() {
    LOGD("~TalkiePort()");
    if (mDevice == nullptr) {
        LOGD("No valid device");
        return;
    }
    int err;
    talkie_t* dev = reinterpret_cast<talkie_t*>(mDevice);
    if (0 != (err = dev->common.close(mDevice))) {
        LOGD("Can't close talkie module, error: %d", err);
        return;
    }
    mDevice = nullptr;
}

ITalkiePort* TalkiePort::getInstance() {
    if (!sInstance) {
      sInstance = new TalkiePort();
    }
    return sInstance;
}

Return<int32_t> TalkiePort::talkie_init(){
    if(mDevice == NULL){
        return -1;
    }
    talkie_t* dev = reinterpret_cast<talkie_t*>(mDevice);
    LOGD("hal Device talkie_init \n");
    return dev-> talkie_init(dev);
}

Return<int32_t> TalkiePort::talkie_deinit(){
    if(mDevice == NULL){
        return -1;
    }
    talkie_t* dev = reinterpret_cast<talkie_t*>(mDevice);
    LOGD("hal Device talkie_deinit \n");
    return dev-> talkie_deinit(dev);
}

Return<int32_t> TalkiePort::talkie_app_start(){
    if(mDevice == NULL){
        return -1;
    }
    talkie_t* dev = reinterpret_cast<talkie_t*>(mDevice);
    LOGD("hal Device talkie_app_start \n");
    return dev-> talkie_app_start(dev);
}


Return<int32_t> TalkiePort::talkie_ppt_down(int32_t arg){
    if(mDevice == NULL){
        return -1;
    }
    talkie_t* dev = reinterpret_cast<talkie_t*>(mDevice);
    LOGD("hal Device talkie_ppt_down \n");
    return dev-> talkie_ppt_down(dev,arg);
}

Return<int32_t> TalkiePort::talkie_ppt_up(int32_t arg){
    if(mDevice == NULL){
        return -1;
    }
    talkie_t* dev = reinterpret_cast<talkie_t*>(mDevice);
    LOGD("hal Device talkie_ppt_up \n");
    return dev-> talkie_ppt_up(dev,arg);
}

void sighandler(int sig){
    if (sig == SIGUSR1) {
        destroyThread = true;
        LOGD("destroy set");
        return;
    }
    signal(SIGUSR1, sighandler);
}
void* thread_start(void *arg){
    LOGD("hal Device thread_start \n");
    int32_t respond = -1;
    TalkiePort* thisPtr = static_cast<TalkiePort*>(TalkiePort::getInstance());
    while(!destroyThread){
        //hidl_string data = "c";
        if(thisPtr->mDevice == NULL){
        		respond = -1;
        }else{
		     talkie_t* dev = reinterpret_cast<talkie_t*>(thisPtr->mDevice);
		     respond = dev-> talkie_speak_enable(dev);
		     LOGD("hal thread_start, respond: %d", respond);
        }
        if(mTalkiePortCallback != nullptr){
            mTalkiePortCallback->reportResults(respond);
        }
    }
    return NULL;
}

void startSpeakEnable(bool state){
    LOGD("hal Device startSpeakEnable \n");
    signal(SIGUSR1, sighandler);
    if(state){
        destroyThread = false;
        pthread_create(&tid,NULL,thread_start,NULL);
    }else{
        pthread_kill(tid, SIGUSR1);
    }
}

Return<void>  TalkiePort::talkie_speak_enable(const sp<ITalkiePortCallback>& cb){
    LOGD("hal Device talkie_speak_enable \n");
    mTalkiePortCallback = cb;
    startSpeakEnable(true);
    return Void();
}

Return<int32_t> TalkiePort::talkie_speak_disable(){
    if(mDevice == NULL){
        return -1;
    }
    startSpeakEnable(false);
    talkie_t* dev = reinterpret_cast<talkie_t*>(mDevice);
    LOGD("hal Device talkie_speak_disable \n");
    return dev-> talkie_speak_disable(dev);
}

Return<int32_t> TalkiePort::talkie_app_exit(){
    if(mDevice == NULL){
        return -1;
    }
    startSpeakEnable(false);
    talkie_t* dev = reinterpret_cast<talkie_t*>(mDevice);
    LOGD("hal Device talkie_app_exit \n");
    return dev-> talkie_app_exit(dev);
}

hw_device_t* TalkiePort::openHal(){
    hw_device_t* dev = nullptr;
    hw_module_t const* module;
    int err = hw_get_module(TALKIE_HARDWARE_MODULE_ID, &module);
    if (err != 0) {
        LOGD("Can't open TalkiePort detect HW Module, error: %d", err);
        return nullptr;
    }

    err = module->methods->open(module, "TalkiePort", &dev);
    if (err < 0) {
        LOGD("Can't open TalkiePort Detect, error: %d", err);
        return nullptr;
    }
    LOGD("Open TalkiePort hal");
    return dev;
}

} //namespace implementation
} //namespace V1_0
} //namespace talkie
} //namespace hardware
} //namespace hct
} //namespace vendor
