//
// Copyright (C) 2024 The Android Open Source Project
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
#include "debug_lmrt.h"
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <android-base/logging.h>

extern lmrt_payload_t lmrt_payloads;
extern std::vector<uint8_t> committed_lmrt_tlvs;

class LmrtContentTest : public testing::Test {
private:
    lmrt_payload_t original_lmrt_payloads;
    std::vector<uint8_t> original_lmrt_tlvs = {};
    int original_routing_table_size;
protected:
    void SetUp() override {
        lmrt_payloads.more.swap(original_lmrt_payloads.more);
        lmrt_payloads.entry_count.swap(original_lmrt_payloads.entry_count);
        lmrt_payloads.tlvs.swap(original_lmrt_payloads.tlvs);
        committed_lmrt_tlvs.swap(original_lmrt_tlvs);
        original_routing_table_size = nfc_cb.max_ce_table;

        lmrt_payloads.more = {1, 1, 1, 1, 1, 0};
        lmrt_payloads.entry_count = {1, 1, 1, 1, 1, 1};
        lmrt_payloads.tlvs = {
                {0x52, 0x02, 0x00, 0x11},
                {0x43, 0x04, 0xC0, 0x3B, 0xFE, 0xFE},
                {0x41, 0x03, 0x00, 0x11, 0x04},
                {0x00, 0x03, 0xC0, 0x3B, 0x02},
                {0x00, 0x03, 0x00, 0x39, 0x00},
                {0x00, 0x03, 0x00, 0x39, 0x01}
        };
    }
    void TearDown() override {
        lmrt_payloads.more.swap(original_lmrt_payloads.more);
        lmrt_payloads.entry_count.swap(original_lmrt_payloads.entry_count);
        lmrt_payloads.tlvs.swap(original_lmrt_payloads.tlvs);
        committed_lmrt_tlvs.swap(original_lmrt_tlvs);
        nfc_cb.max_ce_table = original_routing_table_size;
    }
};

class LmrtLogTest : public ::testing::Test {
protected:
    android::base::LogSeverity original_log_severity;

    void SetUp() override {
        original_log_severity =
                android::base::SetMinimumLogSeverity(android::base::VERBOSE);
    }
    void TearDown() override {
        android::base::SetMinimumLogSeverity(original_log_severity);
    }
    MOCK_METHOD(void, logVerbose, (const std::string& message), ());
};

// Test: Test if payload is cleared after init
TEST_F(LmrtContentTest, InitLmrtPayloads) {
    debug_lmrt_init();

    EXPECT_EQ(lmrt_payloads.more.size(), 0);
    EXPECT_EQ(lmrt_payloads.entry_count.size(), 0);
    EXPECT_EQ(lmrt_payloads.tlvs.size(), 0);
}

// Test: Capture lmrt from nullptr
TEST_F(LmrtContentTest, CaptureLmrtFromNull) {
    lmrt_payload_t expect_lmrt_payloads;
    expect_lmrt_payloads.more = {1, 1, 1, 1, 1, 0};
    expect_lmrt_payloads.entry_count = {1, 1, 1, 1, 1, 1};
    expect_lmrt_payloads.tlvs = {
            {0x52, 0x02, 0x00, 0x11},
            {0x43, 0x04, 0xC0, 0x3B, 0xFE, 0xFE},
            {0x41, 0x03, 0x00, 0x11, 0x04},
            {0x00, 0x03, 0xC0, 0x3B, 0x02},
            {0x00, 0x03, 0x00, 0x39, 0x00},
            {0x00, 0x03, 0x00, 0x39, 0x01}
    };

    lmrt_capture(nullptr, 0);

    EXPECT_EQ(lmrt_payloads.more.size(), expect_lmrt_payloads.more.size());
    EXPECT_EQ(lmrt_payloads.entry_count.size(), expect_lmrt_payloads.more.size());
    EXPECT_EQ(lmrt_payloads.tlvs.size(), expect_lmrt_payloads.more.size());

    for (int i = 0; i < lmrt_payloads.more.size(); i++) {
        EXPECT_EQ(lmrt_payloads.more[i], expect_lmrt_payloads.more[i]);
    }

    for (int i = 0; i < lmrt_payloads.entry_count.size(); i++) {
        EXPECT_EQ(lmrt_payloads.entry_count[i], expect_lmrt_payloads.entry_count[i]);
    }

    for (int i = 0; i < lmrt_payloads.tlvs.size(); i++) {
        EXPECT_EQ(lmrt_payloads.tlvs[i].size(), expect_lmrt_payloads.tlvs[i].size());
        for (int j = 0; j < lmrt_payloads.tlvs[i].size(); j++) {
            EXPECT_EQ(lmrt_payloads.tlvs[i][j], expect_lmrt_payloads.tlvs[i][j]);
        }
    }
}

