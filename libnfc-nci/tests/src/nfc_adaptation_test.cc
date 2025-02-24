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
#include <android/binder_process.h>
#include <fcntl.h>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "NfcAdaptation.h"
#include "nfc_hal_api.h"

void mock_callback(__attribute__((unused)) uint8_t event,
                   __attribute__((unused)) tHAL_NFC_STATUS status) {}
void mock_callback2(__attribute__((unused)) uint16_t data_len,
                    __attribute__((unused)) uint8_t* p_data) {}

class NfcAdaptionTest : public ::testing::Test {
 protected:
  NfcAdaptation* theInstance;
  void SetUp() override {
    ABinderProcess_startThreadPool();
    theInstance = &NfcAdaptation::GetInstance();
    theInstance->Initialize();
  }
  void TearDown() override { theInstance->Finalize(); }
};

TEST_F(NfcAdaptionTest, Dump) {
  int fd = open("/tmp/nfc_snoop_test_dump", O_RDWR | O_CREAT | O_TRUNC, 0644);
  ASSERT_GE(fd, 0);
  theInstance->Dump(fd);
  close(fd);
}
TEST_F(NfcAdaptionTest, FactoryReset) { theInstance->FactoryReset(); }
TEST_F(NfcAdaptionTest, Shutdown) { theInstance->DeviceShutdown(); }
TEST_F(NfcAdaptionTest, HalFuncEntries) {
  tHAL_NFC_ENTRY* mHalEntryFuncs = theInstance->GetHalEntryFuncs();
  ASSERT_NE(mHalEntryFuncs, nullptr);
  mHalEntryFuncs->initialize();
  mHalEntryFuncs->open(mock_callback, mock_callback2);
  mHalEntryFuncs->core_initialized(0, nullptr);
  EXPECT_TRUE(mHalEntryFuncs->prediscover());
  mHalEntryFuncs->control_granted();
  mHalEntryFuncs->power_cycle();
  mHalEntryFuncs->get_max_ee();
  mHalEntryFuncs->close();
  mHalEntryFuncs->terminate();
}