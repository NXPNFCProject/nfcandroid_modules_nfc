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

#include "nfa_hci_main.cc"
#include <gtest/gtest.h>
#include <gmock/gmock.h>
void ResetNfaHciCb() {
    nfa_hci_cb.msg_len = 0;
    nfa_hci_cb.assembly_failed = false;
    memset(nfa_hci_cb.p_msg_data, 0, nfa_hci_cb.max_msg_len);
}

class NfaHciAssembleMsgTest : public testing::Test {
protected:
    void SetUp() override {
        nfa_hci_cb.max_msg_len = 1024;
        nfa_hci_cb.p_msg_data = new uint8_t[nfa_hci_cb.max_msg_len];
        ResetNfaHciCb();
    }
    void TearDown() override {
        delete[] nfa_hci_cb.p_msg_data;
    }
};

TEST_F(NfaHciAssembleMsgTest, NormalReassembly) {
    uint8_t test_data[] = {0x01, 0x02, 0x03};
    uint16_t data_len = sizeof(test_data);
    nfa_hci_assemble_msg(test_data, data_len);
    EXPECT_EQ(nfa_hci_cb.msg_len, data_len);
    EXPECT_FALSE(nfa_hci_cb.assembly_failed);
    EXPECT_EQ(memcmp(nfa_hci_cb.p_msg_data, test_data, data_len), 0);
}

TEST_F(NfaHciAssembleMsgTest, BufferOverflow) {
    uint8_t test_data[] = {0xFF, 0xEE, 0xDD, 0xCC};
    uint16_t data_len = nfa_hci_cb.max_msg_len + 10;
    nfa_hci_assemble_msg(test_data, data_len);
    EXPECT_EQ(nfa_hci_cb.msg_len, nfa_hci_cb.max_msg_len);
    EXPECT_TRUE(nfa_hci_cb.assembly_failed);
}

TEST_F(NfaHciAssembleMsgTest, PartialReassembly) {
    uint8_t test_data[] = {0xAA, 0xBB, 0xCC};
    nfa_hci_cb.msg_len = nfa_hci_cb.max_msg_len - 1;
    nfa_hci_assemble_msg(test_data, sizeof(test_data));
    EXPECT_EQ(nfa_hci_cb.msg_len, nfa_hci_cb.max_msg_len);
    EXPECT_TRUE(nfa_hci_cb.assembly_failed);
    EXPECT_EQ(nfa_hci_cb.p_msg_data[nfa_hci_cb.max_msg_len - 1], 0xAA);
}

TEST_F(NfaHciAssembleMsgTest, EmptyData) {
    uint8_t* test_data = nullptr;
    uint16_t data_len = 0;
    nfa_hci_assemble_msg(test_data, data_len);
    EXPECT_EQ(nfa_hci_cb.msg_len, 0);
    EXPECT_FALSE(nfa_hci_cb.assembly_failed);
}

TEST_F(NfaHciAssembleMsgTest, AppendToExistingData) {
    uint8_t initial_data[] = {0x11, 0x22};
    uint8_t new_data[] = {0x33, 0x44};
    memcpy(nfa_hci_cb.p_msg_data, initial_data, sizeof(initial_data));
    nfa_hci_cb.msg_len = sizeof(initial_data);
    nfa_hci_assemble_msg(new_data, sizeof(new_data));
    EXPECT_EQ(nfa_hci_cb.msg_len, sizeof(initial_data) + sizeof(new_data));
    EXPECT_FALSE(nfa_hci_cb.assembly_failed);
    EXPECT_EQ(memcmp(nfa_hci_cb.p_msg_data, initial_data, sizeof(initial_data)), 0);
    EXPECT_EQ(memcmp(nfa_hci_cb.p_msg_data + sizeof(initial_data), new_data, sizeof(new_data)), 0);
}

class NfaHciIsValidCfgTest : public testing::Test {
protected:
    void SetUp() override {
        memset(&nfa_hci_cb, 0, sizeof(nfa_hci_cb));
    }
};