// Test: Capture lmrt from the invalid nci buffer
TEST_F(LmrtContentTest, CaptureLmrtFromInvalidNci) {
    lmrt_payload_t expect_lmrt_payloads;
    expect_lmrt_payloads.more = {1, 1, 1, 1, 1, 0};
    expect_lmrt_payloads.entry_count = {1, 1, 1, 1, 1, 1};
    expect_lmrt_payloads.tlvs = {
            {0x52, 0x02, 0x00, 0x11},
            {0x43, 0x04, 0xC0, 0x3B, 0xFE, 0xFE},
            {0x41, 0x03, 0x00, 0x11, 0x04},
            {0x00, 0x03, 0xC0, 0x3B, 0x02},
            {0x00, 0x03, 0x00, 0x39, 0x00},
            {0x00, 0x03, 0x00, 0x39, 0x01}
    };

    uint8_t invalid_nci_lmrt_cmd[] = {0x20, 0x01, 0x00};
    lmrt_capture(invalid_nci_lmrt_cmd, sizeof(invalid_nci_lmrt_cmd));

    EXPECT_EQ(lmrt_payloads.more.size(), expect_lmrt_payloads.more.size());
    EXPECT_EQ(lmrt_payloads.entry_count.size(), expect_lmrt_payloads.more.size());
    EXPECT_EQ(lmrt_payloads.tlvs.size(), expect_lmrt_payloads.more.size());

    for (int i = 0; i < lmrt_payloads.more.size(); i++) {
        EXPECT_EQ(lmrt_payloads.more[i], expect_lmrt_payloads.more[i]);
    }

    for (int i = 0; i < lmrt_payloads.entry_count.size(); i++) {
        EXPECT_EQ(lmrt_payloads.entry_count[i], expect_lmrt_payloads.entry_count[i]);
    }

    for (int i = 0; i < lmrt_payloads.tlvs.size(); i++) {
        EXPECT_EQ(lmrt_payloads.tlvs[i].size(), expect_lmrt_payloads.tlvs[i].size());
        for (int j = 0; j < lmrt_payloads.tlvs[i].size(); j++) {
            EXPECT_EQ(lmrt_payloads.tlvs[i][j], expect_lmrt_payloads.tlvs[i][j]);
        }
    }
}

// Test: Capture lmrt from the valid nci buffer
TEST_F(LmrtContentTest, CaptureLmrtPayload) {
    lmrt_payload_t expect_lmrt_payloads;
    expect_lmrt_payloads.more = {0};
    expect_lmrt_payloads.entry_count = {1};
    expect_lmrt_payloads.tlvs = {
            {0x00, 0x03, 0x00, 0x39, 0x01}
    };

    uint8_t nci_lmrt_cmd[] = {0x20, 0x01, 0x07, 0x00, 0x01, 0x00, 0x03, 0x00, 0x39, 0x01};
    lmrt_capture(nci_lmrt_cmd, sizeof(nci_lmrt_cmd));

    EXPECT_EQ(lmrt_payloads.more.size(), expect_lmrt_payloads.more.size());
    EXPECT_EQ(lmrt_payloads.entry_count.size(), expect_lmrt_payloads.more.size());
    EXPECT_EQ(lmrt_payloads.tlvs.size(), expect_lmrt_payloads.more.size());

    for (int i = 0; i < lmrt_payloads.more.size(); i++) {
        EXPECT_EQ(lmrt_payloads.more[i], expect_lmrt_payloads.more[i]);
    }

    for (int i = 0; i < lmrt_payloads.entry_count.size(); i++) {
        EXPECT_EQ(lmrt_payloads.entry_count[i], expect_lmrt_payloads.entry_count[i]);
    }

    for (int i = 0; i < lmrt_payloads.tlvs.size(); i++) {
        EXPECT_EQ(lmrt_payloads.tlvs[i].size(), expect_lmrt_payloads.tlvs[i].size());
        for (int j = 0; j < lmrt_payloads.tlvs[i].size(); j++) {
            EXPECT_EQ(lmrt_payloads.tlvs[i][j], expect_lmrt_payloads.tlvs[i][j]);
        }
    }
}

