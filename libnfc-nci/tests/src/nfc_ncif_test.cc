//
// Copyright (C) 2025 The Android Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "mock_gki_utils.h"
#include "nfc_int.h"

class NfcNcifTest : public ::testing::Test {
 protected:
  void SetUp() override {}

  void TearDown() override {}
};

TEST_F(NfcNcifTest, NfcModeSetNtfTimeout) {
  nfc_cb.p_resp_cback = [](tNFC_RESPONSE_EVT event, tNFC_RESPONSE* p_response) {
    ASSERT_EQ(p_response->mode_set.status, NCI_STATUS_FAILED);
    ASSERT_EQ(p_response->mode_set.nfcee_id, *nfc_cb.last_nfcee_cmd);
    ASSERT_EQ(p_response->mode_set.mode, NCI_NFCEE_MD_DEACTIVATE);
    ASSERT_EQ(event, NFC_NFCEE_MODE_SET_REVT);
  };

  nfc_mode_set_ntf_timeout();
}

TEST_F(NfcNcifTest, ProcActivateValidPacketIsoDepPollASuccess) {
  uint8_t packet[] = {
      0x01,                       // RF Disc ID
      NCI_INTERFACE_ISO_DEP,      // Interface Type
      NCI_PROTOCOL_18092_ACTIVE,  // Protocol
      NCI_DISCOVERY_TYPE_POLL_A,  // Mode
      0x02,                       // Buff Size
      0x03,                       // Num Buff
      0x01,                       // RF Param Length
      0x05,                       // RF Parameter
      0x01,                       // Data Mode
      0x02,                       // TX Bitrate
      0x03,                       // RX Bitrate
      0x04,                       // Length of activation parameters
      0x0A,                       // ATS RES Length
      0x01,
      0x02,
      0x03,
      0x04,
      0x05,
      0x06,
      0x07,
      0x08,
      0x09,
      0x0A  // ATS RES
  };
  uint8_t packet_len = sizeof(packet);

  nfc_cb.p_discv_cback = [](tNFC_DISCOVER_EVT event, tNFC_DISCOVER* p_data) {
    ASSERT_EQ(p_data->activate.intf_param.type, NCI_INTERFACE_ISO_DEP);
    ASSERT_EQ(p_data->activate.protocol, NCI_PROTOCOL_NFC_DEP);
    ASSERT_EQ(p_data->activate.rf_tech_param.mode, NCI_DISCOVERY_TYPE_POLL_A);
    ASSERT_EQ(p_data->activate.rf_disc_id, 0x01);
    ASSERT_EQ(p_data->activate.data_mode, 0x01);
    ASSERT_EQ(p_data->activate.tx_bitrate, 0x02);
    ASSERT_EQ(p_data->activate.rx_bitrate, 0x03);
    ASSERT_EQ(event, NFC_ACTIVATE_DEVT);
    ASSERT_EQ(p_data->activate.intf_param.intf_param.pa_iso.ats_res_len, 0x0A);
    ASSERT_EQ(p_data->activate.intf_param.intf_param.pa_iso.ats_res[0], 0x01);
    ASSERT_EQ(p_data->activate.intf_param.intf_param.pa_iso.ats_res[9], 0x0A);
  };

  nfc_ncif_proc_activate(packet, packet_len);
}

TEST_F(NfcNcifTest, ProcActivateInvalidPacketLength) {
  uint8_t packet[] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06};  // Short packet
  uint8_t packet_len = sizeof(packet);

  nfc_cb.p_discv_cback = [](tNFC_DISCOVER_EVT event, tNFC_DISCOVER* p_data) {
    ASSERT_EQ(p_data->status, NCI_STATUS_FAILED);
    ASSERT_EQ(event, NFC_ACTIVATE_DEVT);
  };

  nfc_ncif_proc_activate(packet, packet_len);
}

