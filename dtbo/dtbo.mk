#
# Copyright (C) 2020 The LineageOS Project
#
# SPDX-License-Identifier: Apache-2.0
#

APPEND_CERTS := $(DEVICE_PATH)/dtbo/append_certs.py
BOARD_DTBO_CFG := $(DTBO_OUT)/dtboimg.cfg
MKDTIMG := $(HOST_OUT_EXECUTABLES)/mkdtimg$(HOST_EXECUTABLE_SUFFIX)
MKDTBOIMG := $(HOST_OUT_EXECUTABLES)/mkdtboimg.py$(HOST_EXECUTABLE_SUFFIX)

$(BOARD_PREBUILT_DTBOIMAGE): $(DTC) $(MKDTIMG) $(MKDTBOIMG)
$(BOARD_PREBUILT_DTBOIMAGE):
	@echo "Building dtbo.img"
	$(call make-dtbo-target,$(KERNEL_DEFCONFIG))
	$(call make-dtbo-target,dtbs)
	$(MKDTBOIMG) cfg_create $@ $(BOARD_DTBO_CFG) -d $(DTBO_OUT)/arch/$(KERNEL_ARCH)/boot/dts
	$(APPEND_CERTS) --alignment 16 --cert1 $(DEVICE_PATH)/dtbo/security/cert1.der --cert2 $(DEVICE_PATH)/dtbo/security/cert2.der --dtbo $(BOARD_PREBUILT_DTBOIMAGE)
