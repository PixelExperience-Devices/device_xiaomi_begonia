#
# Copyright (C) 2019 The Potato Open Sauce Project
#
# SPDX-License-Identifier: Apache-2.0
#

DEVICE_PATH := device/redmi/begonia

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

# Net
PRODUCT_PACKAGES += \
    netutils-wrapper-1.0

# NFC
PRODUCT_PACKAGES += \
    NfcNci \
    Tag \
    com.android.nfc_extras

# Inherit vendor
$(call inherit-product, vendor/redmi/begonia/begonia-vendor.mk)
