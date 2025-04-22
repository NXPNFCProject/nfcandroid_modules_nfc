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
#include "nfa_wlc_api.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "nfa_wlc_int.h"

static void mock_callback(__attribute__((unused)) tNFA_WLC_EVT event,
                          __attribute__((unused)) tNFA_WLC_EVT_DATA* p_data) {}

TEST(NfaWlcApiTest, WlcEnable) {
  EXPECT_EQ(NFA_WlcEnable(nullptr), NFA_STATUS_FAILED);
  EXPECT_EQ(NFA_WlcEnable(mock_callback), NFA_STATUS_OK);
}

TEST(NfaWlcApiTest, WlcStart) {
  tNFA_WLC_MODE mode;
  mode = 1;
  EXPECT_EQ(NFA_WlcStart(mode), NFA_STATUS_INVALID_PARAM);
  mode = 0;
  EXPECT_EQ(NFA_WlcStart(mode), NFA_STATUS_OK);
}

TEST(NfaWlcApiTest, WlcStartWPT) {
  uint8_t power_adj_req;
  uint8_t wpt_time_int;
  power_adj_req = 0x15;
  wpt_time_int = 0;
  EXPECT_FALSE(NFA_WlcStartWPT(power_adj_req, wpt_time_int));
  power_adj_req = POWER_ADJ_REQ_DEC_MIN;
  wpt_time_int = 0x14;
  EXPECT_FALSE(NFA_WlcStartWPT(power_adj_req, wpt_time_int));
  wpt_time_int = WPT_DURATION_INT_MAX;
  EXPECT_EQ(NFA_WlcStartWPT(power_adj_req, wpt_time_int), NFA_STATUS_OK);
}