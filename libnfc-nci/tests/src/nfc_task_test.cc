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

#include "nfc_task.cc"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "gki.h"
#include "mock_gki_utils.h"
#include "nfc_int.h"

class NfcTaskTest : public ::testing::Test {
  void SetUp() override { gki_utils = new MockGkiUtils(); }
  void TearDown() override {
    delete gki_utils;
    gki_utils = nullptr;
  }
};

// Test case 1: Empty timer list, non-NFC task
TEST_F(NfcTaskTest, EmptyListNonNfcTask) {
  TIMER_LIST_ENT tle;
  uint16_t type = 100;
  uint32_t timeout = 5;
  NFC_HDR* p_msg = (NFC_HDR*)malloc(NFC_HDR_SIZE);

  EXPECT_CALL(*((MockGkiUtils*)gki_utils),
              timer_list_empty(&nfc_cb.timer_queue))
      .WillOnce(testing::Return(true));
  EXPECT_CALL(*((MockGkiUtils*)gki_utils), get_taskid())
      .WillOnce(testing::Return(2));  // Not NFC_TASK
  EXPECT_CALL(*((MockGkiUtils*)gki_utils), getbuf(NFC_HDR_SIZE))
      .WillOnce(testing::Return(p_msg));
  EXPECT_CALL(*((MockGkiUtils*)gki_utils),
              send_msg(NFC_TASK, NFC_MBOX_ID, p_msg));
  EXPECT_CALL(*((MockGkiUtils*)gki_utils),
              remove_from_timer_list(&nfc_cb.timer_queue, &tle));
  EXPECT_CALL(*((MockGkiUtils*)gki_utils),
              add_to_timer_list(&nfc_cb.timer_queue, &tle));

  nfc_start_timer(&tle, type, timeout);

  ASSERT_EQ(tle.event, type);
  ASSERT_EQ(tle.ticks, timeout);

  free(p_msg);
}

// Test case 2: Empty timer list, NFC task
TEST_F(NfcTaskTest, EmptyListNfcTask) {
  TIMER_LIST_ENT tle;
  uint16_t type = 200;
  uint32_t timeout = 10;

  EXPECT_CALL(*((MockGkiUtils*)gki_utils),
              timer_list_empty(&nfc_cb.timer_queue))
      .WillOnce(testing::Return(true));
  EXPECT_CALL(*((MockGkiUtils*)gki_utils), get_taskid())
      .WillOnce(testing::Return(NFC_TASK));
  EXPECT_CALL(*((MockGkiUtils*)gki_utils),
              start_timer(NFC_TIMER_ID, GKI_SECS_TO_TICKS(1), true));
  EXPECT_CALL(*((MockGkiUtils*)gki_utils),
              remove_from_timer_list(&nfc_cb.timer_queue, &tle));
  EXPECT_CALL(*((MockGkiUtils*)gki_utils),
              add_to_timer_list(&nfc_cb.timer_queue, &tle));

  nfc_start_timer(&tle, type, timeout);

  ASSERT_EQ(tle.event, type);
  ASSERT_EQ(tle.ticks, timeout);
}

// Test case 3: Non-empty timer list
TEST_F(NfcTaskTest, NonEmptyList) {
  TIMER_LIST_ENT tle;
  uint16_t type = 300;
  uint32_t timeout = 15;

  EXPECT_CALL(*((MockGkiUtils*)gki_utils),
              timer_list_empty(&nfc_cb.timer_queue))
      .WillOnce(testing::Return(false));
  EXPECT_CALL(*((MockGkiUtils*)gki_utils),
              remove_from_timer_list(&nfc_cb.timer_queue, &tle));
  EXPECT_CALL(*((MockGkiUtils*)gki_utils),
              add_to_timer_list(&nfc_cb.timer_queue, &tle));

  nfc_start_timer(&tle, type, timeout);

  ASSERT_EQ(tle.event, type);
  ASSERT_EQ(tle.ticks, timeout);
}

