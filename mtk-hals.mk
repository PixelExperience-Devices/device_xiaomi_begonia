#
# Copyright (C) 2020 The PixelExperience Project
#
# SPDX-License-Identifier: Apache-2.0
#

# AEE Config
$(call inherit-product, vendor/mediatek/opensource/external/aee/config_external/aee.mk)

# AEE
PRODUCT_PACKAGES += \
    rttv \
    aeev \
    libaedv \
    aee_aedv \
    aee_aedv64 \
    aee_archivev \
    aee_dumpstatev \
    libdirect-coredump \
    libdirect-coredump.vendor \
    vendor.mediatek.hardware.log@1.0-service

# Audio
PRODUCT_PACKAGES += \
    libfile_op \
    libmtk_drvb \
    libccci_util \
    libaudioloudc \
    libmtkshifter \
    libmtklimiter \
    libaudiofmtconv \
    lib_speech_enh \
    libMtkSpeechEnh \
    libblisrc32_vendor \
    libmtkaudio_utils \
    libspeech_enh_lib \
    libaudiosmartpamtk \
    libmtkshifter_vendor \
    libmtklimiter_vendor \
    libaudiotoolkit_vendor \
    libaudiodcrflt_vendor \
    libspeechparser_vendor \
    libaudiocustparam_vendor \
    libaudiocomponentenginec \
    libbessound_hd_mtk_vendor \
    libaudio_param_parser-vnd \
    libaudiocompensationfilterc \
    libaudiocomponentengine_vendor \
    libaudioprimarydevicehalifclient \
    libaudiocompensationfilter_vendor \
    vendor.mediatek.hardware.audio@5.1 \
    android.hardware.audio@5.0-impl-mediatek

# Bluetooth
PRODUCT_PACKAGES += \
    android.hardware.bluetooth@1.0-impl-mediatek \
    android.hardware.bluetooth@1.0-service-mediatek

# Ccci
PRODUCT_PACKAGES += \
    ccci_fsd \
    ccci_mdinit \
    ccci_rpcd

# Consumerir
PRODUCT_PACKAGES += \
    consumerir.mt6785

# Dplanner
PRODUCT_PACKAGES += \
    vendor.mediatek.hardware.dplanner@2.0 \
    vendor.mediatek.hardware.dplanner@2.0-service

# Keymaster
PRODUCT_PACKAGES += \
    libkmsetkey \
    vendor.mediatek.hardware.keymaster_attestation@1.0 \
    vendor.mediatek.hardware.keymaster_attestation@1.1 \
    vendor.mediatek.hardware.keymaster_attestation@1.1-impl \
    vendor.mediatek.hardware.keymaster_attestation@1.1-service

# Media
PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/configs/media/media_codecs_mediatek_audio.xml:$(TARGET_COPY_OUT_VENDOR)/etc/media_codecs_mediatek_audio.xml \
    $(LOCAL_PATH)/configs/media/media_codecs_mediatek_video.xml:$(TARGET_COPY_OUT_VENDOR)/etc/media_codecs_mediatek_video.xml

# Nvram
PRODUCT_PACKAGES += \
    libnvram \
    libnvram_sec \
    libcustom_nvram \
    libnvram_platform \
    libnvram_daemon_callback \
    vendor.mediatek.hardware.nvram@1.1-impl \
    vendor.mediatek.hardware.nvram@1.1-service \
    vendor.mediatek.hardware.nvram@1.0 \
    vendor.mediatek.hardware.nvram@1.1

# Permissions
PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/configs/permissions/privapp-permissions-mediatek.xml:$(TARGET_COPY_OUT_SYSTEM)/etc/permissions/privapp-permissions-mediatek.xml

# Power
PRODUCT_PACKAGES += \
    fstb.cfg \
    libpowerhal \
    power.mt6785 \
    powerscntbl.xml \
    libpowerhalwrap \
    powercontable.xml \
    libmtkperf_client \
    libpowerhalwrap_vendor \
    power_native_test_v_1_0 \
    power_legacy_test_v_1_0 \
    libmtkperf_client_vendor \
    vendor.mediatek.hardware.power@1.1 \
    vendor.mediatek.hardware.power@2.0 \
    vendor.mediatek.hardware.power@2.1 \
    vendor.mediatek.hardware.netdagent@1.0 \
    vendor.mediatek.hardware.power@2.1-impl \
    android.hardware.power@1.3-impl-mediatek \
    vendor.mediatek.hardware.mtkpower@1.0 \
    vendor.mediatek.hardware.mtkpower@1.0-impl \
    vendor.mediatek.hardware.mtkpower@1.0-service

# Ril
PRODUCT_PACKAGES += \
    libsysenv

# Telephony Jars
PRODUCT_BOOT_JARS += \
    mediatek-common \
    mediatek-framework \
    mediatek-ims-base \
    mediatek-ims-common \
    mediatek-telecom-common \
    mediatek-telephony-base \
    mediatek-telephony-common

# Thermal
PRODUCT_PACKAGES += \
    thermal.mt6785

# USB
PRODUCT_PACKAGES += \
    android.hardware.usb@1.1-service-mediatek
