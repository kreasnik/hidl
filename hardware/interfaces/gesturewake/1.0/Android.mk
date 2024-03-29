# This file is autogenerated by hidl-gen. Do not edit manually.

LOCAL_PATH := $(call my-dir)

################################################################################

include $(CLEAR_VARS)
LOCAL_MODULE := vendor.hct.hardware.gesturewake-V1.0-java
LOCAL_MODULE_CLASS := JAVA_LIBRARIES

intermediates := $(call local-generated-sources-dir, COMMON)

HIDL := $(HOST_OUT_EXECUTABLES)/hidl-gen$(HOST_EXECUTABLE_SUFFIX)

LOCAL_JAVA_LIBRARIES := \
    android.hidl.base-V1.0-java \

LOCAL_NO_STANDARD_LIBRARIES := true
LOCAL_JAVA_LIBRARIES += core-oj hwbinder

#
# Build IGestureWake.hal
#
GEN := $(intermediates)/vendor/hct/hardware/gesturewake/V1_0/IGestureWake.java
$(GEN): $(HIDL)
$(GEN): PRIVATE_HIDL := $(HIDL)
$(GEN): PRIVATE_DEPS := $(LOCAL_PATH)/IGestureWake.hal
$(GEN): PRIVATE_DEPS += $(LOCAL_PATH)/IGestureWakeCallback.hal
$(GEN): $(LOCAL_PATH)/IGestureWakeCallback.hal
$(GEN): PRIVATE_OUTPUT_DIR := $(intermediates)
$(GEN): PRIVATE_CUSTOM_TOOL = \
        $(PRIVATE_HIDL) -o $(PRIVATE_OUTPUT_DIR) \
        -Ljava \
        -randroid.hidl:system/libhidl/transport \
        -rvendor.hct.hardware:vendor/haocheng/proprietary/hardware/interfaces \
        vendor.hct.hardware.gesturewake@1.0::IGestureWake

$(GEN): $(LOCAL_PATH)/IGestureWake.hal
	$(transform-generated-source)
LOCAL_GENERATED_SOURCES += $(GEN)

#
# Build IGestureWakeCallback.hal
#
GEN := $(intermediates)/vendor/hct/hardware/gesturewake/V1_0/IGestureWakeCallback.java
$(GEN): $(HIDL)
$(GEN): PRIVATE_HIDL := $(HIDL)
$(GEN): PRIVATE_DEPS := $(LOCAL_PATH)/IGestureWakeCallback.hal
$(GEN): PRIVATE_OUTPUT_DIR := $(intermediates)
$(GEN): PRIVATE_CUSTOM_TOOL = \
        $(PRIVATE_HIDL) -o $(PRIVATE_OUTPUT_DIR) \
        -Ljava \
        -randroid.hidl:system/libhidl/transport \
        -rvendor.hct.hardware:vendor/haocheng/proprietary/hardware/interfaces \
        vendor.hct.hardware.gesturewake@1.0::IGestureWakeCallback

$(GEN): $(LOCAL_PATH)/IGestureWakeCallback.hal
	$(transform-generated-source)
LOCAL_GENERATED_SOURCES += $(GEN)
include $(BUILD_JAVA_LIBRARY)


################################################################################

include $(CLEAR_VARS)
LOCAL_MODULE := vendor.hct.hardware.gesturewake-V1.0-java-static
LOCAL_MODULE_CLASS := JAVA_LIBRARIES

intermediates := $(call local-generated-sources-dir, COMMON)

HIDL := $(HOST_OUT_EXECUTABLES)/hidl-gen$(HOST_EXECUTABLE_SUFFIX)

LOCAL_STATIC_JAVA_LIBRARIES := \
    android.hidl.base-V1.0-java-static \

LOCAL_NO_STANDARD_LIBRARIES := true
LOCAL_JAVA_LIBRARIES := core-oj hwbinder

#
# Build IGestureWake.hal
#
GEN := $(intermediates)/vendor/hct/hardware/gesturewake/V1_0/IGestureWake.java
$(GEN): $(HIDL)
$(GEN): PRIVATE_HIDL := $(HIDL)
$(GEN): PRIVATE_DEPS := $(LOCAL_PATH)/IGestureWake.hal
$(GEN): PRIVATE_DEPS += $(LOCAL_PATH)/IGestureWakeCallback.hal
$(GEN): $(LOCAL_PATH)/IGestureWakeCallback.hal
$(GEN): PRIVATE_OUTPUT_DIR := $(intermediates)
$(GEN): PRIVATE_CUSTOM_TOOL = \
        $(PRIVATE_HIDL) -o $(PRIVATE_OUTPUT_DIR) \
        -Ljava \
        -randroid.hidl:system/libhidl/transport \
        -rvendor.hct.hardware:vendor/haocheng/proprietary/hardware/interfaces \
        vendor.hct.hardware.gesturewake@1.0::IGestureWake

$(GEN): $(LOCAL_PATH)/IGestureWake.hal
	$(transform-generated-source)
LOCAL_GENERATED_SOURCES += $(GEN)

#
# Build IGestureWakeCallback.hal
#
GEN := $(intermediates)/vendor/hct/hardware/gesturewake/V1_0/IGestureWakeCallback.java
$(GEN): $(HIDL)
$(GEN): PRIVATE_HIDL := $(HIDL)
$(GEN): PRIVATE_DEPS := $(LOCAL_PATH)/IGestureWakeCallback.hal
$(GEN): PRIVATE_OUTPUT_DIR := $(intermediates)
$(GEN): PRIVATE_CUSTOM_TOOL = \
        $(PRIVATE_HIDL) -o $(PRIVATE_OUTPUT_DIR) \
        -Ljava \
        -randroid.hidl:system/libhidl/transport \
        -rvendor.hct.hardware:vendor/haocheng/proprietary/hardware/interfaces \
        vendor.hct.hardware.gesturewake@1.0::IGestureWakeCallback

$(GEN): $(LOCAL_PATH)/IGestureWakeCallback.hal
	$(transform-generated-source)
LOCAL_GENERATED_SOURCES += $(GEN)
include $(BUILD_STATIC_JAVA_LIBRARY)



include $(call all-makefiles-under,$(LOCAL_PATH))
