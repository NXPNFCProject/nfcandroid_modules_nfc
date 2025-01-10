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
#pragma once

#include <aidl/android/hardware/nfc/INfc.h>
#include <android/hardware/nfc/1.1/INfc.h>
#include <android/hardware/nfc/1.2/INfc.h>

#include "nfc_api.h"
#include "nfc_hal_api.h"

using android::sp;
using android::hardware::nfc::V1_0::INfc;
using INfcAidl = ::aidl::android::hardware::nfc::INfc;
using NfcAidlConfig = ::aidl::android::hardware::nfc::NfcConfig;
using NfcVendorConfigV1_2 = android::hardware::nfc::V1_2::NfcConfig;

// This is only intended for a limited time to handle non-AOSP vendor interface
// implementations on existing upgrading devices and not as a new extension point.
// This will be removed once all devices are upgraded to the latest NFC HAL.
/**
 * @brief Vendor extension control block holds below data's
 *        hidlHal - reference to HIDL Hal instance
 *        aidlHal - reference to AIDL Hal instance
 *        pHalCback - reference to HAL events callback
 *        pDataCallback - reference to NCI response and notification packets
 *
 */
struct VendorExtnCb {
  sp<INfc> hidlHal;
  std::shared_ptr<INfcAidl> aidlHal;
  tHAL_NFC_CBACK* pHalCback;
  tHAL_NFC_DATA_CBACK* pDataCback;
};

/**
 * @brief Holds the vendor extension config
 *
 */
struct VendorExtnConfig {
  NfcAidlConfig* aidlVendorConfig;
  NfcVendorConfigV1_2* hidlVendorConfig;
};

class NfcVendorExtn {
 public:
  /**
   * @brief Get the singleton of this object.
   * @return Reference to this object.
   *
   */
  static NfcVendorExtn* getInstance();

  /**
   * @brief This function sets up and initialize the extension feature
   * @param vendorExtnCb
   * @return true if init is success else false
   *
   */
  bool Initialize(VendorExtnCb vendorExtnCb);

  /**
   * @brief sends the NCI packet to handle extension feature
   * @param  dataLen length of the NCI packet
   * @param  pData data buffer pointer
   * @return returns true if it is vendor specific feature,
   * and handled only by extension library otherwise returns
   * false and it have to be handled by libnfc.
   *
   */
  bool processCmd(uint16_t dataLen, uint8_t* pData);

  /**
   * @brief sends the NCI packet to handle extension feature
   * @param  dataLen length of the NCI packet
   * @param  pData data buffer pointer
   * @return returns true if it is vendor specific feature,
   * and handled only by extension library otherwise returns
   * false and it have to be handled by libnfc.
   *
   */
  bool processRspNtf(uint16_t dataLen, uint8_t* pData);

  /**
   * @brief sends the NCI packet to handle extension feature
   * @param  event
   * @param  status
   * @return returns true if it is vendor specific feature,
   * and handled only by extension library otherwise returns
   * false and it have to be handled by libnfc.
   *
   */
  bool processEvent(uint8_t event, tHAL_NFC_STATUS status);

  /**
   * @brief Loads the Nfc Vendor Config
   * @return
   *
   */
  void getVendorConfigs(VendorExtnConfig vndExtConfig);

  /**
   * @brief This function de-initializes the extension feature
   * @return void
   *
   */
  bool finalize();

 private:
  VendorExtnCb mVendorExtnCb;
  VendorExtnConfig mVendorExtnConfig;

  NfcVendorExtn();

  ~NfcVendorExtn();
};