TEST_F(NfcNcifTest, ProcActivateIsoDepListenASuccess) {
  uint8_t packet[] = {
      0x01,                         // RF Disc ID
      NCI_INTERFACE_ISO_DEP,        // Interface Type
      NCI_PROTOCOL_18092_ACTIVE,    // Protocol
      NCI_DISCOVERY_TYPE_LISTEN_A,  // Mode
      0x02,                         // Buff Size
      0x03,                         // Num Buff
      0x01,                         // RF Param Length
      0x05,                         // RF Parameter
      0x01,                         // Data Mode
      0x02,                         // TX Bitrate
      0x03,                         // RX Bitrate
      0x01,                         // activation parameter length
      0x04                          // RATS value
  };
  uint8_t packet_len = sizeof(packet);

  nfc_cb.p_discv_cback = [](tNFC_DISCOVER_EVT event, tNFC_DISCOVER* p_data) {
    ASSERT_EQ(p_data->activate.intf_param.type, NCI_INTERFACE_ISO_DEP);
    ASSERT_EQ(p_data->activate.protocol, NCI_PROTOCOL_NFC_DEP);
    ASSERT_EQ(p_data->activate.rf_tech_param.mode, NCI_DISCOVERY_TYPE_LISTEN_A);
    ASSERT_EQ(p_data->activate.intf_param.intf_param.la_iso.rats, 0x04);
    ASSERT_EQ(event, NFC_ACTIVATE_DEVT);
  };

  nfc_ncif_proc_activate(packet, packet_len);
}

TEST_F(NfcNcifTest, ProcActivateIsoDepPollBSuccess) {
  uint8_t packet[] = {
      0x01,                       // RF Disc ID
      NCI_INTERFACE_ISO_DEP,      // Interface Type
      NCI_PROTOCOL_18092_ACTIVE,  // Protocol
      NCI_DISCOVERY_TYPE_POLL_B,  // Mode
      0x02,                       // Buff Size
      0x03,                       // Num Buff
      0x01,                       // RF Param Length
      0x05,                       // RF Parameter
      0x01,                       // Data Mode
      0x02,                       // TX Bitrate
      0x03,                       // RX Bitrate
      0x04,                       // activation parameter length
      0x05,                       // ATTRIB RES length
      0x0A,
      0x0B,
      0x0C,
      0x0D,
      0x0E  // ATTRIB RES
  };
  uint8_t packet_len = sizeof(packet);

  nfc_cb.p_discv_cback = [](tNFC_DISCOVER_EVT event, tNFC_DISCOVER* p_data) {
    ASSERT_EQ(p_data->activate.intf_param.type, NCI_INTERFACE_ISO_DEP);
    ASSERT_EQ(p_data->activate.protocol, NCI_PROTOCOL_NFC_DEP);
    ASSERT_EQ(p_data->activate.rf_tech_param.mode, NCI_DISCOVERY_TYPE_POLL_B);
    ASSERT_EQ(p_data->activate.intf_param.intf_param.pb_iso.attrib_res_len,
              0x05);
    ASSERT_EQ(p_data->activate.intf_param.intf_param.pb_iso.attrib_res[0],
              0x0A);
    ASSERT_EQ(p_data->activate.intf_param.intf_param.pb_iso.attrib_res[4],
              0x0E);
    ASSERT_EQ(event, NFC_ACTIVATE_DEVT);
  };

  nfc_ncif_proc_activate(packet, packet_len);
}

