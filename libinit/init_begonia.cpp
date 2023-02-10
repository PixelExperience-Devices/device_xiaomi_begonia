/*
 * Copyright (C) 2023 PixelExperience
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <vector>

#include <android-base/logging.h>
#include <android-base/properties.h>
#define _REALLY_INCLUDE_SYS__SYSTEM_PROPERTIES_H_
#include <sys/_system_properties.h>
#include <sys/sysinfo.h>

using android::base::GetProperty;

void property_override(char const prop[], char const value[], bool add = true) {
    prop_info* pi;

    pi = (prop_info*)__system_property_find(prop);
    if (pi)
        __system_property_update(pi, value, strlen(value));
    else if (add)
        __system_property_add(prop, strlen(prop), value, strlen(value));
}

void load_begonia() {
    property_override("bluetooth.device.default_name", "Redmi Note 8 Pro");
    property_override("ro.product.brand", "Redmi");
    property_override("ro.product.device", "begonia");
    property_override("ro.product.manufacturer", "Xiaomi");
    property_override("ro.product.model", "M1906G7G");
    property_override("ro.product.name", "begonia");
    property_override("ro.build.fingerprint", "Redmi/begonia_eea/begonia:11/RP1A.200720.011/V12.5.15.0.RGGEUXM:user/release-keys");
    property_override("ro.vendor.build.fingerprint", "Redmi/begonia_eea/begonia:11/RP1A.200720.011/V12.5.15.0.RGGEUXM:user/release-keys");
    property_override("ro.bootimage.build.fingerprint", "Redmi/begonia_eea/begonia:11/RP1A.200720.011/V12.5.15.0.RGGEUXM:user/release-keys");
    property_override("ro.build.description", "Redmi/begonia_eea/begonia:11/RP1A.200720.011/V12.5.15.0.RGGEUXM:user/release-keys");
}

void vendor_load_properties() {
    std::string region = GetProperty("ro.boot.hwc", "");
    if (access("/system/bin/recovery", F_OK) != 0) {
        load_begonia();
    }
}
