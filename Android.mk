LOCAL_PATH := $(call my-dir)

ifeq ($(TARGET_DEVICE),begonia)

include $(call all-subdir-makefiles,$(LOCAL_PATH))

include $(CLEAR_VARS)

VENDOR_SYMLINKS := \
    $(TARGET_OUT_VENDOR) \
    $(TARGET_OUT_VENDOR)/lib/hw \
    $(TARGET_OUT_VENDOR)/lib64/hw

$(VENDOR_SYMLINKS): $(LOCAL_INSTALLED_MODULE)
	$(hide) echo "Making vendor symlinks"
	@ln -sf /mnt/vendor/cust $(TARGET_OUT_VENDOR)/cust
	@ln -sf /mnt/vendor/nvcfg $(TARGET_OUT_VENDOR)/nvcfg
	@ln -sf /mnt/vendor/nvdata $(TARGET_OUT_VENDOR)/nvdata
	@ln -sf /mnt/vendor/persist $(TARGET_OUT_VENDOR)/persist
	@ln -sf /mnt/vendor/protect_f $(TARGET_OUT_VENDOR)/protect_f
	@ln -sf /mnt/vendor/protect_s $(TARGET_OUT_VENDOR)/protect_s
	@mkdir -p $(TARGET_OUT_VENDOR)/lib/hw
	@mkdir -p $(TARGET_OUT_VENDOR)/lib64/hw
	@ln -sf libSoftGatekeeper.so $(TARGET_OUT_VENDOR)/lib/hw/gatekeeper.default.so
	@ln -sf libSoftGatekeeper.so $(TARGET_OUT_VENDOR)/lib64/hw/gatekeeper.default.so
	$(hide) touch $@

ALL_DEFAULT_INSTALLED_MODULES += $(VENDOR_SYMLINKS)

endif