TEST_F(NfaHciIsValidCfgTest, ValidConfiguration) {
    strncpy(nfa_hci_cb.cfg.reg_app_names[0], "App1", NFA_MAX_HCI_APP_NAME_LEN);
    nfa_hci_cb.cfg.b_send_conn_evts[0] = true;
    nfa_hci_cb.cfg.dyn_gates[0].gate_id = NFA_HCI_LOOP_BACK_GATE;
    nfa_hci_cb.cfg.dyn_gates[0].pipe_inx_mask = 0x01;
    nfa_hci_cb.cfg.dyn_gates[0].gate_owner = 0;
    nfa_hci_cb.cfg.dyn_pipes[0].pipe_id = NFA_HCI_FIRST_DYNAMIC_PIPE;
    nfa_hci_cb.cfg.dyn_pipes[0].pipe_state = NFA_HCI_PIPE_OPENED;
    nfa_hci_cb.cfg.dyn_pipes[0].local_gate = NFA_HCI_LOOP_BACK_GATE;
    nfa_hci_cb.cfg.dyn_pipes[0].dest_gate = NFA_HCI_LOOP_BACK_GATE;
    nfa_hci_cb.cfg.admin_gate.pipe01_state = NFA_HCI_PIPE_OPENED;
    nfa_hci_cb.cfg.link_mgmt_gate.pipe00_state = NFA_HCI_PIPE_OPENED;
    nfa_hci_cb.cfg.id_mgmt_gate.pipe_inx_mask = 0x01;
    EXPECT_FALSE(nfa_hci_is_valid_cfg());
}

TEST_F(NfaHciIsValidCfgTest, InvalidAppNameLength) {
    memset(nfa_hci_cb.cfg.reg_app_names[0], 'A', NFA_MAX_HCI_APP_NAME_LEN + 1);
    EXPECT_FALSE(nfa_hci_is_valid_cfg());
}

TEST_F(NfaHciIsValidCfgTest, DuplicateAppNames) {
    strncpy(nfa_hci_cb.cfg.reg_app_names[0], "App1", NFA_MAX_HCI_APP_NAME_LEN);
    strncpy(nfa_hci_cb.cfg.reg_app_names[1], "App1", NFA_MAX_HCI_APP_NAME_LEN);
    EXPECT_FALSE(nfa_hci_is_valid_cfg());
}

TEST_F(NfaHciIsValidCfgTest, InvalidConnectivityEventFlag) {
    strncpy(nfa_hci_cb.cfg.reg_app_names[0], "App1", NFA_MAX_HCI_APP_NAME_LEN);
    nfa_hci_cb.cfg.b_send_conn_evts[0] = 2; // Invalid value
    EXPECT_TRUE(nfa_hci_is_valid_cfg());
}

TEST_F(NfaHciIsValidCfgTest, InvalidGateId) {
    nfa_hci_cb.cfg.dyn_gates[0].gate_id = 0xFF;
    EXPECT_TRUE(nfa_hci_is_valid_cfg());
}

TEST_F(NfaHciIsValidCfgTest, DuplicateGateIds) {
    nfa_hci_cb.cfg.dyn_gates[0].gate_id = NFA_HCI_LOOP_BACK_GATE;
    nfa_hci_cb.cfg.dyn_gates[1].gate_id = NFA_HCI_LOOP_BACK_GATE;
    EXPECT_FALSE(nfa_hci_is_valid_cfg());
}

TEST_F(NfaHciIsValidCfgTest, InvalidPipeState) {
    nfa_hci_cb.cfg.dyn_pipes[0].pipe_id = NFA_HCI_FIRST_DYNAMIC_PIPE;
    nfa_hci_cb.cfg.dyn_pipes[0].pipe_state = 0xFF;
    EXPECT_FALSE(nfa_hci_is_valid_cfg());
}

TEST_F(NfaHciIsValidCfgTest, InvalidAdminGatePipeState) {
    nfa_hci_cb.cfg.admin_gate.pipe01_state = 0xFF;
    EXPECT_FALSE(nfa_hci_is_valid_cfg());
}

TEST_F(NfaHciIsValidCfgTest, InvalidLinkMgmtGatePipeState) {
    nfa_hci_cb.cfg.link_mgmt_gate.pipe00_state = 0xFF;
    EXPECT_FALSE(nfa_hci_is_valid_cfg());
}

