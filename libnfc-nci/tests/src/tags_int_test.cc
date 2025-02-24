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

#include "tags_int.cc"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <string>

using testing::AllOf;
using testing::Ge;
using testing::Le;

// Tests for t1t_cmd_to_rsp_info
TEST(TagsInitTest, T1tCmdToRspInfo) {
  for (int i = 0; i < T1T_MAX_NUM_OPCODES; i++) {
    const tT1T_CMD_RSP_INFO* p0 = &t1t_cmd_rsp_infos[i];
    const tT1T_CMD_RSP_INFO* p1 = t1t_cmd_to_rsp_info(p0->opcode);
    ASSERT_TRUE(p1 != nullptr);
    EXPECT_EQ(p1->opcode, p0->opcode);
  }
}

// Tests for t1t_tag_init_data
TEST(TagsInitTest, T1tTagInitData) {
  for (int i = 0; i < T1T_MAX_TAG_MODELS; i++) {
    const tT1T_INIT_TAG* p0 = &t1t_init_content[i];
    const tT1T_INIT_TAG* p1 = t1t_tag_init_data(p0->tag_model);
    ASSERT_TRUE(p1 != nullptr);
    EXPECT_EQ(p1->tag_model, p0->tag_model);
  }
}

// Tests for t2t_tag_init_data
TEST(TagsInitTest, T2tTagInitData) {
  for (int i = 0; i < T2T_MAX_TAG_MODELS; i++) {
    const tT2T_INIT_TAG* p0 = &t2t_init_content[i];
    const tT2T_INIT_TAG* p1 = t2t_tag_init_data(
        p0->manufacturer_id, p0->b_multi_version, p0->version_no);
    ASSERT_TRUE(p1 != nullptr);
    EXPECT_EQ(p1->manufacturer_id, p0->manufacturer_id);
  }
}

// Tests for t2t_cmd_to_rsp_info
TEST(TagsInitTest, T2tCmdToRspInfo) {
  for (int i = 0; i < T2T_MAX_NUM_OPCODES; i++) {
    const tT2T_CMD_RSP_INFO* p0 = &t2t_cmd_rsp_infos[i];
    const tT2T_CMD_RSP_INFO* p1 = t2t_cmd_to_rsp_info(p0->opcode);
    ASSERT_TRUE(p1 != nullptr);
    EXPECT_EQ(p1->opcode, p0->opcode);
  }
}

// Tests for t1t_info_to_evt
TEST(TagsInitTest, T1tInfoToEvt) {
  for (int i = 0; i < T1T_MAX_NUM_OPCODES; i++) {
    const tT1T_CMD_RSP_INFO* p0 = &t1t_cmd_rsp_infos[i];
    uint8_t event = t1t_info_to_evt(p0);
    EXPECT_THAT(event, AllOf(Ge(RW_T1T_FIRST_EVT), Le(RW_T1T_MAX_EVT)));
  }
}

// Tests for t2t_info_to_evt
TEST(TagsInitTest, T2tInfoToEvt) {
  for (int i = 0; i < T2T_MAX_NUM_OPCODES; i++) {
    const tT2T_CMD_RSP_INFO* p0 = &t2t_cmd_rsp_infos[i];
    uint8_t event = t2t_info_to_evt(p0);
    EXPECT_THAT(event, AllOf(Ge(RW_T2T_FIRST_EVT), Le(RW_T2T_MAX_EVT)));
  }
}

// Tests for t1t_info_to_str
TEST(TagsInitTest, T1tInfoToStr) {
  for (int i = 0; i < T1T_MAX_NUM_OPCODES; i++) {
    const tT1T_CMD_RSP_INFO* p0 = &t1t_cmd_rsp_infos[i];
    const char* str = t1t_info_to_str(p0);
    EXPECT_EQ(str, t1t_cmd_str[i]);
  }
}

// Tests for t2t_info_to_str
TEST(TagsInitTest, T2tInfoToStr) {
  for (int i = 0; i < T2T_MAX_NUM_OPCODES; i++) {
    const tT2T_CMD_RSP_INFO* p0 = &t2t_cmd_rsp_infos[i];
    const char* str = t2t_info_to_str(p0);
    EXPECT_EQ(str, t2t_cmd_str[i]);
  }
}

// Tests for tags_pow
TEST(TagsInitTest, Tags_Pow) { EXPECT_EQ(tags_pow(1, 1), 1); }

// Tests for tags_ones32
TEST(TagsInitTest, Tags_Ones32) { EXPECT_EQ(tags_ones32(0), 0); }

// Tests for tags_log2
TEST(TagsInitTest, Tags_Log2) { EXPECT_EQ(tags_log2(2), 1); }