TEST_F(NfcNcifTest, ProcActivateIsoDepListenBSuccess) {
  uint8_t packet[] = {
      0x01,                         // RF Disc ID
      NCI_INTERFACE_ISO_DEP,        // Interface Type
      NCI_PROTOCOL_18092_ACTIVE,    // Protocol
      NCI_DISCOVERY_TYPE_LISTEN_B,  // Mode
      0x02,                         // Buff Size
      0x03,                         // Num Buff
      0x01,                         // RF Param Length
      0x05,                         // RF Parameter
      0x01,                         // Data Mode
      0x02,                         // TX Bitrate
      0x03,                         // RX Bitrate
      0x0A,                         // activation parameter length
      0x09,                         // ATTRIB REQ length
      0x01,
      0x02,
      0x03,
      0x04,
      0x05,
      0x06,
      0x07,
      0x08,
      0x09  // ATTRIB REQ
  };
  uint8_t packet_len = sizeof(packet);

  nfc_cb.p_discv_cback = [](tNFC_DISCOVER_EVT event, tNFC_DISCOVER* p_data) {
    ASSERT_EQ(p_data->activate.intf_param.type, NCI_INTERFACE_ISO_DEP);
    ASSERT_EQ(p_data->activate.protocol, NCI_PROTOCOL_NFC_DEP);
    ASSERT_EQ(p_data->activate.rf_tech_param.mode, NCI_DISCOVERY_TYPE_LISTEN_B);
    ASSERT_EQ(p_data->activate.intf_param.intf_param.lb_iso.attrib_req_len,
              0x09);
    for (int i = 0; i < 9; i++)
      ASSERT_EQ(p_data->activate.intf_param.intf_param.lb_iso.attrib_req[i],
                i + 1);
    ASSERT_EQ(p_data->activate.intf_param.intf_param.lb_iso.nfcid0[0], 0x01);
    ASSERT_EQ(p_data->activate.intf_param.intf_param.lb_iso.nfcid0[3], 0x04);
    ASSERT_EQ(event, NFC_ACTIVATE_DEVT);
  };

  nfc_ncif_proc_activate(packet, packet_len);
}

TEST_F(NfcNcifTest, ProcActivateT1TSuccess) {
  uint8_t packet[] = {
      0x01,                       // RF Disc ID
      NCI_INTERFACE_FRAME,        // Interface Type
      NCI_PROTOCOL_T1T,           // Protocol
      NCI_DISCOVERY_TYPE_POLL_A,  // Mode (this is just an example - not a real
                                  // T1T mode)
      0x02,                       // Buff Size
      0x03,                       // Num Buff
      0x01,                       // RF Param Length
      0x05,                       // RF Parameter
      0x01,                       // Data Mode
      0x02,                       // TX Bitrate
      0x03,                       // RX Bitrate
      0x02,                       // activation parameter length
      0x0A,
      0x0B  // HR
  };
  uint8_t packet_len = sizeof(packet);

  nfc_cb.p_discv_cback = [](tNFC_DISCOVER_EVT event, tNFC_DISCOVER* p_data) {
    ASSERT_EQ(p_data->activate.intf_param.type, NCI_INTERFACE_FRAME);
    ASSERT_EQ(p_data->activate.protocol, NCI_PROTOCOL_T1T);
    ASSERT_EQ(p_data->activate.rf_tech_param.param.pa.hr_len, 0x02);
    ASSERT_EQ(p_data->activate.rf_tech_param.param.pa.hr[0], 0x0A);
    ASSERT_EQ(p_data->activate.rf_tech_param.param.pa.hr[1], 0x0B);
    ASSERT_EQ(event, NFC_ACTIVATE_DEVT);
  };

  nfc_ncif_proc_activate(packet, packet_len);
}

TEST_F(NfcNcifTest, ProcDiscoverNtfValidPacketSuccess) {
  // Arrange
  uint8_t packet[] = {
      0x00, 0x01, 0x02,  // NCI Header
      0x01,              // RF Disc ID
      0x02,              // Protocol
      0x03,              // Mode
      0x01,              // RF Param Length
      0x05,              // RF Parameter (example)
      0x01, 0x02         // More flag
  };
  uint16_t packet_len = sizeof(packet);

  nfc_cb.p_discv_cback = [](tNFC_DISCOVER_EVT event, tNFC_DISCOVER* p_data) {
    ASSERT_EQ(p_data->result.rf_disc_id, 0x01);
    ASSERT_EQ(p_data->result.protocol, 0x02);
    ASSERT_EQ(p_data->result.rf_tech_param.mode, 0x03);
    ASSERT_EQ(p_data->result.more, 0x01);
    ASSERT_EQ(event, NFC_RESULT_DEVT);
  };

  nfc_ncif_proc_discover_ntf(packet, packet_len);
}