TEST_F(NfaHciIsValidCfgTest, InvalidPipeInIdentityManagementGate) {
    nfa_hci_cb.cfg.id_mgmt_gate.pipe_inx_mask = 0x01;
    nfa_hci_cb.cfg.dyn_pipes[0].pipe_id = NFA_HCI_FIRST_DYNAMIC_PIPE;
    nfa_hci_cb.cfg.dyn_pipes[0].local_gate = 0xFF;
    EXPECT_FALSE(nfa_hci_is_valid_cfg());
}

TEST_F(NfaHciIsValidCfgTest, DuplicatePipeIds) {
    nfa_hci_cb.cfg.dyn_pipes[0].pipe_id = NFA_HCI_FIRST_DYNAMIC_PIPE;
    nfa_hci_cb.cfg.dyn_pipes[1].pipe_id = NFA_HCI_FIRST_DYNAMIC_PIPE;
    EXPECT_FALSE(nfa_hci_is_valid_cfg());
}

class MockNfaHci {
public:
    MOCK_METHOD(void, nfa_sys_cback_notify_nfcc_power_mode_proc_complete, (uint8_t id), ());
    MOCK_METHOD(void, nfa_sys_stop_timer, (TIMER_LIST_ENT* p_tle), ());
};

MockNfaHci mock_nfa_hci;

class NfaHciProcNfccPowerModeTest : public testing::Test {
protected:
    void SetUp() override {
        mock_nfa_hci = std::make_unique<MockNfaHci>();
        memset(&nfa_hci_cb, 0, sizeof(nfa_hci_cb));
    }
    std::unique_ptr<MockNfaHci> mock_nfa_hci;
};

TEST_F(NfaHciProcNfccPowerModeTest, FullPowerModeWhenIdle) {
    nfa_hci_cb.hci_state = NFA_HCI_STATE_IDLE;
    nfa_hci_cb.num_nfcee = 1;
    EXPECT_CALL(*mock_nfa_hci, nfa_sys_cback_notify_nfcc_power_mode_proc_complete(
            NFA_ID_HCI)).Times(0);
    nfa_hci_proc_nfcc_power_mode(NFA_DM_PWR_MODE_FULL);
    EXPECT_EQ(nfa_hci_cb.b_low_power_mode, false);
    EXPECT_EQ(nfa_hci_cb.hci_state, NFA_HCI_STATE_RESTORE);
    EXPECT_EQ(nfa_hci_cb.ee_disc_cmplt, false);
    EXPECT_EQ(nfa_hci_cb.ee_disable_disc, true);
    EXPECT_EQ(nfa_hci_cb.w4_hci_netwk_init, false);
    EXPECT_EQ(nfa_hci_cb.conn_id, 0);
    EXPECT_EQ(nfa_hci_cb.num_ee_dis_req_ntf, 0);
    EXPECT_EQ(nfa_hci_cb.num_hot_plug_evts, 0);
}

TEST_F(NfaHciProcNfccPowerModeTest, FullPowerModeWhenNotIdle) {
    nfa_hci_cb.hci_state = NFA_HCI_STATE_RESTORE;
    EXPECT_CALL(*mock_nfa_hci, nfa_sys_cback_notify_nfcc_power_mode_proc_complete(
            NFA_ID_HCI)).Times(0);
    nfa_hci_proc_nfcc_power_mode(NFA_DM_PWR_MODE_FULL);
}

TEST_F(NfaHciProcNfccPowerModeTest, NonFullPowerMode) {
    nfa_hci_cb.hci_state = NFA_HCI_STATE_IDLE;
    nfa_hci_cb.num_nfcee = 1;
    EXPECT_CALL(*mock_nfa_hci, nfa_sys_cback_notify_nfcc_power_mode_proc_complete(
            NFA_ID_HCI)).Times(0);
    nfa_hci_proc_nfcc_power_mode(0);
    EXPECT_EQ(nfa_hci_cb.hci_state, NFA_HCI_STATE_IDLE);
    EXPECT_EQ(nfa_hci_cb.w4_rsp_evt, false);
    EXPECT_EQ(nfa_hci_cb.conn_id, 0);
    EXPECT_EQ(nfa_hci_cb.b_low_power_mode, true);
}

