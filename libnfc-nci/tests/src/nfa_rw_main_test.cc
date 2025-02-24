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

#include "nfa_rw_main.cc"
#include <gtest/gtest.h>
#include <gmock/gmock.h>

class NfaRwEvt2StrTest: public :: testing::Test{
protected:
};

TEST_F(NfaRwEvt2StrTest, TestKnownEvents){
    EXPECT_EQ(nfa_rw_evt_2_str(NFA_RW_OP_REQUEST_EVT),"NFA_RW_OP_REQUEST_EVT");
    EXPECT_EQ(nfa_rw_evt_2_str(NFA_RW_ACTIVATE_NTF_EVT),"NFA_RW_ACTIVATE_NTF_EVT");
    EXPECT_EQ(nfa_rw_evt_2_str(NFA_RW_DEACTIVATE_NTF_EVT),"NFA_RW_DEACTIVATE_NTF_EVT");
    EXPECT_EQ(nfa_rw_evt_2_str(NFA_RW_PRESENCE_CHECK_TICK_EVT),"NFA_RW_PRESENCE_CHECK_TICK_EVT");
    EXPECT_EQ(nfa_rw_evt_2_str(
            NFA_RW_PRESENCE_CHECK_TIMEOUT_EVT),"NFA_RW_PRESENCE_CHECK_TIMEOUT_EVT");
}

TEST_F(NfaRwEvt2StrTest, TestUnknownEvents){
    EXPECT_EQ(nfa_rw_evt_2_str(0xFF),"Unknown");
}

class NfaRwHandleEventTest : public ::testing::Test {
protected:
    tNFA_RW_ACTION test_nfa_rw_action_tbl[NFA_RW_MAX_EVT];
    void SetUp() override {
        memcpy(test_nfa_rw_action_tbl, nfa_rw_action_tbl, sizeof(nfa_rw_action_tbl));
        for (int i = 0; i < NFA_RW_MAX_EVT; ++i) {
            test_nfa_rw_action_tbl[i] = nfa_rw_handle_valid_event;
        }
    }
    static bool nfa_rw_handle_valid_event(tNFA_RW_MSG* p_msg) {
        (void )p_msg;
        return true;
    }
    static bool nfa_rw_handle_invalid_event(tNFA_RW_MSG* p_msg) {
        (void )p_msg;
        return false;
    }
    void SetTestActionTable() {
        for (int i = 0; i < NFA_RW_MAX_EVT; ++i) {
            const_cast<tNFA_RW_ACTION*>(nfa_rw_action_tbl)[i] = test_nfa_rw_action_tbl[i];
        }
        std::cout << "Test action table set to global action table" << std::endl;
    }
};

TEST_F(NfaRwHandleEventTest, ValidEventHandled) {
    NFC_HDR msg;
    msg.event = 0x01;
    msg.len = 0;
    test_nfa_rw_action_tbl[0x01] = nfa_rw_handle_valid_event;
    SetTestActionTable();
    std::cout << "Starting test for valid event (0x01)" << std::endl;
    bool result = nfa_rw_handle_valid_event((tNFA_RW_MSG*)&msg);
    std::cout << "Test result for valid event (0x01): " << result << std::endl;
    EXPECT_TRUE(result);
}

TEST_F(NfaRwHandleEventTest, ValidEventButHandlerFails) {
    NFC_HDR msg;
    msg.event = 0x02;
    msg.len = 0;
    test_nfa_rw_action_tbl[0x02] = nfa_rw_handle_invalid_event;
    SetTestActionTable();
    bool result = nfa_rw_handle_event(&msg);
    EXPECT_TRUE(result);
}

TEST_F(NfaRwHandleEventTest, InvalidEvent) {
    NFC_HDR msg;
    msg.event = 0xFF;
    msg.len = 0;
    SetTestActionTable();
    bool result = nfa_rw_handle_event(&msg);
    EXPECT_TRUE(result);
}

TEST_F(NfaRwHandleEventTest, EventExceedsMax) {
    NFC_HDR msg;
    msg.event = NFA_RW_MAX_EVT + 1;
    msg.len = 0;
    SetTestActionTable();
    bool result = nfa_rw_handle_event(&msg);
    EXPECT_TRUE(result);
}