TEST_F(NfcNcifTest, ProcDiscoverNtfInvalidPacketLength) {
  uint8_t packet[] = {0x00, 0x01, 0x02, 0x03, 0x01};  // Short packet
  uint16_t packet_len = sizeof(packet);

  nfc_cb.p_discv_cback = [](tNFC_DISCOVER_EVT event, tNFC_DISCOVER* p_data) {
    ASSERT_EQ(p_data->status, NCI_STATUS_FAILED);
    ASSERT_EQ(event, NFC_RESULT_DEVT);
  };

  nfc_ncif_proc_discover_ntf(packet, packet_len);
}

TEST_F(NfcNcifTest, ProcDiscoverNtfInvalidRFParameterLength) {
  uint8_t packet[] = {
      0x00, 0x01, 0x02,  // NCI Header
      0x01,              // RF Disc ID
      0x02,              // Protocol
      0x03,              // Mode
      0x01,              // RF Param Length
                         // Missing RF parameters
  };
  uint16_t packet_len = sizeof(packet);

  nfc_cb.p_discv_cback = [](tNFC_DISCOVER_EVT event, tNFC_DISCOVER* p_data) {
    ASSERT_EQ(p_data->status, NCI_STATUS_FAILED);
    ASSERT_EQ(event, NFC_RESULT_DEVT);
  };

  nfc_ncif_proc_discover_ntf(packet, packet_len);
}

TEST_F(NfcNcifTest, ProcEEShortPacketFailure) {
  uint8_t packet[] = {0x01, 0x02, 0x03};
  uint16_t packet_len = sizeof(packet);
  nfc_cb.p_resp_cback = [](tNFC_RESPONSE_EVT event, tNFC_RESPONSE* p_response) {
    ASSERT_EQ(p_response->ee_action.status, NFC_STATUS_FAILED);
    ASSERT_EQ(p_response->ee_action.nfcee_id, 0);
    ASSERT_EQ(event, NFC_EE_ACTION_REVT);
  };

  nfc_ncif_proc_ee_action(packet, packet_len);
}

TEST_F(NfcNcifTest, ProcEEValidPacket7816SelectSuccess) {
  uint8_t packet[] = {
      0x01,                     // NFCEE ID
      NCI_EE_TRIG_7816_SELECT,  // Trigger
      0x03,                     // Data length
      0x01,
      0x02,
      0x03  // AID
  };
  uint16_t packet_len = sizeof(packet);

  nfc_cb.p_resp_cback = [](tNFC_RESPONSE_EVT event, tNFC_RESPONSE* p_response) {
    ASSERT_EQ(p_response->ee_action.status, NFC_STATUS_OK);
    ASSERT_EQ(p_response->ee_action.nfcee_id, 0x01);
    ASSERT_EQ(p_response->ee_action.act_data.trigger, NCI_EE_TRIG_7816_SELECT);
    ASSERT_EQ(p_response->ee_action.act_data.param.aid.len_aid, 0x03);
    ASSERT_EQ(p_response->ee_action.act_data.param.aid.aid[0], 0x01);
    ASSERT_EQ(p_response->ee_action.act_data.param.aid.aid[1], 0x02);
    ASSERT_EQ(p_response->ee_action.act_data.param.aid.aid[2], 0x03);
    ASSERT_EQ(event, NFC_EE_ACTION_REVT);
  };

  nfc_ncif_proc_ee_action(packet, packet_len);
}