TEST_F(NfaHciProcNfccPowerModeTest, FullPowerModeWhenMultipleNfcee) {
    nfa_hci_cb.hci_state = NFA_HCI_STATE_IDLE;
    nfa_hci_cb.num_nfcee = 2;
    nfa_hci_proc_nfcc_power_mode(NFA_DM_PWR_MODE_FULL);
    EXPECT_EQ(nfa_hci_cb.w4_hci_netwk_init, true);
}

TEST_F(NfaHciProcNfccPowerModeTest, FullPowerModeWhenSingleNfcee) {
    nfa_hci_cb.hci_state = NFA_HCI_STATE_IDLE;
    nfa_hci_cb.num_nfcee = 1;
    nfa_hci_proc_nfcc_power_mode(NFA_DM_PWR_MODE_FULL);
    EXPECT_EQ(nfa_hci_cb.w4_hci_netwk_init, false);
}

TEST_F(NfaHciProcNfccPowerModeTest, LowPowerModeStateReset) {
    nfa_hci_cb.hci_state = NFA_HCI_STATE_RESTORE;
    nfa_hci_cb.num_nfcee = 1;
    EXPECT_CALL(*mock_nfa_hci, nfa_sys_cback_notify_nfcc_power_mode_proc_complete(
            NFA_ID_HCI)).Times(0);
    EXPECT_CALL(*mock_nfa_hci, nfa_sys_stop_timer(&nfa_hci_cb.timer)).Times(0);
    nfa_hci_proc_nfcc_power_mode(0);
    EXPECT_EQ(nfa_hci_cb.hci_state, NFA_HCI_STATE_IDLE);
    EXPECT_EQ(nfa_hci_cb.b_low_power_mode, true);
    EXPECT_EQ(nfa_hci_cb.conn_id, 0);
}

class MockNfaHciCallbacks {
public:
    MOCK_METHOD(void, nfa_hci_startup_complete, (tNFA_STATUS status), ());
    MOCK_METHOD(void, nfa_hciu_send_get_param_cmd, (uint8_t pipe, uint8_t index), ());
    MOCK_METHOD(void, nfa_hciu_send_clear_all_pipe_cmd, (), ());
    MOCK_METHOD(void, nfa_hciu_remove_all_pipes_from_host, (uint8_t host), ());
    MOCK_METHOD(void, nfa_hci_api_dealloc_gate, (tNFA_HCI_EVENT_DATA* p_evt_data), ());
    MOCK_METHOD(void, nfa_hci_api_deregister, (tNFA_HCI_EVENT_DATA* p_evt_data), ());
    MOCK_METHOD(void, nfa_hciu_send_to_app, (tNFA_HCI_EVT event, tNFA_HCI_EVT_DATA* p_evt,
                                             tNFA_HANDLE app_handle), ());
    MOCK_METHOD(void, nfa_hciu_send_delete_pipe_cmd, (uint8_t pipe), ());
    MOCK_METHOD(void, nfa_hciu_release_pipe, (uint8_t pipe_id), ());
};

MockNfaHciCallbacks* mock_nfa_hci_calls = nullptr;

class NfaHciRspTimeoutTest : public ::testing::Test {
protected:
    virtual void SetUp() {
        mock_nfa_hci_calls = new MockNfaHciCallbacks();
    }
    virtual void TearDown() {
        delete mock_nfa_hci_calls;
        mock_nfa_hci_calls = nullptr;
    }
};

TEST_F(NfaHciRspTimeoutTest, TestStartupState) {
    nfa_hci_cb.hci_state = NFA_HCI_STATE_STARTUP;
    EXPECT_CALL(*mock_nfa_hci_calls, nfa_hci_startup_complete(NFA_STATUS_TIMEOUT)).Times(0);
    nfa_hci_rsp_timeout();
}

TEST_F(NfaHciRspTimeoutTest, TestRestoreState) {
    nfa_hci_cb.hci_state = NFA_HCI_STATE_RESTORE;
    EXPECT_CALL(*mock_nfa_hci_calls, nfa_hci_startup_complete(NFA_STATUS_TIMEOUT)).Times(0);
    nfa_hci_rsp_timeout();
}

