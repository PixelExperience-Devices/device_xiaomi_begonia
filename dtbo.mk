BOARD_PREBUILT_DTBOIMAGE := $(PRODUCT_OUT)/dtbo-pre.img
BOARD_KERNEL_DTBO_CFG := dtboimg.cfg
MKDTBOIMG := system/libufdt/utils/src/mkdtboimg.py
APPEND_CERTS := $(DEVICE_PATH)/append_certs.py

# BUG: mkdtboimg.py doesn't support absolute paths yet. Fix this later.
define build-dtboimage-target-from-cfg
    $(call pretty,"Target dtbo image from cfg: $(BOARD_PREBUILT_DTBOIMAGE)")
    $(hide) $(MKDTBOIMG) cfg_create $@ $(KERNEL_OUT)/$(BOARD_KERNEL_DTBO_CFG) -d /
    $(hide) chmod a+r $@
endef

define append-dtboimage-certs
    $(call pretty,"Target signed dtbo image: $(BOARD_PREBUILT_DTBOIMAGE)")
    $(hide) mv $(BOARD_PREBUILT_DTBOIMAGE) $(BOARD_PREBUILT_DTBOIMAGE).tmp
    $(hide) $(APPEND_CERTS) --image $(BOARD_PREBUILT_DTBOIMAGE).tmp --cert1 \
        $(DEVICE_PATH)/security/cert1.der --cert2 $(DEVICE_PATH)/security/cert2.der --output $(BOARD_PREBUILT_DTBOIMAGE)
    $(hide) chmod a+r $@
endef


$(BOARD_PREBUILT_DTBOIMAGE): $(MKDTBOIMG) $(INSTALLED_KERNEL_TARGET)
	$(build-dtboimage-target-from-cfg)
	$(append-dtboimage-certs)
