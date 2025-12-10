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

#include "gki.h"
#include "gki_common.h"
#include "nfc_target.h"

class GkiTimerTest : public ::testing::Test {
 protected:
  void SetUp() override { gki_timers_init(); }
};

void mock_callback(bool start) { (void)start; }

TEST_F(GkiTimerTest, Timer) {
  GKI_start_timer(NFC_TIMER_ID, GKI_SECS_TO_TICKS(1), true);
  GKI_timer_update(5);
  EXPECT_EQ(GKI_get_tick_count(), 5);
  gki_adjust_timer_count(10);
  EXPECT_EQ(GKI_ready_to_sleep(), 10);
  GKI_stop_timer(NFC_TIMER_ID);
}

TEST_F(GkiTimerTest, List) {
  GKI_timer_queue_register_callback(mock_callback);
  TIMER_LIST_Q timer_list_queue;
  GKI_init_timer_list(&timer_list_queue);
  TIMER_LIST_ENT timer_list_ent;
  GKI_init_timer_list_entry(&timer_list_ent);
  timer_list_ent.in_use = true;
  timer_list_ent.ticks = 10;
  GKI_add_to_timer_list(&timer_list_queue, &timer_list_ent);
  EXPECT_EQ(GKI_get_remaining_ticks(&timer_list_queue, &timer_list_ent), 10);
  GKI_update_timer_list(&timer_list_queue, 1);
  EXPECT_FALSE(GKI_timer_queue_empty());
  GKI_remove_from_timer_list(&timer_list_queue, &timer_list_ent);
}