// Test: Capture lmrt from the valid but empty nci buffer
TEST_F(LmrtContentTest, CaptureEmptyLmrtPayload) {
    uint8_t nci_lmrt_cmd[] = {0x20, 0x01, 0x02, 0x00, 0x00};
    lmrt_capture(nci_lmrt_cmd, sizeof(nci_lmrt_cmd));

    EXPECT_EQ(lmrt_payloads.more.size(), 1);
    EXPECT_EQ(lmrt_payloads.entry_count.size(), 1);
    EXPECT_EQ(lmrt_payloads.tlvs.size(), 1);
    EXPECT_EQ(lmrt_payloads.tlvs[0].size(), 0);

    EXPECT_EQ(lmrt_payloads.more[0], 0);
    EXPECT_EQ(lmrt_payloads.entry_count[0], 0);
}

TEST_F(LmrtContentTest, GetRoutingTableSize) {
    nfc_cb.max_ce_table = 123;

    int result = lmrt_get_max_size();
    EXPECT_EQ(result, 123);
}

// Test: test if lmrt_get_tlvs() returns the address of committed_lmrt_tlvs
TEST_F(LmrtContentTest, GetCorrectTlvsPointer) {
    std::vector<uint8_t>* result = lmrt_get_tlvs();
    EXPECT_EQ(result, &committed_lmrt_tlvs);
}

// Test: test if modification affects the original committed_lmrt_tlvs
TEST_F(LmrtContentTest, GetModifiedTlvsPointer) {
    std::vector<uint8_t>* result = lmrt_get_tlvs();
    (*result).push_back(1);
    EXPECT_EQ(committed_lmrt_tlvs.back(), 1);
}

// Test: Test if committed_lmrt_tlvs could be update
TEST_F(LmrtContentTest, UpdateLmrtTlvs) {
    committed_lmrt_tlvs = {0x00, 0x03, 0x00, 0x39, 0x01};
    lmrt_update();

    int i = 0;
    for (auto tlv : lmrt_payloads.tlvs) {
        for (uint8_t b : tlv) {
            if (i >= committed_lmrt_tlvs.size())
                break;
            EXPECT_EQ(committed_lmrt_tlvs[i], b);
            i += 1;
        }
    }
}

// Test: Logging when payloads are empty
TEST_F(LmrtLogTest, LogEmptyLmrtPayloads) {
    lmrt_payloads.more.clear();
    lmrt_payloads.entry_count.clear();
    lmrt_payloads.tlvs.clear();
    EXPECT_CALL(*this, logVerbose("lmrt_log: No payloads to log")).Times(0);
    lmrt_log();
}

// Test: Logging a single LMRT payload
TEST_F(LmrtLogTest, LogSingleLmrtPayload) {
    lmrt_payloads.more = {1};
    lmrt_payloads.entry_count = {3};
    lmrt_payloads.tlvs = {{0x01, 0x02, 0x03}};
    EXPECT_CALL(*this, logVerbose("lmrt_log: Packet 1/1")).Times(0);
    EXPECT_CALL(*this, logVerbose("lmrt_log: 3 entries in this packet")).Times(0);
    EXPECT_CALL(*this, logVerbose("lmrt_log: tlv: 010203")).Times(0);
    lmrt_log();
}

// Test: Logging multiple LMRT payloads
TEST_F(LmrtLogTest, LogMultipleLmrtPayloads) {
    lmrt_payloads.more = {1, 0};
    lmrt_payloads.entry_count = {3, 2};
    lmrt_payloads.tlvs = {
            {0x01, 0x02, 0x03},{0x04, 0x05}
    };
    EXPECT_CALL(*this, logVerbose("lmrt_log: Packet 1/2")).Times(0);
    EXPECT_CALL(*this, logVerbose("lmrt_log: 3 entries in this packet")).Times(0);
    EXPECT_CALL(*this, logVerbose("lmrt_log: tlv: 010203")).Times(0);
    EXPECT_CALL(*this, logVerbose("lmrt_log: Packet 2/2")).Times(0);
    EXPECT_CALL(*this, logVerbose("lmrt_log: 2 entries in this packet")).Times(0);
    EXPECT_CALL(*this, logVerbose("lmrt_log: tlv: 0405")).Times(0);
    lmrt_log();
}
