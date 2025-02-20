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

#include "gki_ulinux.cc"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "gki.h"
#include "gki_common.h"
#include "gki_int.h"
#include "nfc_int.h"

uint32_t TASK_A(__attribute__((unused)) uint32_t arg) {
  uint16_t event;
  while (true) {
    event = GKI_wait(0xFFFF, 0);
    if (event) {
      break;
    }
  }
  return 0;
}
uint32_t TASK_B(__attribute__((unused)) uint32_t arg) {
  GKI_run(nullptr);
  return 0;
}
class GkiUlinuxTest : public ::testing::Test {
 protected:
  void SetUp() override { GKI_init(); }
};

TEST_F(GkiUlinuxTest, Task) {
  EXPECT_EQ(GKI_get_os_tick_count(), 0);
  gki_system_tick_start_stop_cback(true);
  GKI_sched_unlock();
  GKI_create_task((TASKPTR)TASK_A, NFC_TASK, (int8_t*)"TASK_A", nullptr, 0,
                  (pthread_cond_t*)nullptr, nullptr);
  EXPECT_EQ(GKI_map_taskname(NFC_TASK), (int8_t*)"TASK_A");
  EXPECT_EQ(GKI_isend_event(NFC_TASK, NFC_TASK_EVT_TRANSPORT_READY),
            GKI_SUCCESS);
  EXPECT_EQ(GKI_suspend_task(NFC_TASK), GKI_SUCCESS);
  EXPECT_EQ(GKI_resume_task(NFC_TASK), GKI_SUCCESS);
  GKI_create_task((TASKPTR)TASK_B, BTU_TASK, (int8_t*)"TASK_B", nullptr, 0,
                  (pthread_cond_t*)nullptr, nullptr);
  GKI_sched_lock();
  GKI_stop();
}
TEST_F(GkiUlinuxTest, Memory) {
  void* p_mem = GKI_os_malloc(sizeof(uint16_t));
  EXPECT_NE(p_mem, nullptr);
  GKI_register_mempool(p_mem);
  GKI_os_free(p_mem);
  GKI_exception(GKI_ERROR_BUF_SIZE_ZERO, "getbuf: Size is zero");
}