TEST_F(NfaHciRspTimeoutTest, TestWaitNetwkEnableStateWithInit) {
    nfa_hci_cb.hci_state = NFA_HCI_STATE_WAIT_NETWK_ENABLE;
    nfa_hci_cb.w4_hci_netwk_init = true;
    EXPECT_CALL(*mock_nfa_hci_calls, nfa_hciu_send_get_param_cmd(
            NFA_HCI_ADMIN_PIPE, NFA_HCI_HOST_LIST_INDEX)).Times(0);
    nfa_hci_rsp_timeout();
}

TEST_F(NfaHciRspTimeoutTest, TestWaitNetwkEnableStateNoInit) {
    nfa_hci_cb.hci_state = NFA_HCI_STATE_WAIT_NETWK_ENABLE;
    nfa_hci_cb.w4_hci_netwk_init = false;
    EXPECT_CALL(*mock_nfa_hci_calls, nfa_hci_startup_complete(NFA_STATUS_FAILED)).Times(0);
    nfa_hci_rsp_timeout();
}

TEST_F(NfaHciRspTimeoutTest, TestRemoveGateStateWithDeletePipe) {
    nfa_hci_cb.hci_state = NFA_HCI_STATE_REMOVE_GATE;
    nfa_hci_cb.cmd_sent = NFA_HCI_ADM_DELETE_PIPE;
    EXPECT_CALL(*mock_nfa_hci_calls, nfa_hciu_send_clear_all_pipe_cmd()).Times(0);
    nfa_hci_rsp_timeout();
}

TEST_F(NfaHciRspTimeoutTest, TestRemoveGateStateNoDeletePipe) {
    nfa_hci_cb.hci_state = NFA_HCI_STATE_REMOVE_GATE;
    nfa_hci_cb.cmd_sent = 0;
    EXPECT_CALL(*mock_nfa_hci_calls, nfa_hciu_remove_all_pipes_from_host(0)).Times(0);
    EXPECT_CALL(*mock_nfa_hci_calls, nfa_hci_api_dealloc_gate(nullptr)).Times(0);
    nfa_hci_rsp_timeout();
}

TEST_F(NfaHciRspTimeoutTest, TestAppDeregisterStateWithDeletePipe) {
    nfa_hci_cb.hci_state = NFA_HCI_STATE_APP_DEREGISTER;
    nfa_hci_cb.cmd_sent = NFA_HCI_ADM_DELETE_PIPE;
    EXPECT_CALL(*mock_nfa_hci_calls, nfa_hciu_send_clear_all_pipe_cmd()).Times(0);
    nfa_hci_rsp_timeout();
}

TEST_F(NfaHciRspTimeoutTest, TestWaitRspStateWithRspEvt) {
    nfa_hci_cb.hci_state = NFA_HCI_STATE_WAIT_RSP;
    nfa_hci_cb.w4_rsp_evt = true;
    nfa_hci_cb.pipe_in_use = 1;
    EXPECT_CALL(*mock_nfa_hci_calls, nfa_hciu_send_to_app(
            NFA_HCI_EVENT_RCVD_EVT, testing::_ , nfa_hci_cb.app_in_use)).Times(0);
    nfa_hci_rsp_timeout();
}

TEST_F(NfaHciRspTimeoutTest, TestWaitRspStateWithSetParameterCmd) {
    nfa_hci_cb.hci_state = NFA_HCI_STATE_WAIT_RSP;
    nfa_hci_cb.w4_rsp_evt = false;
    nfa_hci_cb.cmd_sent = NFA_HCI_ANY_SET_PARAMETER;
    nfa_hci_cb.pipe_in_use = 1;
    EXPECT_CALL(*mock_nfa_hci_calls, nfa_hciu_send_delete_pipe_cmd(1)).Times(0);
    EXPECT_CALL(*mock_nfa_hci_calls, nfa_hciu_release_pipe(1)).Times(0);
    nfa_hci_rsp_timeout();
}

TEST_F(NfaHciRspTimeoutTest, TestDisabledOrInvalidState) {
    nfa_hci_cb.hci_state = NFA_HCI_STATE_DISABLED;
    EXPECT_CALL(*mock_nfa_hci_calls, nfa_hciu_send_to_app(
            testing::_, testing::_, testing::_)).Times(0);
    nfa_hci_rsp_timeout();
}

