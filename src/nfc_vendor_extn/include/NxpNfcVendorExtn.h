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
#include "NfcVendorExtn.h"

/**
 * @brief Holds NCI packet data length and data buffer
 *
 */
typedef struct {
  uint16_t* data_len;
  uint8_t* p_data;
} NciData_t;

/**
 * @brief Holds functional event datas to support
 *        extension features
 */
typedef union {
  NciData_t nci_msg;
  NciData_t nci_rsp_ntf;
  uint8_t write_status;
  uint8_t hal_state;
  uint8_t rf_state;
  uint8_t hal_event;
  uint8_t hal_event_status;
} NfcExtEventData_t;

/**
 * @brief Holds functional event codes to support
 *        extension features.
 */
typedef enum {
  HANDLE_VENDOR_NCI_MSG,
  HANDLE_VENDOR_NCI_RSP_NTF,
  HANDLE_WRITE_COMPLETE_STATUS,
  HANDLE_HAL_CONTROL_GRANTED,
  HANDLE_NFC_HAL_STATE_UPDATE,
  HANDLE_RF_HAL_STATE_UPDATE,
  HANDLE_HAL_EVENT,
  HANDLE_FW_DNLD_STATUS_UPDATE,
} NfcExtEvent_t;

typedef enum {
  NFCC_HAL_TRANS_ERR_CODE = 6u,
  NFCC_HAL_FATAL_ERR_CODE = 8u,
} NfcExtHal_NFCC_ERROR_CODE_t;
