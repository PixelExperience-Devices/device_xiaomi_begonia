LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_REQUIRED_MODULES :=

LOCAL_CFLAGS += -Wno-unused-parameter -Wno-int-to-pointer-cast
LOCAL_CFLAGS += -Wno-maybe-uninitialized -Wno-parentheses
LOCAL_CPPFLAGS += -Wno-conversion-null

ifneq ($(TARGET_BUILD_VARIANT), user)
LOCAL_CFLAGS += -DCONFIG_DEBUG_VER
endif

LOCAL_C_INCLUDES += \
	external/libnl/include \
	$(call include-path-for, libhardware_legacy)/hardware_legacy \
	external/wpa_supplicant_8/src/drivers

LOCAL_SRC_FILES := \
	wifi_hal.cpp \
	common.cpp \
	cpp_bindings.cpp \
	wifi_logger.cpp \
	wifi_offload.cpp

LOCAL_SHARED_LIBRARIES += libutils liblog
LOCAL_MODULE := libwifi-hal-mt66xx
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_OWNER := mtk

include $(BUILD_STATIC_LIBRARY)