class NfaHciSetReceiveBufTest : public ::testing::Test {
protected:
    void SetUp() override {
        nfa_hci_cb.p_msg_data = nullptr;
        nfa_hci_cb.max_msg_len = 0;
        nfa_hci_cb.rsp_buf_size = 0;
        nfa_hci_cb.p_rsp_buf = nullptr;
        nfa_hci_cb.type = 0;
    }
};

TEST_F(NfaHciSetReceiveBufTest, PipeNotInRange) {
    uint8_t pipe = 0;
    nfa_hci_set_receive_buf(pipe);
    EXPECT_EQ(nfa_hci_cb.p_msg_data, nfa_hci_cb.msg_data);
    EXPECT_EQ(nfa_hci_cb.max_msg_len, NFA_MAX_HCI_EVENT_LEN);
}

TEST_F(NfaHciSetReceiveBufTest, PipeInRangeButWrongType) {
    uint8_t pipe = NFA_HCI_FIRST_DYNAMIC_PIPE;
    nfa_hci_cb.type = 1;
    nfa_hci_set_receive_buf(pipe);
    EXPECT_EQ(nfa_hci_cb.p_msg_data, nfa_hci_cb.msg_data);
    EXPECT_EQ(nfa_hci_cb.max_msg_len, NFA_MAX_HCI_EVENT_LEN);
}

TEST_F(NfaHciSetReceiveBufTest, PipeInRangeWithNoResponseBuffer) {
    uint8_t pipe = NFA_HCI_FIRST_DYNAMIC_PIPE;
    nfa_hci_cb.type = NFA_HCI_EVENT_TYPE;
    nfa_hci_cb.rsp_buf_size = 0;
    nfa_hci_set_receive_buf(pipe);
    EXPECT_EQ(nfa_hci_cb.p_msg_data, nfa_hci_cb.msg_data);
    EXPECT_EQ(nfa_hci_cb.max_msg_len, NFA_MAX_HCI_EVENT_LEN);
}

TEST_F(NfaHciSetReceiveBufTest, PipeInRangeWithRspBufSizeZeroAndNullRspBuf) {
    uint8_t pipe = NFA_HCI_FIRST_DYNAMIC_PIPE;
    nfa_hci_cb.type = NFA_HCI_EVENT_TYPE;
    nfa_hci_cb.rsp_buf_size = 10;
    nfa_hci_cb.p_rsp_buf = nullptr;
    nfa_hci_set_receive_buf(pipe);
    EXPECT_EQ(nfa_hci_cb.p_msg_data, nfa_hci_cb.msg_data);
    EXPECT_EQ(nfa_hci_cb.max_msg_len, NFA_MAX_HCI_EVENT_LEN);
}

TEST_F(NfaHciSetReceiveBufTest, PipeInRangeWithValidRspBuf) {
    uint8_t pipe = NFA_HCI_FIRST_DYNAMIC_PIPE;
    nfa_hci_cb.type = NFA_HCI_EVENT_TYPE;
    nfa_hci_cb.rsp_buf_size = 10;
    uint8_t rsp_buf[10] = {0};
    nfa_hci_cb.p_rsp_buf = rsp_buf;
    nfa_hci_set_receive_buf(pipe);
    EXPECT_EQ(nfa_hci_cb.p_msg_data, rsp_buf);
    EXPECT_EQ(nfa_hci_cb.max_msg_len, 10);
}

TEST_F(NfaHciSetReceiveBufTest, PipeInRangeWithValidRspBufOfDifferentSize) {
    uint8_t pipe = NFA_HCI_FIRST_DYNAMIC_PIPE;
    nfa_hci_cb.type = NFA_HCI_EVENT_TYPE;
    nfa_hci_cb.rsp_buf_size = 20;
    uint8_t rsp_buf[20] = {0};
    nfa_hci_cb.p_rsp_buf = rsp_buf;
    nfa_hci_set_receive_buf(pipe);
    EXPECT_EQ(nfa_hci_cb.p_msg_data, rsp_buf);
    EXPECT_EQ(nfa_hci_cb.max_msg_len, 20);
}

