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
#include "nfa_wlc_main.cc"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "nfa_wlc_api.h"
#include "nfa_wlc_int.h"

static void mock_callback(__attribute__((unused)) tNFA_WLC_EVT event,
                          __attribute__((unused)) tNFA_WLC_EVT_DATA* p_data) {}

class NfaWlcMainTest : public ::testing::Test {
 protected:
  void SetUp() override { nfa_wlc_init(); }
};

TEST_F(NfaWlcMainTest, EventNotify) {
  tNFA_WLC_EVT_DATA wlc_cback_data;
  nfa_wlc_cb.p_wlc_cback = mock_callback;
  nfa_wlc_event_notify(NFA_WLC_START_RESULT_EVT, &wlc_cback_data);
}
TEST_F(NfaWlcMainTest, EventHandle) {
  NFC_HDR p_msg;
  p_msg.event = 0xFF;
  EXPECT_TRUE(nfa_wlc_handle_event(&p_msg));
}
TEST_F(NfaWlcMainTest, Disable) { nfa_wlc_sys_disable(); }
TEST_F(NfaWlcMainTest, EvtToStr) {
  EXPECT_EQ(nfa_wlc_evt_2_str(NFA_WLC_API_ENABLE_EVT),
            "NFA_WLC_API_ENABLE_EVT");
  EXPECT_EQ(nfa_wlc_evt_2_str(NFA_WLC_API_START_EVT), "NFA_WLC_API_START_EVT");
  EXPECT_EQ(nfa_wlc_evt_2_str(NFA_WLC_API_NON_AUTO_START_WPT_EVT),
            "NFA_WLC_API_NON_AUTO_START_WPT_EVT");
  EXPECT_EQ(nfa_wlc_evt_2_str(NFA_WLC_API_REMOVE_EP_EVT), "Unknown");
}