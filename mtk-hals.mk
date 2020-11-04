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

# Camera
PRODUCT_PACKAGES += \
    vendor.mediatek.hardware.camera.atms@1.0.vendor \
    vendor.mediatek.hardware.camera.bgservice@1.0.vendor \
    vendor.mediatek.hardware.camera.bgservice@1.1.vendor \
    vendor.mediatek.hardware.camera.ccap@1.0.vendor \
    vendor.mediatek.hardware.camera.frhandler@1.0.vendor \
    vendor.mediatek.hardware.camera.lomoeffect@1.0.vendor \
    vendor.mediatek.hardware.camera.postproc@1.0.vendor \
    vendor.mediatek.hardware.camera.security@1.0.vendor

# Ccci
PRODUCT_PACKAGES += \
    ccci_fsd \
    ccci_mdinit \
    ccci_rpcd

# Consumerir
PRODUCT_PACKAGES += \
    consumerir.mt6785

# Dfps
PRODUCT_PACKAGES += \
    vendor.mediatek.hardware.dfps@1.0.vendor

# Dplanner
PRODUCT_PACKAGES += \
    vendor.mediatek.hardware.dplanner@2.0 \
    vendor.mediatek.hardware.dplanner@2.0-service

# Engineermode
PRODUCT_PACKAGES += \
    vendor.mediatek.hardware.engineermode@1.0.vendor \
    vendor.mediatek.hardware.engineermode@1.1.vendor \
    vendor.mediatek.hardware.engineermode@1.2.vendor

# Gpu
PRODUCT_PACKAGES += \
    vendor.mediatek.hardware.gpu@1.0.vendor

# Keymaster
PRODUCT_PACKAGES += \
    libkmsetkey \
    vendor.mediatek.hardware.keymaster_attestation@1.0 \
    vendor.mediatek.hardware.keymaster_attestation@1.1 \
    vendor.mediatek.hardware.keymaster_attestation@1.1-impl \
    vendor.mediatek.hardware.keymaster_attestation@1.1-service

# Lbs
PRODUCT_PACKAGES += \
    vendor.mediatek.hardware.lbs@1.0.vendor

# Mdmonitor
PRODUCT_PACKAGES += \
    vendor.mediatek.hardware.mdmonitor@1.0.vendor

# Media
PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/configs/media/media_codecs_mediatek_audio.xml:$(TARGET_COPY_OUT_VENDOR)/etc/media_codecs_mediatek_audio.xml \
    $(LOCAL_PATH)/configs/media/media_codecs_mediatek_video.xml:$(TARGET_COPY_OUT_VENDOR)/etc/media_codecs_mediatek_video.xml

# Mms
PRODUCT_PACKAGES += \
    vendor.mediatek.hardware.mms@1.0.vendor \
    vendor.mediatek.hardware.mms@1.1.vendor \
    vendor.mediatek.hardware.mms@1.2.vendor \
    vendor.mediatek.hardware.mms@1.3.vendor

# Mtkcodecservice
PRODUCT_PACKAGES += \
    vendor.mediatek.hardware.mtkcodecservice@1.1.vendor

# Mtkradioex
PRODUCT_PACKAGES += \
    vendor.mediatek.hardware.mtkradioex@1.0.vendor \
    vendor.mediatek.hardware.mtkradioex@1.1.vendor \
    vendor.mediatek.hardware.mtkradioex@1.2.vendor \
    vendor.mediatek.hardware.mtkradioex@1.3.vendor \
    vendor.mediatek.hardware.mtkradioex@1.4.vendor \
    vendor.mediatek.hardware.mtkradioex@1.5.vendor

# Neuropilot
PRODUCT_PACKAGES += \
    vendor.mediatek.hardware.neuropilot@1.0.vendor \
    vendor.mediatek.hardware.neuropilot@1.1.vendor

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

# Pq
PRODUCT_PACKAGES += \
    vendor.mediatek.hardware.pq@2.0.vendor \
    vendor.mediatek.hardware.pq@2.1.vendor \
    vendor.mediatek.hardware.pq@2.2.vendor \
    vendor.mediatek.hardware.pq@2.3.vendor

# Radio OP
PRODUCT_PACKAGES += \
    vendor.mediatek.hardware.radio_op@1.1.vendor \
    vendor.mediatek.hardware.radio_op@1.2.vendor \
    vendor.mediatek.hardware.radio_op@1.3.vendor \
    vendor.mediatek.hardware.radio_op@1.4.vendor \
    vendor.mediatek.hardware.radio_op@2.0.vendor

# Ril
PRODUCT_PACKAGES += \
    libsysenv

# Sensors
PRODUCT_PACKAGES += \
    android.hardware.sensors@1.0-impl-mediatek \
    android.hardware.sensors@1.0-service-mediatek

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

# Wi-Fi
PRODUCT_PACKAGES += \
    vendor.mediatek.hardware.wifi.supplicant@2.0.vendor \
    vendor.mediatek.hardware.wifi.supplicant@2.1.vendor
