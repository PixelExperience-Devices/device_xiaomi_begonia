#
# Copyright (C) 2019 The LineageOS Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

# Audio
PRODUCT_SYSTEM_DEFAULT_PROPERTIES += \
    ro.camera.sound.forced=0 \
    ro.audio.silent=0 \
    aaudio.mmap_policy=2 \
    aaudio.mmap_exclusive_policy=2 \
    ro.vendor.audio.soundfx.type=mi \
    ro.vendor.audio.soundfx.usb=true \
    ro.vendor.audio.voice.volume.boost=manual \
    ro.vendor.audio.sos=true \
    ro.vendor.audio.game.effect=true \
    ro.vendor.audio.soundtrigger=mtk \
    ro.vendor.audio.soundtrigger.lowpower=true \
    ro.config.vc_call_vol_steps=7

# Display
PRODUCT_SYSTEM_DEFAULT_PROPERTIES += \
    ro.opengles.version=196610 \
    ro.zygote.preload.enable=0 \
    qemu.hw.mainkeys=0 \
    sys.ipo.disable=1 \
    sys.displayfeature_hidl=true \
    ro.whitepoint_calibration_enable=false \
    ro.displayfeature.histogram.enable=false \
    ro.xiaomi.bl.poll=true \
    ro.colorpick_adjust=true \
    sys.displayfeature.entry.enable=true

# MCD
PRODUCT_SYSTEM_DEFAULT_PROPERTIES += \
    persist.sys.mcd_config_file=/system/etc/mcd_default.conf \
    persist.sys.mdlog_dumpback=2

# Memory optimizations
PRODUCT_SYSTEM_DEFAULT_PROPERTIES += \
    ro.sys.fw.bservice_enable=true \
    ro.sys.fw.bservice_limit=5 \
    ro.sys.fw.bservice_age=5000 \
    ro.am.reschedule_service=true \
    ro.sys.fw.bg_apps_limit=24

# OMX
PRODUCT_SYSTEM_DEFAULT_PROPERTIES += \
    debug.stagefright.omx_default_rank.sw-audio=1 \
    debug.stagefright.omx_default_rank=0 \
    debug.stagefright.ccodec=0

# Perf
PRODUCT_SYSTEM_DEFAULT_PROPERTIES += \
    ro.kernel.zio=3810810516 \
    ro.mtk_perf_simple_start_win=1 \
    ro.mtk_perf_fast_start_win=1 \
    ro.mtk_perf_response_time=1

# RIL
PRODUCT_SYSTEM_DEFAULT_PROPERTIES += \
    vendor.rild.libpath=mtk-ril.so \
    vendor.rild.libargs=-d /dev/ttyC0 \
    persist.radio.sar_sar_sensor=true

# USB
PRODUCT_SYSTEM_DEFAULT_PROPERTIES += \
    ro.sys.usb.mtp.whql.enable=0 \
    sys.ipo.pwrdncap=2 \
    ro.sys.usb.storage.type=mtp \
    ro.sys.usb.bicr=no \
    ro.sys.usb.charging.only=yes

# WLAN
PRODUCT_SYSTEM_DEFAULT_PROPERTIES += \
    wifi.interface=wlan0 \
    ro.mediatek.wlan.wsc=1 \
    ro.mediatek.wlan.p2p=1 \
    mediatek.wlan.ctia=0 \
    wifi.tethering.interface=ap0 \
    wifi.direct.interface=p2p0
