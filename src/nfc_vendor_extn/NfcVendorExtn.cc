/*
 * Copyright 2024, The Android Open Source Project
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

#include "NfcVendorExtn.h"

#include <android-base/logging.h>
#include <android-base/stringprintf.h>
#include <android/log.h>
#include <dlfcn.h>
#include <error.h>
#include <log/log.h>

using android::base::StringPrintf;
#define UNUSED_PROP(X) (void)(X);

std::string mLibName = "libnfc_vendor_extn.so";
std::string mLibPathName = "/system/lib64/" + mLibName;

static NfcVendorExtn* sNfcVendorExtn;

void* p_oem_extn_handle = NULL;

NfcVendorExtn::NfcVendorExtn() {}

NfcVendorExtn::~NfcVendorExtn() { sNfcVendorExtn = nullptr; }

NfcVendorExtn* NfcVendorExtn::getInstance() {
  if (sNfcVendorExtn == nullptr) {
    sNfcVendorExtn = new NfcVendorExtn();
  }
  return sNfcVendorExtn;
}

bool phNfcExtn_LibSetup() {
  LOG(VERBOSE) << StringPrintf("%s Enter", __func__);
  p_oem_extn_handle = dlopen(mLibPathName.c_str(), RTLD_NOW);
  if (p_oem_extn_handle == NULL) {
    LOG(DEBUG) << StringPrintf(
        "%s Error : opening (%s) !! dlerror: "
        "%s",
        __func__, mLibPathName.c_str(), dlerror());
    return false;
  }
  return true;
}

bool NfcVendorExtn::Initialize(sp<INfc> hidlHal,
                               std::shared_ptr<INfcAidl> aidlHal) {
  LOG(VERBOSE) << StringPrintf("%s:", __func__);
  mVendorExtnCb.hidlHal = hidlHal;
  mVendorExtnCb.aidlHal = aidlHal;
  return phNfcExtn_LibSetup();
}

void NfcVendorExtn::setNciCallback(tHAL_NFC_CBACK* pHalCback,
                                   tHAL_NFC_DATA_CBACK* pDataCback) {
  LOG(VERBOSE) << StringPrintf("%s:", __func__);
  mVendorExtnCb.pHalCback = pHalCback;
  mVendorExtnCb.pDataCback = pDataCback;
}

bool NfcVendorExtn::processCmd(uint16_t dataLen, uint8_t* pData) {
  LOG(VERBOSE) << StringPrintf("%s: Enter dataLen:%d", __func__, dataLen);
  UNUSED_PROP(pData);
  return true;
}

bool NfcVendorExtn::processRspNtf(uint16_t dataLen, uint8_t* pData) {
  LOG(VERBOSE) << StringPrintf("%s: Enter dataLen:%d", __func__, dataLen);
  UNUSED_PROP(pData);
  return true;
}

bool NfcVendorExtn::processEvent(uint8_t event, uint8_t status) {
  LOG(VERBOSE) << StringPrintf("%s: Enter event: %d, status: %d", __func__,
                               event, status);
  return true;
}

void NfcVendorExtn::getVendorConfigs(
    std::map<std::string, ConfigValue>* pConfigMap) {
  LOG(VERBOSE) << StringPrintf("%s:", __func__);
  mVendorExtnCb.configMap = *pConfigMap;
}

VendorExtnCb* NfcVendorExtn::getVendorExtnCb() { return &mVendorExtnCb; }

bool NfcVendorExtn::finalize(void) {
  LOG(VERBOSE) << StringPrintf("%s:", __func__);
  return true;
}
