#include "WifiProbePort.h"

namespace vendor {
namespace hct {
namespace hardware {
namespace wifi_probe {
namespace V1_0 {
namespace implementation {

pthread_t tid;
volatile bool destroyThread;
WifiProbePort *WifiProbePort::sInstance = nullptr;
sp<IWifiProbeCallback> mWifiProbeCallback = nullptr;

WifiProbePort::WifiProbePort() : mDevice(nullptr){
    ALOGD("+WifiProbePort()");
    sInstance = this; // keep track of the most recent instance
    mDevice = openHal();
}

WifiProbePort::~WifiProbePort() {
    ALOGD("~WifiProbePort()");
    if (mDevice == nullptr) {
        ALOGD("No valid device");
        return;
    }
    int err;
    wifi_probe_t* dev = reinterpret_cast<wifi_probe_t*>(mDevice);
    if (0 != (err = dev->common.close(mDevice))) {
        ALOGD("Can't close wifi_probe module, error: %d", err);
        return;
    }
    mDevice = nullptr;
}

IWifiProbePort* WifiProbePort::getInstance() {
    if (!sInstance) {
        sInstance = new WifiProbePort();
    }
    return sInstance;
}

Return<int32_t> WifiProbePort::wifi_probe_open(){
    if(mDevice == NULL){
        return -1;
    }
    wifi_probe_t* dev = reinterpret_cast<wifi_probe_t*>(mDevice);
    ALOGD("hal Device wifi_probe_open \n");
    return dev->wifi_probe_open(dev);
}

Return<int32_t> WifiProbePort::wifi_probe_close(){
    if(mDevice == NULL){
        return -1;
    }
    wifi_probe_t* dev = reinterpret_cast<wifi_probe_t*>(mDevice);
    ALOGD("hal Device wifi_probe_close \n");
    return dev->wifi_probe_close(dev);
}

Return<int32_t> WifiProbePort::wifi_probe_start_search(){
    int32_t ret = -1;
    if(mDevice == NULL){
        return -1;
    }
    message_t msg;
    memset(&msg, 0, sizeof(message_t));
    msg.header.id = AT_M2S_START_SEARCH;
    msg.header.len = 0;
    wifi_probe_t* dev = reinterpret_cast<wifi_probe_t*>(mDevice);
    ALOGD("hal Device wifi_probe_start_search \n");
    dev->wifi_probe_start_search(dev, &msg);
    if(msg.header.result == SUCCESS){
        ret = 0;
    }
    return ret;
}


Return<int32_t> WifiProbePort::wifi_probe_stop_search(){
    int32_t ret = -1;
    if(mDevice == NULL){
        return -1;
    }
    message_t msg;
    memset(&msg, 0, sizeof(message_t));
    msg.header.id = AT_M2S_STOP_SEARCH;
    msg.header.len = 0;
    wifi_probe_t* dev = reinterpret_cast<wifi_probe_t*>(mDevice);
    ALOGD("hal Device wifi_probe_stop_search \n");
    dev->wifi_probe_stop_search(dev, &msg);
    if(msg.header.result == SUCCESS){
        ret = 0;
    }
    return ret;
}

Return<void> WifiProbePort::wifi_probe_get_version(wifi_probe_get_version_cb _hidl_cb){
    if(mDevice == NULL){
        return Void();
    }
    message_t msg;
    memset(&msg, 0, sizeof(message_t));
    msg.header.id = AT_M2S_GET_VER;
    msg.header.len = 0;
    wifi_probe_t* dev = reinterpret_cast<wifi_probe_t*>(mDevice);
    ALOGD("hal Device wifi_probe_get_version \n");
    hidl_string version;
    dev->wifi_probe_get_version(dev, &msg);
    version = msg.para;
    _hidl_cb(version);
    return Void();
}

Return<int32_t> WifiProbePort::wifi_probe_start_firmware_upgrade(const hidl_vec<uint8_t> &upgrade_data, const int32_t upgrade_data_len){
    int32_t ret = -1;
    if(mDevice == NULL){
        return -1;
    }
    message_t msg;
    memset(&msg, 0, sizeof(message_t));
    msg.header.id = AT_M2S_UPGRADE;
    msg.header.len = upgrade_data_len;
    memcpy(msg.para, upgrade_data.data(), msg.header.len);
    wifi_probe_t* dev = reinterpret_cast<wifi_probe_t*>(mDevice);
    ALOGD("hal Device wifi_probe_start_firmware_upgrade \n");
    dev->wifi_probe_start_firmware_upgrade(dev, &msg);
    if(msg.header.result == SUCCESS){
        ret = 0;
    }
    return ret;
}

Return<int32_t> WifiProbePort::wifi_probe_set_rate(const int32_t rate){
    int32_t ret = -1;
    if(mDevice == NULL){
        return -1;
    }
    message_t msg;
    memset(&msg, 0, sizeof(message_t));
    msg.header.id = AT_M2S_SET_RATE;
    sprintf(msg.para, "%d", rate);
    msg.header.len = strlen(msg.para);
    wifi_probe_t* dev = reinterpret_cast<wifi_probe_t*>(mDevice);
    ALOGD("hal Device wifi_probe_set_rate para:%s \n", msg.para);
    dev->wifi_probe_set_rate(dev, &msg);
    if(msg.header.result == SUCCESS){
        ret = 0;
    }
    return ret;
}

Return<void> WifiProbePort::wifi_probe_get_data(wifi_probe_get_data_cb _hidl_cb){
    if(mDevice == NULL){
        return Void();
    }
    message_t msg;
    memset(&msg, 0, sizeof(message_t));
    msg.header.result = DATA_READ;
    msg.header.id = AT_M2S_GET_DATA;
    msg.header.len = 0;
    wifi_probe_t* dev = reinterpret_cast<wifi_probe_t*>(mDevice);
    ALOGD("hal Device wifi_probe_get_data \n");
    dev->wifi_probe_get_data(dev, &msg);
    ProbeData probe_data;
    if(msg.header.id == AT_M2S_GET_VER_RSP){
        probe_data.type = "version";
    }else if(msg.header.id == AT_M2S_GET_DATA_END_RSP){
        probe_data.type = "transfer_done";
    }else{
        probe_data.type = "transfer";
    }
    probe_data.state = msg.header.result;
    probe_data.result = msg.para;
    _hidl_cb(probe_data);
    return Void();
}

void sighandler(int sig){
    if (sig == SIGUSR1) {
        destroyThread = true;
        ALOGD("destroy set");
        return;
    }
    signal(SIGUSR1, sighandler);
}

void* thread_start(void *arg){
    ALOGD("hal Device thread_start \n");
    int32_t respond = -1;
    int32_t eint = 0;
    WifiProbePort* thisPtr = static_cast<WifiProbePort*>(WifiProbePort::getInstance());
    while(!destroyThread){
        if(thisPtr->mDevice == NULL){
        		respond = -1;
        }else{
		     wifi_probe_t* dev = reinterpret_cast<wifi_probe_t*>(thisPtr->mDevice);
		     respond = dev->wifi_probe_get_eint(dev, eint);
		     ALOGD("hal thread_start, respond: %d", respond);
        }
        if(mWifiProbeCallback != nullptr){
            mWifiProbeCallback->reportResults(respond);
        }
    }
    return NULL;
}

void checkWifiProbeEint(bool state){
    ALOGD("hal Device checkWifiProbeEint \n");
    signal(SIGUSR1, sighandler);
    if(state){
        destroyThread = false;
        pthread_create(&tid,NULL,thread_start,NULL);
    }else{
        pthread_kill(tid, SIGUSR1);
    }
}

Return<void>  WifiProbePort::wifi_probe_get_eint(const bool start, const sp<IWifiProbeCallback>& cb){
    ALOGD("hal Device wifi_probe_get_eint \n");
    mWifiProbeCallback = cb;
    checkWifiProbeEint(start);
    return Void();
}

Return<int32_t> WifiProbePort::wifi_probe_set_firmware_size(const int32_t size){
    int32_t ret = -1;
    if(mDevice == NULL){
        return -1;
    }
    message_t msg;
    memset(&msg, 0, sizeof(message_t));
    msg.header.id = AT_M2S_SET_FIRMWARE_SIZE;
    sprintf(msg.para, "%d", size);
    msg.header.len = strlen(msg.para);
    wifi_probe_t* dev = reinterpret_cast<wifi_probe_t*>(mDevice);
    ALOGD("hal Device wifi_probe_set_firmware_size para:%s \n", msg.para);
    dev->wifi_probe_set_firmware_size(dev, &msg);
    if(msg.header.result == SUCCESS){
        ret = 0;
    }
    return ret;
}

Return<int32_t> WifiProbePort::wifi_probe_set_firmware_done(const int32_t size){
    int32_t ret = -1;
    if(mDevice == NULL){
        return -1;
    }
    message_t msg;
    memset(&msg, 0, sizeof(message_t));
    msg.header.id = AT_M2S_UPGRADE_DONE;
    sprintf(msg.para, "%d", size);
    msg.header.len = strlen(msg.para);
    wifi_probe_t* dev = reinterpret_cast<wifi_probe_t*>(mDevice);
    ALOGD("hal Device wifi_probe_set_firmware_done para:%s \n", msg.para);
    dev->wifi_probe_set_firmware_done(dev, &msg);
    if(msg.header.result == SUCCESS){
        ret = 0;
    }
    return ret;
}

hw_device_t* WifiProbePort::openHal(){
    hw_device_t* dev = nullptr;
    hw_module_t const* module;
    int err = hw_get_module(WIFI_PROBE_HARDWARE_MODULE_ID, &module);
    if (err != 0) {
        ALOGD("Can't open WifiProbePort detect HW Module, error: %d", err);
        return nullptr;
    }

    err = module->methods->open(module, "wifi_probe", &dev);
    if (err < 0) {
        ALOGD("Can't open WifiProbePort Detect, error: %d", err);
        return nullptr;
    }
    ALOGD("Open WifiProbePort hal");
    return dev;
}

} //namespace implementation
} //namespace V1_0
} //namespace wifi_probe
} //namespace hardware
} //namespace hct
} //namespace vendor
