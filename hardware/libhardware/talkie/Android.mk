LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := optional
LOCAL_SHARED_LIBRARIES := liblog libcutils
LOCAL_SRC_FILES := ioctl_talkie.c hw_talkie.c
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_RELATIVE_PATH := hw
LOCAL_MODULE := talkie.default
LOCAL_INIT_RC := init.talkie.rc

include $(BUILD_SHARED_LIBRARY)
