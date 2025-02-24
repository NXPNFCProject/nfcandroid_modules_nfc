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
#include <gtest/gtest.h>

#include "nfa_mem_co.h"
#include "nfa_nv_co.h"
#include "nfc_hal_nv_co.h"

extern void verify_stack_non_volatile_store();
extern void delete_stack_non_volatile_store(bool forceDelete);

TEST(LibmainTest, buffer) {
  void* buffer;
  buffer = nfa_mem_co_alloc(0xFF);
  EXPECT_NE(buffer, nullptr);
  nfa_nv_co_write((uint8_t*)buffer, sizeof(buffer), HC_F2_NV_BLOCK);
  nfa_nv_co_read((uint8_t*)buffer, sizeof(buffer), HC_F2_NV_BLOCK);
  nfa_mem_co_free(buffer);
}

TEST(LibmainTest, VerifyStack) {
  verify_stack_non_volatile_store();
  delete_stack_non_volatile_store(true);
}