#define NFA_EE_INTERFACE_UNKNOWN 0
class NfaHciStartupTest : public ::testing::Test {
protected:
    MOCK_METHOD(void, nfa_hciu_send_open_pipe_cmd, (uint8_t pipe), ());
    MOCK_METHOD(void, NFC_NfceeModeSet, (uint8_t nfcee_id, tNFC_NFCEE_MODE mode), ());
    MOCK_METHOD(int, NFC_ConnCreate, (uint8_t dest_type, uint8_t id,
            uint8_t protocol, tNFC_CONN_CBACK* p_cback), ());
    MOCK_METHOD(void, NFA_EeGetInfo, (uint8_t* p_num_nfcee, tNFA_EE_INFO* p_info), ());
    MOCK_METHOD(void, NFA_EeModeSet, (uint8_t ee_handle, uint8_t mode), ());
    MOCK_METHOD(void, NFC_SetStaticHciCback, (tNFC_CONN_CBACK* p_cback), ());
    MOCK_METHOD(uint8_t, NFC_GetNCIVersion, (), ());
    MOCK_METHOD(void, nfa_hci_startup_complete, (tNFA_STATUS status), ());
    void SetUp() override {
        memset(&nfa_hci_cb, 0, sizeof(nfa_hci_cb));
    }
    void TearDown() override {
    }
};

TEST_F(NfaHciStartupTest, Test_HciLoopbackDebug) {
    HCI_LOOPBACK_DEBUG = NFA_HCI_DEBUG_ON;
    EXPECT_CALL(*this, nfa_hciu_send_open_pipe_cmd(NFA_HCI_ADMIN_PIPE)).Times(0);
    nfa_hci_startup();
}

TEST_F(NfaHciStartupTest, Test_NVReadAndEEDiscoveryIncomplete) {
    nfa_hci_cb.nv_read_cmplt = false;
    nfa_hci_cb.ee_disc_cmplt = false;
    EXPECT_CALL(*this, nfa_hciu_send_open_pipe_cmd(NFA_HCI_ADMIN_PIPE)).Times(0);
    nfa_hci_startup();
}

TEST_F(NfaHciStartupTest, Test_NVReadEECompleteConnIDZero) {
    nfa_hci_cb.nv_read_cmplt = true;
    nfa_hci_cb.ee_disc_cmplt = true;
    nfa_hci_cb.conn_id = 0;
    EXPECT_CALL(*this, NFC_SetStaticHciCback(nfa_hci_conn_cback)).Times(0);
    nfa_hci_startup();
}

TEST_F(NfaHciStartupTest, Test_NfcVersionLessThan2_0) {
    nfa_hci_cb.nv_read_cmplt = true;
    nfa_hci_cb.ee_disc_cmplt = true;
    nfa_hci_cb.conn_id = 0;
    EXPECT_CALL(*this, NFA_EeGetInfo(testing::_, testing::_)).Times(0);
    nfa_hci_startup();
}

TEST_F(NfaHciStartupTest, Test_HciAccessInterfaceFoundAndActive) {
    nfa_hci_cb.nv_read_cmplt = true;
    nfa_hci_cb.ee_disc_cmplt = true;
    nfa_hci_cb.conn_id = 0;
    tNFA_EE_INFO ee_info = {};
    ee_info.ee_handle = 1;
    ee_info.ee_interface[0] = NFA_EE_INTERFACE_HCI_ACCESS;
    ee_info.ee_status = NFA_EE_STATUS_INACTIVE;
    nfa_hci_cb.ee_info[0] = ee_info;
    nfa_hci_cb.num_nfcee = 1;
    EXPECT_CALL(*this, NFC_NfceeModeSet(1, NFC_MODE_ACTIVATE)).Times(0);
    nfa_hci_startup();
}

TEST_F(NfaHciStartupTest, Test_HciAccessInterfaceNotFound) {
    nfa_hci_cb.nv_read_cmplt = true;
    nfa_hci_cb.ee_disc_cmplt = true;
    nfa_hci_cb.conn_id = 0;
    nfa_hci_cb.num_nfcee = 1;
    nfa_hci_cb.ee_info[0].ee_interface[0] = NFA_EE_INTERFACE_UNKNOWN;
    EXPECT_CALL(*this, nfa_hci_startup_complete(NFA_STATUS_FAILED)).Times(0);
    nfa_hci_startup();
}

