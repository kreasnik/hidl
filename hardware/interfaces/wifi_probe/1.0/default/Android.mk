LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := vendor.hct.hardware.wifi_probe@1.0-service
LOCAL_INIT_RC := vendor.hct.hardware.wifi_probe@1.0-service.rc
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_RELATIVE_PATH := hw
LOCAL_SRC_FILES := \
    WifiProbePort.cpp \
    service.cpp \

LOCAL_SHARED_LIBRARIES := \
    libcutils \
    libutils \
    liblog \
    libhidlbase \
    libhidltransport \
    libhardware \
    libutils \
    vendor.hct.hardware.wifi_probe@1.0 \

include $(BUILD_EXECUTABLE)