TEST_F(NfcTaskTest, TestStopQuickTimer) {
  TIMER_LIST_ENT* mock_p_tle = new TIMER_LIST_ENT();
  EXPECT_CALL(*((MockGkiUtils*)gki_utils),
              remove_from_timer_list(testing::_, testing::_))
      .Times(1);
  EXPECT_CALL(*((MockGkiUtils*)gki_utils), stop_timer(testing::_)).Times(1);
  nfc_stop_quick_timer(mock_p_tle);
}

// Test case 1: Empty quick timer list, non-NFC task
TEST_F(NfcTaskTest, StartQuickTimerEmptyListNonNfcTask) {
  TIMER_LIST_ENT tle;
  uint16_t type = 100;
  uint32_t timeout = 5;
  NFC_HDR* p_msg = (NFC_HDR*)malloc(NFC_HDR_SIZE);

  EXPECT_CALL(*((MockGkiUtils*)gki_utils),
              timer_list_empty(&nfc_cb.quick_timer_queue))
      .WillOnce(testing::Return(true));
  EXPECT_CALL(*((MockGkiUtils*)gki_utils), get_taskid())
      .WillOnce(testing::Return(2));  // Not NFC_TASK
  EXPECT_CALL(*((MockGkiUtils*)gki_utils), getbuf(NFC_HDR_SIZE))
      .WillOnce(testing::Return(p_msg));
  EXPECT_CALL(*((MockGkiUtils*)gki_utils),
              send_msg(NFC_TASK, NFC_MBOX_ID, p_msg));
  EXPECT_CALL(*((MockGkiUtils*)gki_utils),
              remove_from_timer_list(&nfc_cb.quick_timer_queue, &tle));
  EXPECT_CALL(*((MockGkiUtils*)gki_utils),
              add_to_timer_list(&nfc_cb.quick_timer_queue, &tle));

  nfc_start_quick_timer(&tle, type, timeout);

  ASSERT_EQ(tle.event, type);
  ASSERT_EQ(tle.ticks, timeout);

  free(p_msg);
}

// Test case 2: Empty quick timer list, NFC task
TEST_F(NfcTaskTest, StartQuickTimerEmptyListNfcTask) {
  TIMER_LIST_ENT tle;
  uint16_t type = 200;
  uint32_t timeout = 10;

  EXPECT_CALL(*((MockGkiUtils*)gki_utils),
              timer_list_empty(&nfc_cb.quick_timer_queue))
      .WillOnce(testing::Return(true));
  EXPECT_CALL(*((MockGkiUtils*)gki_utils), get_taskid())
      .WillOnce(testing::Return(NFC_TASK));
  EXPECT_CALL(*((MockGkiUtils*)gki_utils),
              start_timer(NFC_QUICK_TIMER_ID, testing::_, true));
  EXPECT_CALL(*((MockGkiUtils*)gki_utils),
              remove_from_timer_list(&nfc_cb.quick_timer_queue, &tle));
  EXPECT_CALL(*((MockGkiUtils*)gki_utils),
              add_to_timer_list(&nfc_cb.quick_timer_queue, &tle));

  nfc_start_quick_timer(&tle, type, timeout);

  ASSERT_EQ(tle.event, type);
  ASSERT_EQ(tle.ticks, timeout);
}

// Test case 3: Non-empty quick timer list
TEST_F(NfcTaskTest, StartQuickTimerNonEmptyList) {
  TIMER_LIST_ENT tle;
  uint16_t type = 300;
  uint32_t timeout = 15;

  EXPECT_CALL(*((MockGkiUtils*)gki_utils),
              timer_list_empty(&nfc_cb.quick_timer_queue))
      .WillOnce(testing::Return(false));
  EXPECT_CALL(*((MockGkiUtils*)gki_utils),
              remove_from_timer_list(&nfc_cb.quick_timer_queue, &tle));
  EXPECT_CALL(*((MockGkiUtils*)gki_utils),
              add_to_timer_list(&nfc_cb.quick_timer_queue, &tle));

  nfc_start_quick_timer(&tle, type, timeout);

  ASSERT_EQ(tle.event, type);
  ASSERT_EQ(tle.ticks, timeout);
}
