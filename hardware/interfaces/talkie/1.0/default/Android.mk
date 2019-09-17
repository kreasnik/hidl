LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := vendor.hct.hardware.talkie@1.0-service
LOCAL_INIT_RC := vendor.hct.hardware.talkie@1.0-service.rc
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_RELATIVE_PATH := hw
LOCAL_SRC_FILES := \
    TalkiePort.cpp \
    service.cpp \

LOCAL_SHARED_LIBRARIES := \
    libcutils \
    liblog \
    libhidlbase \
    libhidltransport \
    libhardware \
    libutils \
    vendor.hct.hardware.talkie@1.0 \

include $(BUILD_EXECUTABLE)