TEST_F(NfaHciStartupTest, Test_ConnectionCreationFails) {
    nfa_hci_cb.nv_read_cmplt = true;
    nfa_hci_cb.ee_disc_cmplt = true;
    nfa_hci_cb.conn_id = 0;
    EXPECT_CALL(*this, nfa_hci_startup_complete(NFA_STATUS_FAILED)).Times(0);
    nfa_hci_startup();
}

class NfaHciEeInfoCbackTest : public ::testing::Test {
protected:
    void SetUp() override {
        nfa_hci_cb.hci_state = NFA_HCI_STATE_STARTUP;
        nfa_hci_cb.num_nfcee = 1;
        nfa_hci_cb.num_ee_dis_req_ntf = 0;
        nfa_hci_cb.num_hot_plug_evts = 0;
        nfa_hci_cb.conn_id = 0;
        nfa_hci_cb.ee_disable_disc = false;
        nfa_hci_cb.ee_disc_cmplt = false;
        nfa_hci_cb.w4_hci_netwk_init = false;
        nfa_hci_cb.timer = {};
    }
};

TEST_F(NfaHciEeInfoCbackTest, TestEEStatusOn) {
    nfa_hci_cb.hci_state = NFA_HCI_STATE_STARTUP;
    nfa_hci_ee_info_cback(NFA_EE_DISC_STS_ON);
    EXPECT_TRUE(nfa_hci_cb.ee_disc_cmplt);
    EXPECT_EQ(nfa_hci_cb.num_ee_dis_req_ntf, 0);
    EXPECT_EQ(nfa_hci_cb.num_hot_plug_evts, 0);
    EXPECT_EQ(nfa_hci_cb.conn_id, 0);
}

TEST_F(NfaHciEeInfoCbackTest, TestEEStatusOff) {
    nfa_hci_cb.hci_state = NFA_HCI_STATE_WAIT_NETWK_ENABLE;
    nfa_hci_cb.num_nfcee = 2;
    nfa_hci_cb.num_ee_dis_req_ntf = 1;
    nfa_hci_cb.num_hot_plug_evts = 1;
    nfa_hci_ee_info_cback(NFA_EE_DISC_STS_OFF);
    EXPECT_TRUE(nfa_hci_cb.ee_disable_disc);
}

TEST_F(NfaHciEeInfoCbackTest, TestEEStatusOffNoUiccHost) {
    nfa_hci_cb.hci_state = NFA_HCI_STATE_WAIT_NETWK_ENABLE;
    nfa_hci_cb.num_nfcee = 1;
    nfa_hci_ee_info_cback(NFA_EE_DISC_STS_OFF);
    EXPECT_FALSE(nfa_hci_cb.w4_hci_netwk_init);
}

TEST_F(NfaHciEeInfoCbackTest, TestEEStatusReq) {
    nfa_hci_cb.hci_state = NFA_HCI_STATE_WAIT_NETWK_ENABLE;
    nfa_hci_cb.num_ee_dis_req_ntf = 1;
    nfa_hci_cb.num_nfcee = 2;
    nfa_hci_ee_info_cback(NFA_EE_DISC_STS_REQ);
    EXPECT_EQ(nfa_hci_cb.num_ee_dis_req_ntf, 2);
}

TEST_F(NfaHciEeInfoCbackTest, TestEEStatusRecoveryRediscovered) {
    nfa_hci_cb.hci_state = NFA_HCI_STATE_WAIT_NETWK_ENABLE;
    nfa_hci_ee_info_cback(NFA_EE_RECOVERY_REDISCOVERED);
    EXPECT_EQ(nfa_hci_cb.num_nfcee, 0);
}

TEST_F(NfaHciEeInfoCbackTest, TestEEStatusModeSetComplete) {
    nfa_hci_cb.hci_state = NFA_HCI_STATE_WAIT_NETWK_ENABLE;
    nfa_hci_ee_info_cback(NFA_EE_MODE_SET_COMPLETE);
    EXPECT_EQ(nfa_hci_cb.num_nfcee, 0);
}

TEST_F(NfaHciEeInfoCbackTest, TestEEStatusRecoveryInit) {
    nfa_hci_ee_info_cback(NFA_EE_RECOVERY_INIT);
    EXPECT_EQ(nfa_hci_cb.hci_state, NFA_HCI_STATE_EE_RECOVERY);
    EXPECT_TRUE(nfa_ee_cb.isDiscoveryStopped);
}
