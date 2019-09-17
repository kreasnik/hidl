LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := optional
LOCAL_SHARED_LIBRARIES := liblog libcutils libutils
LOCAL_SRC_FILES := ioctl_wifi_probe.c hw_wifi_probe.c
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_RELATIVE_PATH := hw
LOCAL_MODULE := wifi_probe.default

include $(BUILD_SHARED_LIBRARY)
