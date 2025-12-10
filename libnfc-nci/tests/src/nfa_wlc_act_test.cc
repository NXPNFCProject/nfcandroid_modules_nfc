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

#include "nfa_dm_int.h"
#include "nfa_rw_int.h"
#include "nfa_wlc_api.h"
#include "nfa_wlc_int.h"
#include "nfc_api.h"
#include "nfc_int.h"

extern tNFC_CB nfc_cb;
extern tNFA_WLC_CB nfa_wlc_cb;
extern tNFA_DM_CB nfa_dm_cb;
extern tNFA_RW_CB nfa_rw_cb;

static void mock_callback(__attribute__((unused)) tNFA_WLC_EVT event,
                          __attribute__((unused)) tNFA_WLC_EVT_DATA* p_data) {}

TEST(NfaWlcActTest, WlcEnable) {
  tNFA_WLC_MSG p_data;
  p_data.enable.p_wlc_cback = mock_callback;
  EXPECT_TRUE(nfa_wlc_enable(&p_data));
}

TEST(NfaWlcActTest, WlcStart) {
  tNFA_WLC_MSG p_data;
  p_data.start.mode = NFA_WLC_AUTONOMOUS;
  EXPECT_FALSE(nfa_wlc_start(&p_data));
  p_data.start.mode = NFA_WLC_NON_AUTONOMOUS;
  EXPECT_FALSE(nfa_wlc_start(&p_data));
  nfc_cb.nci_features = NCI_POLL_REMOVAL_DETECTION;
  nfa_wlc_cb.flags = NFA_WLC_FLAGS_NON_AUTO_MODE_ENABLED;
  EXPECT_FALSE(nfa_wlc_start(&p_data));
  nfa_wlc_cb.flags &= ~NFA_WLC_FLAGS_NON_AUTO_MODE_ENABLED;
  nfa_dm_cb.disc_cb.disc_state = NFA_DM_RFST_LISTEN_ACTIVE;
  EXPECT_FALSE(nfa_wlc_start(&p_data));
  nfa_dm_cb.disc_cb.disc_state = NFA_DM_RFST_POLL_ACTIVE;
  nfa_rw_cb.protocol = NFC_PROTOCOL_UNKNOWN;
  EXPECT_FALSE(nfa_wlc_start(&p_data));
  nfa_rw_cb.protocol = NFA_PROTOCOL_ISO_DEP;
  nfa_rw_cb.flags = NFA_RW_FL_API_BUSY;
  EXPECT_FALSE(nfa_wlc_start(&p_data));
  nfa_rw_cb.flags &= ~NFA_RW_FL_API_BUSY;
  nfa_dm_cb.disc_cb.disc_flags = NFA_DM_DISC_FLAGS_DISABLING;
  EXPECT_FALSE(nfa_wlc_start(&p_data));
  nfa_dm_cb.disc_cb.disc_flags &=
      ~(NFA_DM_DISC_FLAGS_W4_RSP | NFA_DM_DISC_FLAGS_W4_NTF |
        NFA_DM_DISC_FLAGS_STOPPING | NFA_DM_DISC_FLAGS_DISABLING);
  EXPECT_TRUE(nfa_wlc_start(&p_data));
}

TEST(NfaWlcActTest, WlcNonAutoStartWpt) {
  tNFA_WLC_MSG p_data;
  EXPECT_TRUE(nfa_wlc_non_auto_start_wpt(&p_data));
}