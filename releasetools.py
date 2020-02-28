# Copyright (C) 2009 The Android Open Source Project
# Copyright (C) 2019 The Mokee Open Source Project
# Copyright (C) 2019 The LineageOS Open Source Project
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

import common
import re

def FullOTA_InstallEnd(info):
  OTA_InstallEnd(info)

def IncrementalOTA_InstallEnd(info):
  OTA_InstallEnd(info)

def AddImage(info, basename, dest):
  name = basename
  data = info.input_zip.read("IMAGES/" + basename)
  common.ZipWriteStr(info.output_zip, name, data)
  info.script.AppendExtra('package_extract_file("%s", "%s");' % (name, dest))

def OTA_InstallEnd(info):
  PatchVendor(info)
  info.script.Print("Patching firmware images...")
  AddImage(info, "vbmeta.img", "/dev/block/by-name/vbmeta")
  AddImage(info, "dtbo.img", "/dev/block/by-name/dtbo")

def PatchVendor(info):
  info.script.Print("Patching vendor init scripts...")
  info.script.AppendExtra('mount("ext4", "EMMC", "/dev/block/platform/bootdevice/by-name/vendor", "/vendor");')
  info.script.AppendExtra('run_program("/sbin/sed", "-i", "s/wait,check,formattable,quota,resize/latemount,wait,check,formattable,quota/", "/vendor/etc/fstab.mt6785");')
  info.script.AppendExtra('run_program("/sbin/sed", "-i", "s/fstab.mt6785$/fstab.mt6785 --early\\n    mount_all \/vendor\/etc\/fstab.mt6785 --late/", "/vendor/etc/init/hw/init.mt6785.rc");')
  info.script.AppendExtra('unmount("/vendor");')