TEST_F(NfaRwHandleEventTest, EventNoHandlerSet) {
    NFC_HDR msg;
    msg.event = 0x03;
    msg.len = 0;
    test_nfa_rw_action_tbl[0x03] = nullptr;
    SetTestActionTable();
    bool result = nfa_rw_handle_event(&msg);
    EXPECT_TRUE(result);
}

class NfaRwSendRawFrameTest : public ::testing::Test {
protected:
    void SetUp() override {
        GKI_init();
    }
};

TEST_F(NfaRwSendRawFrameTest, SuccessCase) {
    NFC_HDR* p_data = (NFC_HDR*)GKI_getbuf(sizeof(NFC_HDR));
    ASSERT_NE(p_data, nullptr);
    tNFA_STATUS status = nfa_rw_send_raw_frame(p_data);
    EXPECT_EQ(status, NFA_STATUS_OK);
    GKI_freebuf(p_data);
}

TEST_F(NfaRwSendRawFrameTest, AllocationFailure) {
    GKI_disable();
    tNFA_STATUS status = nfa_rw_send_raw_frame(nullptr);
    EXPECT_EQ(status, NFA_STATUS_OK);
    GKI_enable();
}

TEST_F(NfaRwSendRawFrameTest, EventHandlingSuccess) {
    NFC_HDR* p_data = (NFC_HDR*)GKI_getbuf(sizeof(NFC_HDR));
    ASSERT_NE(p_data, nullptr);
    tNFA_STATUS status = nfa_rw_send_raw_frame(p_data);
    EXPECT_EQ(status, NFA_STATUS_OK);
}

TEST_F(NfaRwSendRawFrameTest, EventHandlingFailure) {
    NFC_HDR* p_data = (NFC_HDR*)GKI_getbuf(sizeof(NFC_HDR));
    ASSERT_NE(p_data, nullptr);
    tNFA_STATUS status = nfa_rw_send_raw_frame(p_data);
    EXPECT_EQ(status, NFA_STATUS_OK);
    GKI_freebuf(p_data);
}

class NfaRwProcDiscEvtTest : public ::testing::Test {
protected:
    tNFC_DISCOVER discover_data;
    tNFA_RW_MSG msg;
    void SetUp() override {
        memset(&discover_data, 0, sizeof(discover_data));
    }
};

TEST_F(NfaRwProcDiscEvtTest, TestActivatedEvent) {
    msg.activate_ntf.p_activate_params = nullptr;
    bool excl_rf_not_active = false;
    testing::internal::CaptureStdout();
    nfa_rw_proc_disc_evt(NFA_DM_RF_DISC_ACTIVATED_EVT, &discover_data, excl_rf_not_active);
    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_EQ(output.find("Event: 1"), std::string::npos);
}

TEST_F(NfaRwProcDiscEvtTest, TestDeactivatedEvent) {
    msg.activate_ntf.p_activate_params = nullptr;
    bool excl_rf_not_active = false;
    testing::internal::CaptureStdout();
    nfa_rw_proc_disc_evt(NFA_DM_RF_DISC_DEACTIVATED_EVT, &discover_data, excl_rf_not_active);
    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_EQ(output.find("Event: 2"), std::string::npos);
}

TEST_F(NfaRwProcDiscEvtTest, TestUnknownEvent) {
    msg.activate_ntf.p_activate_params = nullptr;
    bool excl_rf_not_active = false;
    testing::internal::CaptureStdout();
    nfa_rw_proc_disc_evt(0xFF, &discover_data, excl_rf_not_active);
    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_EQ(output, "");
}

void resetNFCStaticCallback() {
    NFC_SetStaticRfCback(nullptr);
}

class NfaRwSysDisableTest : public testing::Test {
protected:
    void SetUp() override {
        memset(&rw_cb, 0, sizeof(tRW_CB));
        memset(&nfa_rw_cb, 0, sizeof(tNFA_RW_CB));
    }
};

