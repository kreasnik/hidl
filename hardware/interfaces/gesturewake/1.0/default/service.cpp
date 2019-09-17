/*
 * Copyright (C) 2017 The Android Open Source Project
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

#define LOG_TAG "vendor.hct.hardware.gesturewaket@1.0-service"

#include <android/log.h>
#include <hidl/HidlSupport.h>
#include <hidl/HidlTransportSupport.h>
#include <vendor/hct/hardware/gesturewake/1.0/IGestureWake.h>
#include <vendor/hct/hardware/gesturewake/1.0/IGestureWakeCallback.h>
#include "GestureWake.h"

using vendor::hct::hardware::gesturewake::V1_0::IGestureWake;
using vendor::hct::hardware::gesturewake::V1_0::IGestureWakeCallback;
using vendor::hct::hardware::gesturewake::V1_0::implementation::GestureWake;
using android::hardware::configureRpcThreadpool;
using android::hardware::joinRpcThreadpool;
using android::sp;

int main() {
    android::sp<IGestureWake> bio = GestureWake::getInstance();

    configureRpcThreadpool(1, true /*callerWillJoin*/);

    if (bio != nullptr) {
        bio->registerAsService();
    } else {
        ALOGE("Can't create instance of BiometricsFingerprint, nullptr");
    }

    joinRpcThreadpool();

    return 0; // should never get here
}
