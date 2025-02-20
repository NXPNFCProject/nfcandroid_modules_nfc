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
#include "gki_int.h"
#include "gki_target.h"

class GkiBufferTest : public ::testing::Test {
 protected:
  void SetUp() override { gki_buffer_init(); }
};

TEST_F(GkiBufferTest, Pool) {
  uint8_t pool_id = GKI_create_pool(1024, 1, GKI_RESTRICTED_POOL, nullptr);
  ASSERT_NE(pool_id, GKI_INVALID_POOL);
  EXPECT_NE(GKI_poolcount(pool_id), 0);
  EXPECT_NE(GKI_poolfreecount(pool_id), 0);
  EXPECT_NE(GKI_get_pool_bufsize(pool_id), 0);
  EXPECT_NE(GKI_poolutilization(pool_id), 100);
  GKI_delete_pool(pool_id);
  EXPECT_EQ(GKI_getpoolbuf(pool_id), nullptr);
}

TEST_F(GkiBufferTest, Queue) {
  BUFFER_Q buffer_q;
  GKI_init_q(&buffer_q);
  EXPECT_TRUE(GKI_queue_is_empty(&buffer_q));
  NFC_HDR* p_msg = (NFC_HDR*)GKI_getbuf(sizeof(NFC_HDR));
  ASSERT_NE(p_msg, nullptr);
  GKI_enqueue(&buffer_q, p_msg);
  EXPECT_EQ(GKI_queue_is_empty(&buffer_q), false);
  EXPECT_EQ(GKI_getnext(p_msg), nullptr);
  GKI_getfirst(&buffer_q);
  GKI_getlast(&buffer_q);
  EXPECT_NE(GKI_dequeue(&buffer_q), nullptr);
  GKI_enqueue_head(&buffer_q, p_msg);
  GKI_remove_from_queue(&buffer_q, p_msg);
  GKI_freebuf(p_msg);
}