TEST_F(NfcNcifTest, ProcEEValidPacketRFProtocolSuccess) {
  uint8_t packet[] = {
      0x02,                     // NFCEE ID
      NCI_EE_TRIG_RF_PROTOCOL,  // Trigger
      0x01,                     // Data length
      0x03,                     // Protocol
  };
  uint16_t packet_len = sizeof(packet);

  nfc_cb.p_resp_cback = [](tNFC_RESPONSE_EVT event, tNFC_RESPONSE* p_response) {
    ASSERT_EQ(p_response->ee_action.status, NFC_STATUS_OK);
    ASSERT_EQ(p_response->ee_action.nfcee_id, 0x02);
    ASSERT_EQ(p_response->ee_action.act_data.trigger, NCI_EE_TRIG_RF_PROTOCOL);
    ASSERT_EQ(p_response->ee_action.act_data.param.protocol, 0x03);
    ASSERT_EQ(event, NFC_EE_ACTION_REVT);
  };

  nfc_ncif_proc_ee_action(packet, packet_len);
}

TEST_F(NfcNcifTest, ProcEEValidPacketRFTechnologySuccess) {
  uint8_t packet[] = {
      0x03,                       // NFCEE ID
      NCI_EE_TRIG_RF_TECHNOLOGY,  // Trigger
      0x01,                       // Data length
      0x04                        // Technology
  };
  uint16_t packet_len = sizeof(packet);

  nfc_cb.p_resp_cback = [](tNFC_RESPONSE_EVT event, tNFC_RESPONSE* p_response) {
    ASSERT_EQ(p_response->ee_action.status, NFC_STATUS_OK);
    ASSERT_EQ(p_response->ee_action.nfcee_id, 0x03);
    ASSERT_EQ(p_response->ee_action.act_data.trigger,
              NCI_EE_TRIG_RF_TECHNOLOGY);
    ASSERT_EQ(p_response->ee_action.act_data.param.technology, 0x04);
    ASSERT_EQ(event, NFC_EE_ACTION_REVT);
  };

  nfc_ncif_proc_ee_action(packet, packet_len);
}

TEST_F(NfcNcifTest, ValidPacketAppInitSuccess) {
  uint8_t packet[] = {
      0x04,                  // NFCEE ID
      NCI_EE_TRIG_APP_INIT,  // Trigger
      0x0B,                  // Data length
      NCI_EE_ACT_TAG_AID,    // Tag
      0x03,                  // Length
      0x01,
      0x02,
      0x03,                 // AID
      NCI_EE_ACT_TAG_DATA,  // Tag
      0x04,                 // Length
      0x0A,
      0x0B,
      0x0C,
      0x0D  // Data
  };
  uint16_t packet_len = sizeof(packet);

  nfc_cb.p_resp_cback = [](tNFC_RESPONSE_EVT event, tNFC_RESPONSE* p_response) {
    ASSERT_EQ(p_response->ee_action.status, NFC_STATUS_OK);
    ASSERT_EQ(p_response->ee_action.nfcee_id, 0x04);
    ASSERT_EQ(p_response->ee_action.act_data.trigger, NCI_EE_TRIG_APP_INIT);
    ASSERT_EQ(p_response->ee_action.act_data.param.app_init.len_aid, 0x03);
    ASSERT_EQ(p_response->ee_action.act_data.param.app_init.aid[0], 0x01);
    ASSERT_EQ(p_response->ee_action.act_data.param.app_init.aid[1], 0x02);
    ASSERT_EQ(p_response->ee_action.act_data.param.app_init.aid[2], 0x03);
    ASSERT_EQ(p_response->ee_action.act_data.param.app_init.len_data, 0x04);
    ASSERT_EQ(p_response->ee_action.act_data.param.app_init.data[0], 0x0A);
    ASSERT_EQ(p_response->ee_action.act_data.param.app_init.data[1], 0x0B);
    ASSERT_EQ(p_response->ee_action.act_data.param.app_init.data[2], 0x0C);
    ASSERT_EQ(p_response->ee_action.act_data.param.app_init.data[3], 0x0D);
    ASSERT_EQ(event, NFC_EE_ACTION_REVT);
  };

  nfc_ncif_proc_ee_action(packet, packet_len);
}