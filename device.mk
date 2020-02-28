#
# Copyright (C) 2020 The PixelExperience Project
#
# SPDX-License-Identifier: Apache-2.0
#

DEVICE_PATH := device/xiaomi/begonia

# Overlays
DEVICE_PACKAGE_OVERLAYS += \
    $(DEVICE_PATH)/overlay

# Properties
-include $(DEVICE_PATH)/system_prop.mk

# Permissions
PRODUCT_COPY_FILES += \
    frameworks/native/data/etc/handheld_core_hardware.xml:system/etc/permissions/handheld_core_hardware.xml

# Audio
PRODUCT_PACKAGES += \
    audio.a2dp.default

# Camera
PRODUCT_PACKAGES += \
    Snap

# Display
PRODUCT_PACKAGES += \
    libvulkan \
    vendor.display.config@1.0

# HIDL
PRODUCT_PACKAGES += \
    android.hidl.base@1.0 \
    android.hidl.manager@1.0

# Init
PRODUCT_PACKAGES += \
    init.mtk.rc

# Input
PRODUCT_COPY_FILES += \
    $(DEVICE_PATH)/idc/uinput-fpc.idc:system/usr/idc/uinput-fpc.idc \
    $(DEVICE_PATH)/idc/uinput-goodix.idc:system/usr/idc/uinput-goodix.idc

PRODUCT_COPY_FILES += \
    $(DEVICE_PATH)/keylayout/uinput-fpc.kl:system/usr/keylayout/uinput-fpc.kl \
    $(DEVICE_PATH)/keylayout/uinput-goodix.kl:system/usr/keylayout/uinput-goodix.kl

# Light
PRODUCT_PACKAGES += \
    android.hardware.light@2.0-service.begonia

# Net
PRODUCT_PACKAGES += \
    netutils-wrapper-1.0

# NFC
PRODUCT_PACKAGES += \
    NfcNci \
    Tag \
    com.android.nfc_extras

# Inherit vendor
$(call inherit-product, vendor/xiaomi/begonia/begonia-vendor.mk)
