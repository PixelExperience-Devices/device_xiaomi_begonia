#
# Copyright (C) 2020 The PixelExperience Project
#
# SPDX-License-Identifier: Apache-2.0
#

# Kernel modules
KERNEL_MODULES_ORIG := $(LOCAL_PATH)/modules
KERNEL_MODULES_DEST := $(TARGET_COPY_OUT_VENDOR)/lib/modules

PRODUCT_COPY_FILES += \
    $(call find-copy-subdir-files,*,$(KERNEL_MODULES_ORIG)/,$(KERNEL_MODULES_DEST))

# Rootdir
PRODUCT_PACKAGES += \
    init.bt_drv.rc \
    init.cccimdinit.rc \
    init.fmradio_drv.rc \
    init.gps_drv.rc \
    init.udc_lib.rc \
    init.wlan_drv.rc \
    init.wmt_drv.rc