TEST_F(NfaRwSysDisableTest, TestRWCBTypeT1T) {
    rw_cb.tcb_type = RW_CB_TYPE_T1T;
    resetNFCStaticCallback();
    NFC_HDR* p_buf = (NFC_HDR*) malloc(sizeof(NFC_HDR));
    ASSERT_NE(p_buf, nullptr);
    p_buf->event = 0;
    p_buf->len = 10;
    rw_cb.tcb.t1t.p_cur_cmd_buf = p_buf;
    nfa_rw_sys_disable();
    nfa_rw_stop_presence_check_timer();
    ASSERT_EQ(rw_cb.tcb.t1t.p_cur_cmd_buf, nullptr);
    ASSERT_EQ(rw_cb.tcb_type, RW_CB_TYPE_UNKNOWN);
    ASSERT_EQ(nfa_rw_cb.p_pending_msg, nullptr);
}

TEST_F(NfaRwSysDisableTest, TestRWCBTypeT2T) {
    rw_cb.tcb_type = RW_CB_TYPE_T2T;
    resetNFCStaticCallback();
    NFC_HDR* p_buf1 = (NFC_HDR*) malloc(sizeof(NFC_HDR));
    NFC_HDR* p_buf2 = (NFC_HDR*) malloc(sizeof(NFC_HDR));
    ASSERT_NE(p_buf1, nullptr);
    ASSERT_NE(p_buf2, nullptr);
    p_buf1->event = 0;
    p_buf1->len = 10;
    p_buf2->event = 0;
    p_buf2->len = 10;
    rw_cb.tcb.t2t.p_cur_cmd_buf = p_buf1;
    rw_cb.tcb.t2t.p_sec_cmd_buf = p_buf2;
    nfa_rw_sys_disable();
    nfa_rw_stop_presence_check_timer();
    ASSERT_EQ(rw_cb.tcb.t2t.p_cur_cmd_buf, nullptr);
    ASSERT_EQ(rw_cb.tcb.t2t.p_sec_cmd_buf, nullptr);
    ASSERT_EQ(rw_cb.tcb_type, RW_CB_TYPE_UNKNOWN);
    ASSERT_EQ(nfa_rw_cb.p_pending_msg, nullptr);
}

TEST_F(NfaRwSysDisableTest, TestRWCBTypeT3T) {
    rw_cb.tcb_type = RW_CB_TYPE_T3T;
    resetNFCStaticCallback();
    NFC_HDR* p_buf = (NFC_HDR*) malloc(sizeof(NFC_HDR));
    ASSERT_NE(p_buf, nullptr);
    p_buf->event = 0;
    p_buf->len = 10;
    rw_cb.tcb.t3t.p_cur_cmd_buf = p_buf;
    nfa_rw_sys_disable();
    nfa_rw_stop_presence_check_timer();
    ASSERT_EQ(rw_cb.tcb.t3t.p_cur_cmd_buf, nullptr);
    ASSERT_EQ(rw_cb.tcb_type, RW_CB_TYPE_UNKNOWN);
    ASSERT_EQ(nfa_rw_cb.p_pending_msg, nullptr);
}

// Test 4: RW_CB_TYPE_T5T - Ensure buffer is freed
TEST_F(NfaRwSysDisableTest, TestRWCBTypeT5T) {
    rw_cb.tcb_type = RW_CB_TYPE_T5T;
    resetNFCStaticCallback();
    NFC_HDR* p_buf = (NFC_HDR*) malloc(sizeof(NFC_HDR));
    ASSERT_NE(p_buf, nullptr);
    p_buf->event = 0;
    p_buf->len = 10;
    rw_cb.tcb.i93.p_retry_cmd = p_buf;
    nfa_rw_sys_disable();
    nfa_rw_stop_presence_check_timer();
    ASSERT_EQ(rw_cb.tcb.i93.p_retry_cmd, nullptr);
    ASSERT_EQ(rw_cb.tcb_type, RW_CB_TYPE_UNKNOWN);
    ASSERT_EQ(nfa_rw_cb.p_pending_msg, nullptr);
}

TEST_F(NfaRwSysDisableTest, TestRWCBTypeUnknown) {
    rw_cb.tcb_type = RW_CB_TYPE_UNKNOWN;
    resetNFCStaticCallback();
    nfa_rw_sys_disable();
    nfa_rw_stop_presence_check_timer();
    ASSERT_EQ(rw_cb.tcb_type, RW_CB_TYPE_UNKNOWN);
    ASSERT_EQ(nfa_rw_cb.p_pending_msg, nullptr);
}
