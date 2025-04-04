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
#include <gmock/gmock.h>
#include <cstring>
#include "nfa_hci_api.cc"

class NFA_HciAddStaticPipeTest : public::testing::Test {
protected:
    void SetUp() override {
        memset(&nfa_hci_cb, 0, sizeof(nfa_hci_cb));
    }
};

TEST_F(NFA_HciAddStaticPipeTest, InvalidHciHandle) {
    tNFA_HANDLE invalid_handle = 0x1234;
    uint8_t host = 0x01;
    uint8_t gate = 0x05;
    uint8_t pipe = 0x03;
    tNFA_STATUS result = NFA_HciAddStaticPipe(invalid_handle, host, gate, pipe);
    EXPECT_EQ(result, NFA_STATUS_FAILED);
}

TEST_F(NFA_HciAddStaticPipeTest, InactiveHost) {
    tNFA_HANDLE hci_handle = 0x1000;
    uint8_t inactive_host = 0x02;
    uint8_t gate = 0x05;
    uint8_t pipe = 0x03;
    tNFA_STATUS result = NFA_HciAddStaticPipe(hci_handle, inactive_host, gate, pipe);
    EXPECT_EQ(result, NFA_STATUS_FAILED);
}

TEST_F(NFA_HciAddStaticPipeTest, InvalidGate) {
    tNFA_HANDLE hci_handle = 0x1000;
    uint8_t host = 0x01;
    uint8_t invalid_gate = NFA_HCI_LAST_HOST_SPECIFIC_GATE - 1;
    uint8_t pipe = 0x03;
    tNFA_STATUS result = NFA_HciAddStaticPipe(hci_handle, host, invalid_gate, pipe);
    EXPECT_EQ(result, NFA_STATUS_FAILED);
}

TEST_F(NFA_HciAddStaticPipeTest, InvalidPipe) {
    tNFA_HANDLE hci_handle = 0x1000;
    uint8_t host = 0x01;
    uint8_t gate = 0x05;
    uint8_t invalid_pipe = NFA_HCI_LAST_DYNAMIC_PIPE - 1;
    tNFA_STATUS result = NFA_HciAddStaticPipe(hci_handle, host, gate, invalid_pipe);
    EXPECT_EQ(result, NFA_STATUS_FAILED);
}

TEST_F(NFA_HciAddStaticPipeTest, AddStaticPipeSuccess) {
    tNFA_HANDLE hci_handle = 0x1000;
    uint8_t host = 0x01;
    uint8_t gate = 0x10;
    uint8_t pipe = 0x20;
    tNFA_HCI_API_ADD_STATIC_PIPE_EVT* p_msg = (tNFA_HCI_API_ADD_STATIC_PIPE_EVT*)malloc(
            sizeof(tNFA_HCI_API_ADD_STATIC_PIPE_EVT));
    if (p_msg != nullptr) {
        p_msg->hdr.event = NFA_HCI_API_ADD_STATIC_PIPE_EVT;
        p_msg->hci_handle = hci_handle;
        p_msg->host = host;
        p_msg->gate = gate;
        p_msg->pipe = pipe;
        nfa_sys_sendmsg(p_msg);
        free(p_msg);
        EXPECT_EQ(NFA_HciAddStaticPipe(hci_handle, host, gate, pipe), NFA_STATUS_FAILED);
    }
}

TEST_F(NFA_HciAddStaticPipeTest, GkiGetbufFailure) {
    tNFA_HANDLE hci_handle = 0x1000;
    uint8_t host = 0x01;
    uint8_t gate = 0x10;
    uint8_t pipe = 0x20;
    tNFA_HCI_API_ADD_STATIC_PIPE_EVT* p_msg = nullptr;
    if (p_msg == nullptr) {
        EXPECT_EQ(NFA_HciAddStaticPipe(hci_handle, host, gate, pipe), NFA_STATUS_FAILED);
    }
}

TEST_F(NFA_HciAddStaticPipeTest, HciStateDisabled) {
    tNFA_HANDLE hci_handle = 0x1000;
    uint8_t host = 0x01;
    uint8_t gate = 0x10;
    uint8_t pipe = 0x20;
    nfa_hci_cb.hci_state = NFA_HCI_STATE_DISABLED;
    tNFA_STATUS result = NFA_HciAddStaticPipe(hci_handle, host, gate, pipe);
    EXPECT_EQ(result, NFA_STATUS_FAILED);
}

class NFA_HciAllocGateTest : public::testing::Test {
protected:
    void SetUp() override {
        memset(&nfa_hci_cb, 0, sizeof(nfa_hci_cb));
    }
};

TEST_F(NFA_HciAllocGateTest, InvalidHciHandle) {
    tNFA_HANDLE invalid_handle = 0x1234;
    uint8_t gate = 0x05;
    tNFA_STATUS result = NFA_HciAllocGate(invalid_handle, gate);
    EXPECT_EQ(result, NFA_STATUS_FAILED);
}

TEST_F(NFA_HciAllocGateTest, InvalidGateBelow) {
    tNFA_HANDLE hci_handle = 0x1000;
    uint8_t invalid_gate = NFA_HCI_FIRST_HOST_SPECIFIC_GENERIC_GATE - 1;
    tNFA_STATUS result = NFA_HciAllocGate(hci_handle, invalid_gate);
    EXPECT_EQ(result, NFA_STATUS_FAILED);
}

TEST_F(NFA_HciAllocGateTest, InvalidGateAbove) {
    tNFA_HANDLE hci_handle = 0x1000;
    uint16_t invalid_gate = NFA_HCI_LAST_PROP_GATE + 1;
    tNFA_STATUS result = NFA_HciAllocGate(hci_handle, static_cast<uint8_t>(invalid_gate));
    EXPECT_EQ(result, NFA_STATUS_FAILED);
}

TEST_F(NFA_HciAllocGateTest, InvalidConnectivityGate) {
    tNFA_HANDLE hci_handle = 0x1000;
    uint8_t invalid_gate = NFA_HCI_CONNECTIVITY_GATE;
    tNFA_STATUS result = NFA_HciAllocGate(hci_handle, invalid_gate);
    EXPECT_EQ(result, NFA_STATUS_FAILED);
}

TEST_F(NFA_HciAllocGateTest, ValidGateAllocation) {
    tNFA_HANDLE hci_handle = 0x1000;
    uint8_t valid_gate = 0x10;
    tNFA_HCI_API_ALLOC_GATE* p_msg = (tNFA_HCI_API_ALLOC_GATE*)malloc(sizeof(
            tNFA_HCI_API_ALLOC_GATE));
    if (p_msg != nullptr) {
        p_msg->hdr.event = NFA_HCI_API_ALLOC_GATE_EVT;
        p_msg->hci_handle = hci_handle;
        p_msg->gate = valid_gate;
        nfa_sys_sendmsg(p_msg);
        free(p_msg);
        EXPECT_EQ(NFA_HciAllocGate(hci_handle, valid_gate), NFA_STATUS_FAILED);
    }
}

TEST_F(NFA_HciAllocGateTest, GkiGetbufFailure) {
    tNFA_HANDLE hci_handle = 0x1000;
    uint8_t valid_gate = 0x10;
    tNFA_HCI_API_ALLOC_GATE* p_msg = nullptr;
    if (p_msg == nullptr) {
        EXPECT_EQ(NFA_HciAllocGate(hci_handle, valid_gate), NFA_STATUS_FAILED);
    }
}

TEST_F(NFA_HciAllocGateTest, HciStateDisabled) {
    tNFA_HANDLE hci_handle = 0x1000;
    uint8_t valid_gate = 0x10;
    nfa_hci_cb.hci_state = NFA_HCI_STATE_DISABLED;
    tNFA_STATUS result = NFA_HciAllocGate(hci_handle, valid_gate);
    EXPECT_EQ(result, NFA_STATUS_FAILED);
}

class NFA_HciClosePipeTest : public::testing::Test {
protected:
    void SetUp() override {
        memset(&nfa_hci_cb, 0, sizeof(nfa_hci_cb));
        nfa_hci_cb.b_low_power_mode = false;
    }
};

TEST_F(NFA_HciClosePipeTest, InvalidHciHandle) {
    tNFA_HANDLE invalid_handle = 0x1234;
    uint8_t pipe = 0x05;
    tNFA_STATUS result = NFA_HciClosePipe(invalid_handle, pipe);
    EXPECT_EQ(result, NFA_STATUS_FAILED);
}

TEST_F(NFA_HciClosePipeTest, InvalidPipeBelow) {
    tNFA_HANDLE hci_handle = 0x1000;
    uint8_t invalid_pipe = NFA_HCI_FIRST_DYNAMIC_PIPE - 1;
    tNFA_STATUS result = NFA_HciClosePipe(hci_handle, invalid_pipe);
    EXPECT_EQ(result, NFA_STATUS_FAILED);
}

TEST_F(NFA_HciClosePipeTest, InvalidPipeAbove) {
    tNFA_HANDLE hci_handle = 0x1000;
    uint8_t invalid_pipe = NFA_HCI_LAST_DYNAMIC_PIPE + 1;
    tNFA_STATUS result = NFA_HciClosePipe(hci_handle, invalid_pipe);
    EXPECT_EQ(result, NFA_STATUS_FAILED);
}

TEST_F(NFA_HciClosePipeTest, ValidPipeClosure) {
    tNFA_HANDLE hci_handle = 0x1000;
    uint8_t valid_pipe = 0x10;
    tNFA_HCI_API_CLOSE_PIPE_EVT* p_msg = (tNFA_HCI_API_CLOSE_PIPE_EVT*)malloc(
            sizeof(tNFA_HCI_API_CLOSE_PIPE_EVT));
    if (p_msg != nullptr) {
        p_msg->hdr.event = NFA_HCI_API_CLOSE_PIPE_EVT;
        p_msg->hci_handle = hci_handle;
        p_msg->pipe = valid_pipe;
        nfa_sys_sendmsg(p_msg);
        free(p_msg);
        EXPECT_EQ(NFA_HciClosePipe(hci_handle, valid_pipe), NFA_STATUS_FAILED);
    }
}

TEST_F(NFA_HciClosePipeTest, GkiGetbufFailure) {
    tNFA_HANDLE hci_handle = 0x1000;
    uint8_t valid_pipe = 0x10;
    tNFA_HCI_API_CLOSE_PIPE_EVT* p_msg = nullptr;
    if (p_msg == nullptr) {
        EXPECT_EQ(NFA_HciClosePipe(hci_handle, valid_pipe), NFA_STATUS_FAILED);
    }
}

TEST_F(NFA_HciClosePipeTest, HciStateDisabled) {
    tNFA_HANDLE hci_handle = 0x1000;
    uint8_t valid_pipe = 0x10;
    nfa_hci_cb.hci_state = NFA_HCI_STATE_DISABLED;
    tNFA_STATUS result = NFA_HciClosePipe(hci_handle, valid_pipe);
    EXPECT_EQ(result, NFA_STATUS_FAILED);
}

TEST_F(NFA_HciClosePipeTest, LowPowerMode) {
    tNFA_HANDLE hci_handle = 0x1000;
    uint8_t valid_pipe = 0x10;
    nfa_hci_cb.b_low_power_mode = true;
    tNFA_STATUS result = NFA_HciClosePipe(hci_handle, valid_pipe);
    EXPECT_EQ(result, NFA_STATUS_FAILED);
}

class NFA_HciCreatePipeTest : public::testing::Test {
protected:
    void SetUp() override {
        memset(&nfa_hci_cb, 0, sizeof(nfa_hci_cb));
        nfa_hci_cb.b_low_power_mode = false;
    }
};

TEST_F(NFA_HciCreatePipeTest, InvalidHciHandle) {
    tNFA_HANDLE invalid_handle = 0x1234;
    uint8_t source_gate = 0x01;
    uint8_t dest_host = 0x02;
    uint8_t dest_gate = 0x05;
    tNFA_STATUS result = NFA_HciCreatePipe(invalid_handle, source_gate, dest_host, dest_gate);
    EXPECT_EQ(result, NFA_STATUS_FAILED);
}

TEST_F(NFA_HciCreatePipeTest, InvalidSourceGate) {
    tNFA_HANDLE hci_handle = 0x1000;
    uint8_t invalid_source_gate = NFA_HCI_FIRST_HOST_SPECIFIC_GENERIC_GATE - 1;
    uint8_t dest_host = 0x02;
    uint8_t dest_gate = 0x05;
    tNFA_STATUS result = NFA_HciCreatePipe(hci_handle, invalid_source_gate, dest_host, dest_gate);
    EXPECT_EQ(result, NFA_STATUS_FAILED);
}

TEST_F(NFA_HciCreatePipeTest, InvalidDestGate) {
    tNFA_HANDLE hci_handle = 0x1000;
    uint8_t source_gate = 0x01;
    uint16_t invalid_dest_gate = NFA_HCI_LAST_PROP_GATE + 1;
    uint8_t dest_host = 0x02;
    tNFA_STATUS result = NFA_HciCreatePipe(
            hci_handle, source_gate, dest_host, static_cast<uint8_t>(invalid_dest_gate));
    EXPECT_EQ(result, NFA_STATUS_FAILED);
}

TEST_F(NFA_HciCreatePipeTest, InactiveDestHost) {
    tNFA_HANDLE hci_handle = 0x1000;
    uint8_t source_gate = 0x01;
    uint8_t dest_gate = 0x05;
    uint8_t inactive_dest_host = 0x03;
    EXPECT_FALSE(nfa_hciu_is_active_host(inactive_dest_host));
    tNFA_STATUS result = NFA_HciCreatePipe(hci_handle, source_gate, inactive_dest_host, dest_gate);
    EXPECT_EQ(result, NFA_STATUS_FAILED);
}

TEST_F(NFA_HciCreatePipeTest, ValidPipeCreation) {
    tNFA_HANDLE hci_handle = 0x1000;
    uint8_t source_gate = 0x01;
    uint8_t dest_host = 0x02;
    uint8_t dest_gate = 0x05;
    nfa_hci_cb.hci_state |= NFA_HCI_STATE_DISABLED;
    nfa_hci_cb.b_low_power_mode = false;
    tNFA_HCI_API_CREATE_PIPE_EVT* p_msg = (tNFA_HCI_API_CREATE_PIPE_EVT*)malloc(
            sizeof(tNFA_HCI_API_CREATE_PIPE_EVT));
    if (p_msg != nullptr) {
        p_msg->hdr.event = NFA_HCI_API_CREATE_PIPE_EVT;
        p_msg->hci_handle = hci_handle;
        p_msg->source_gate = source_gate;
        p_msg->dest_host = dest_host;
        p_msg->dest_gate = dest_gate;
        nfa_sys_sendmsg(p_msg);
        tNFA_STATUS result = NFA_HciCreatePipe(hci_handle, source_gate, dest_host, dest_gate);
        free(p_msg);
        EXPECT_EQ(result, NFA_STATUS_FAILED);
    }
}

TEST_F(NFA_HciCreatePipeTest, GkiGetbufFailure) {
    tNFA_HANDLE hci_handle = 0x1000;
    uint8_t source_gate = 0x01;
    uint8_t dest_host = 0x02;
    uint8_t dest_gate = 0x05;
    tNFA_HCI_API_CREATE_PIPE_EVT* p_msg = nullptr;
    if (p_msg == nullptr) {
        EXPECT_EQ(NFA_HciCreatePipe(
                hci_handle, source_gate, dest_host, dest_gate), NFA_STATUS_FAILED);
    }
}

TEST_F(NFA_HciCreatePipeTest, HciStateDisabled) {
    tNFA_HANDLE hci_handle = 0x1000;
    uint8_t source_gate = 0x01;
    uint8_t dest_host = 0x02;
    uint8_t dest_gate = 0x05;
    nfa_hci_cb.hci_state = NFA_HCI_STATE_DISABLED;
    tNFA_STATUS result = NFA_HciCreatePipe(hci_handle, source_gate, dest_host, dest_gate);
    EXPECT_EQ(result, NFA_STATUS_FAILED);
}

TEST_F(NFA_HciCreatePipeTest, LowPowerMode) {
    tNFA_HANDLE hci_handle = 0x1000;
    uint8_t source_gate = 0x01;
    uint8_t dest_host = 0x02;
    uint8_t dest_gate = 0x05;
    nfa_hci_cb.b_low_power_mode = true;
    tNFA_STATUS result = NFA_HciCreatePipe(hci_handle, source_gate, dest_host, dest_gate);
    EXPECT_EQ(result, NFA_STATUS_FAILED);
}

class NFA_HciDeallocGateTest : public ::testing::Test {
protected:
    void SetUp() override {
        nfa_hci_cb.hci_state = NFA_HCI_STATE_IDLE;
        nfa_hci_cb.b_low_power_mode = false;
    }
};

TEST_F(NFA_HciDeallocGateTest, ValidDeallocation) {
    tNFA_HANDLE hci_handle = 0x1000;
    uint8_t gate = 0x01;
    tNFA_HCI_API_DEALLOC_GATE* p_msg = (tNFA_HCI_API_DEALLOC_GATE*)malloc(
            sizeof(tNFA_HCI_API_DEALLOC_GATE));
    if (p_msg != nullptr) {
        p_msg->hdr.event = NFA_HCI_API_DEALLOC_GATE_EVT;
        p_msg->hci_handle = hci_handle;
        p_msg->gate = gate;
        nfa_sys_sendmsg(p_msg);
        tNFA_STATUS result = NFA_HciDeallocGate(hci_handle, gate);
        free(p_msg);
        EXPECT_EQ(result, NFA_STATUS_FAILED);
    }
}

TEST_F(NFA_HciDeallocGateTest, InvalidHciHandle) {
    tNFA_HANDLE invalid_hci_handle = 0x2000;
    uint8_t gate = 0x01;
    tNFA_STATUS result = NFA_HciDeallocGate(invalid_hci_handle, gate);
    EXPECT_EQ(result, NFA_STATUS_FAILED);
}

TEST_F(NFA_HciDeallocGateTest, InvalidGateBelowRange) {
    tNFA_HANDLE hci_handle = 0x1000;
    uint8_t invalid_gate = NFA_HCI_FIRST_HOST_SPECIFIC_GENERIC_GATE - 1;
    tNFA_STATUS result = NFA_HciDeallocGate(hci_handle, invalid_gate);
    EXPECT_EQ(result, NFA_STATUS_FAILED);
}

TEST_F(NFA_HciDeallocGateTest, InvalidGateAboveRange) {
    tNFA_HANDLE hci_handle = 0x1000;
    uint16_t invalid_gate = NFA_HCI_LAST_PROP_GATE + 1;
    tNFA_STATUS result = NFA_HciDeallocGate(hci_handle, static_cast<uint8_t>(invalid_gate));
    EXPECT_EQ(result, NFA_STATUS_FAILED);
}

TEST_F(NFA_HciDeallocGateTest, ConnectivityGate) {
    tNFA_HANDLE hci_handle = 0x1000;
    uint8_t gate = NFA_HCI_CONNECTIVITY_GATE;
    tNFA_STATUS result = NFA_HciDeallocGate(hci_handle, gate);
    EXPECT_EQ(result, NFA_STATUS_FAILED);
}

TEST_F(NFA_HciDeallocGateTest, LowPowerMode) {
    tNFA_HANDLE hci_handle = 0x1000;
    uint8_t gate = 0x01;
    nfa_hci_cb.b_low_power_mode = true;
    tNFA_STATUS result = NFA_HciDeallocGate(hci_handle, gate);
    EXPECT_EQ(result, NFA_STATUS_FAILED);
}

TEST_F(NFA_HciDeallocGateTest, DisabledHciState) {
    tNFA_HANDLE hci_handle = 0x1000;
    uint8_t gate = 0x01;
    nfa_hci_cb.hci_state = NFA_HCI_STATE_DISABLED;
    tNFA_STATUS result = NFA_HciDeallocGate(hci_handle, gate);
    EXPECT_EQ(result, NFA_STATUS_FAILED);
}

class NFA_HciDebugTest : public ::testing::Test {
protected:
    void SetUp() override {
        memset(&nfa_hci_cb, 0, sizeof(nfa_hci_cb));
        strcpy((char*)nfa_hci_cb.cfg.reg_app_names[0], "TestApp1");
        strcpy((char*)nfa_hci_cb.cfg.reg_app_names[1], "TestApp2");
        nfa_hci_cb.cfg.dyn_gates[0].gate_id = 0x01;
        nfa_hci_cb.cfg.dyn_gates[0].gate_owner = 0x1234;
        nfa_hci_cb.cfg.dyn_gates[0].pipe_inx_mask = 0x000000FF;
        nfa_hci_cb.cfg.dyn_pipes[0].pipe_id = 0x02;
        nfa_hci_cb.cfg.dyn_pipes[0].pipe_state = 1;
        nfa_hci_cb.cfg.dyn_pipes[0].local_gate = 0x10;
        nfa_hci_cb.cfg.dyn_pipes[0].dest_gate = 0x20;
        nfa_hci_cb.cfg.dyn_pipes[0].dest_host = 0x30;
    }
};

TEST_F(NFA_HciDebugTest, DebugSimHciEvent) {
    uint8_t action = NFA_HCI_DEBUG_SIM_HCI_EVENT;
    uint8_t size = 4;
    uint8_t p_data[4] = {0x01, 0x02, 0x03, 0x04};
    NFA_HciDebug(action, size, p_data);
    EXPECT_EQ(nfa_hci_cb.hci_state, NFA_HCI_STATE_DISABLED);
}

TEST_F(NFA_HciDebugTest, DebugEnableLoopback) {
    uint8_t action = NFA_HCI_DEBUG_ENABLE_LOOPBACK;
    uint8_t size = 0;
    uint8_t* p_data = nullptr;
    EXPECT_EQ(HCI_LOOPBACK_DEBUG, NFA_HCI_DEBUG_OFF);
    NFA_HciDebug(action, size, p_data);
    EXPECT_EQ(HCI_LOOPBACK_DEBUG, NFA_HCI_DEBUG_ON);
}

TEST_F(NFA_HciDebugTest, DebugDisableLoopback) {
    uint8_t action = NFA_HCI_DEBUG_DISABLE_LOOPBACK;
    uint8_t size = 0;
    uint8_t* p_data = nullptr;
    EXPECT_EQ(HCI_LOOPBACK_DEBUG, NFA_HCI_DEBUG_ON);
    NFA_HciDebug(action, size, p_data);
    EXPECT_EQ(HCI_LOOPBACK_DEBUG, NFA_HCI_DEBUG_OFF);
}

TEST_F(NFA_HciDebugTest, InvalidAction) {
    uint8_t action = 0xFF;
    uint8_t size = 0;
    uint8_t* p_data = nullptr;
    NFA_HciDebug(action, size, p_data);
    EXPECT_EQ(HCI_LOOPBACK_DEBUG, NFA_HCI_DEBUG_OFF);
}

class NFA_HciDeletePipeTest : public ::testing::Test {
protected:
    void SetUp() override {
        nfa_hci_cb.hci_state = NFA_HCI_STATE_STARTUP;
        nfa_hci_cb.b_low_power_mode = false;
    }
};

TEST_F(NFA_HciDeletePipeTest, InvalidHciHandle) {
    tNFA_HANDLE invalid_hci_handle = 0x1234;
    uint8_t valid_pipe = 0x01;
    tNFA_STATUS result = NFA_HciDeletePipe(invalid_hci_handle, valid_pipe);
    EXPECT_EQ(result, NFA_STATUS_FAILED);
}

TEST_F(NFA_HciDeletePipeTest, InvalidPipeOutOfRange) {
    tNFA_HANDLE valid_hci_handle = 0x1001;
    uint8_t invalid_pipe = 0xFF;
    tNFA_STATUS result = NFA_HciDeletePipe(valid_hci_handle, invalid_pipe);
    EXPECT_EQ(result, NFA_STATUS_FAILED);
}

TEST_F(NFA_HciDeletePipeTest, PipeBelowMinimumRange) {
    tNFA_HANDLE valid_hci_handle = 0x1001;
    uint8_t invalid_pipe = NFA_HCI_FIRST_DYNAMIC_PIPE - 1;
    tNFA_STATUS result = NFA_HciDeletePipe(valid_hci_handle, invalid_pipe);
    EXPECT_EQ(result, NFA_STATUS_FAILED);
}

TEST_F(NFA_HciDeletePipeTest, PipeAboveMaximumRange) {
    tNFA_HANDLE valid_hci_handle = 0x1001;
    uint16_t invalid_pipe = NFA_HCI_LAST_DYNAMIC_PIPE + 1;
    tNFA_STATUS result = NFA_HciDeletePipe(valid_hci_handle, static_cast<uint8_t>(invalid_pipe));
    EXPECT_EQ(result, NFA_STATUS_FAILED);
}

TEST_F(NFA_HciDeletePipeTest, HciStateDisabled) {
    tNFA_HANDLE valid_hci_handle = 0x1001;
    uint8_t valid_pipe = NFA_HCI_FIRST_DYNAMIC_PIPE;
    nfa_hci_cb.hci_state = NFA_HCI_STATE_DISABLED;
    tNFA_STATUS result = NFA_HciDeletePipe(valid_hci_handle, valid_pipe);
    EXPECT_EQ(result, NFA_STATUS_FAILED);
}

TEST_F(NFA_HciDeletePipeTest, LowPowerModeEnabled) {
    tNFA_HANDLE valid_hci_handle = 0x1001;
    uint8_t valid_pipe = NFA_HCI_FIRST_DYNAMIC_PIPE;
    nfa_hci_cb.b_low_power_mode = true;
    tNFA_STATUS result = NFA_HciDeletePipe(valid_hci_handle, valid_pipe);
    EXPECT_EQ(result, NFA_STATUS_FAILED);
}

class NFA_HciDeregisterTest : public ::testing::Test {
protected:
    void SetUp() override {
        nfa_hci_cb.hci_state = NFA_HCI_STATE_STARTUP;
        nfa_hci_cb.cfg.reg_app_names[0][0] = 'A';
        nfa_hci_cb.cfg.reg_app_names[0][1] = 'p';
        nfa_hci_cb.cfg.reg_app_names[0][2] = 'p';
        nfa_hci_cb.cfg.reg_app_names[0][3] = '1';
        nfa_hci_cb.cfg.reg_app_names[0][4] = '\0';
    }
};

TEST_F(NFA_HciDeregisterTest, NullAppName) {
    char* null_app_name = nullptr;
    tNFA_STATUS result = NFA_HciDeregister(null_app_name);
    EXPECT_EQ(result, NFA_STATUS_FAILED);
}

TEST_F(NFA_HciDeregisterTest, AppNameTooLong) {
    char long_app_name[NFA_MAX_HCI_APP_NAME_LEN + 1];
    memset(long_app_name, 'A', sizeof(long_app_name) - 1);
    long_app_name[NFA_MAX_HCI_APP_NAME_LEN] = '\0';
    tNFA_STATUS result = NFA_HciDeregister(long_app_name);
    EXPECT_EQ(result, NFA_STATUS_FAILED);
}

TEST_F(NFA_HciDeregisterTest, AppNotFound) {
    char app_name_not_found[] = "AppNotFound";
    tNFA_STATUS result = NFA_HciDeregister(app_name_not_found);
    EXPECT_EQ(result, NFA_STATUS_FAILED);
}

TEST_F(NFA_HciDeregisterTest, HciStateDisabled) {
    char app_name[] = "App1";
    nfa_hci_cb.hci_state = NFA_HCI_STATE_DISABLED;
    tNFA_STATUS result = NFA_HciDeregister(app_name);
    EXPECT_EQ(result, NFA_STATUS_FAILED);
}

TEST_F(NFA_HciDeregisterTest, ValidDeregister) {
    char app_name[] = "App1";
    nfa_hci_cb.hci_state = NFA_HCI_STATE_STARTUP;
    tNFA_STATUS result = NFA_HciDeregister(app_name);
    EXPECT_EQ(result, NFA_STATUS_OK);
}

class NFA_HciApiTest : public ::testing::Test {
protected:
    void SetUp() override {
        memset(&nfa_hci_cb, 0, sizeof(nfa_hci_cb));
        nfa_hci_cb.hci_state = NFA_HCI_STATE_STARTUP;
    }
};

TEST_F(NFA_HciApiTest, ValidHciHandleAndState) {
    tNFA_HANDLE valid_hci_handle = 0x01;
    nfa_hci_cb.hci_state = NFA_HCI_STATE_STARTUP;
    tNFA_STATUS result = NFA_HciGetGateAndPipeList(valid_hci_handle);
    EXPECT_EQ(result, NFA_STATUS_FAILED);
}

TEST_F(NFA_HciApiTest, InvalidHciHandle) {
    tNFA_HANDLE invalid_hci_handle = 0xFF;
    tNFA_STATUS result = NFA_HciGetGateAndPipeList(invalid_hci_handle);
    EXPECT_EQ(result, NFA_STATUS_FAILED);
}

TEST_F(NFA_HciApiTest, HciStateDisabled) {
    tNFA_HANDLE valid_hci_handle = 0x01;
    nfa_hci_cb.hci_state = NFA_HCI_STATE_DISABLED;
    tNFA_STATUS result = NFA_HciGetGateAndPipeList(valid_hci_handle);
    EXPECT_EQ(result, NFA_STATUS_FAILED);
}

class NFA_HciOpenPipeTest : public ::testing::Test {
protected:
    void SetUp() override {
        memset(&nfa_hci_cb, 0, sizeof(nfa_hci_cb));
        nfa_hci_cb.b_low_power_mode = false;
        nfa_hci_cb.hci_state = NFA_HCI_STATE_STARTUP;
    }
};

TEST_F(NFA_HciOpenPipeTest, InvalidPipeBelow) {
    tNFA_HANDLE hci_handle = 0x1000;
    uint8_t invalid_pipe = NFA_HCI_FIRST_DYNAMIC_PIPE - 1;
    tNFA_STATUS result = NFA_HciOpenPipe(hci_handle, invalid_pipe);
    EXPECT_EQ(result, NFA_STATUS_FAILED);
}

TEST_F(NFA_HciOpenPipeTest, InvalidPipeAbove) {
    tNFA_HANDLE hci_handle = 0x1000;
    uint16_t invalid_pipe = NFA_HCI_LAST_DYNAMIC_PIPE + 1;
    tNFA_STATUS result = NFA_HciOpenPipe(hci_handle, static_cast<uint8_t>(invalid_pipe));
    EXPECT_EQ(result, NFA_STATUS_FAILED);
}

TEST_F(NFA_HciOpenPipeTest, ValidPipeOpenRequest) {
    tNFA_HANDLE hci_handle = 0x1000;
    uint8_t valid_pipe = 0x10;
    tNFA_HCI_API_OPEN_PIPE_EVT* p_msg = (tNFA_HCI_API_OPEN_PIPE_EVT*)malloc(
            sizeof(tNFA_HCI_API_OPEN_PIPE_EVT));
    ASSERT_NE(p_msg, nullptr);
    p_msg->hdr.event = NFA_HCI_API_OPEN_PIPE_EVT;
    p_msg->hci_handle = hci_handle;
    p_msg->pipe = valid_pipe;
    nfa_sys_sendmsg(p_msg);
    free(p_msg);
    EXPECT_EQ(NFA_HciOpenPipe(hci_handle, valid_pipe), NFA_STATUS_FAILED);
}

TEST_F(NFA_HciOpenPipeTest, GkiGetbufFailure) {
    tNFA_HANDLE hci_handle = 0x1000;
    uint8_t valid_pipe = 0x10;
    tNFA_HCI_API_OPEN_PIPE_EVT* p_msg = nullptr;
    if (p_msg == nullptr) {
        EXPECT_EQ(NFA_HciOpenPipe(hci_handle, valid_pipe), NFA_STATUS_FAILED);
    }
}

TEST_F(NFA_HciOpenPipeTest, HciStateDisabled) {
    tNFA_HANDLE hci_handle = 0x1000;
    uint8_t valid_pipe = 0x10;
    nfa_hci_cb.hci_state = NFA_HCI_STATE_DISABLED;
    tNFA_STATUS result = NFA_HciOpenPipe(hci_handle, valid_pipe);
    EXPECT_EQ(result, NFA_STATUS_FAILED);
}

TEST_F(NFA_HciOpenPipeTest, LowPowerMode) {
    tNFA_HANDLE hci_handle = 0x1000;
    uint8_t valid_pipe = 0x10;
    nfa_hci_cb.b_low_power_mode = true;
    tNFA_STATUS result = NFA_HciOpenPipe(hci_handle, valid_pipe);
    EXPECT_EQ(result, NFA_STATUS_FAILED);
}

class NFA_HciSendEventTest : public ::testing::Test {
protected:
    void SetUp() override {
        memset(&nfa_hci_cb, 0, sizeof(nfa_hci_cb));
        nfa_hci_cb.b_low_power_mode = false;
    }
};

TEST_F(NFA_HciSendEventTest, InvalidHciHandle) {
    tNFA_HANDLE invalid_handle = 0x1234;
    uint8_t pipe = 0x10;
    uint8_t evt_code = 0x01;
    uint16_t evt_size = 0;
    uint8_t* p_data = nullptr;
    uint16_t rsp_size = 0;
    uint8_t* p_rsp_buf = nullptr;
    uint16_t rsp_timeout = 0;
    tNFA_STATUS result = NFA_HciSendEvent(
            invalid_handle, pipe, evt_code, evt_size, p_data, rsp_size, p_rsp_buf, rsp_timeout);
    EXPECT_EQ(result, NFA_STATUS_FAILED);
}

TEST_F(NFA_HciSendEventTest, InvalidPipeBelow) {
    tNFA_HANDLE hci_handle = 0x1000;
    uint8_t invalid_pipe = NFA_HCI_FIRST_DYNAMIC_PIPE - 1;
    uint8_t evt_code = 0x01;
    uint16_t evt_size = 0;
    uint8_t* p_data = nullptr;
    uint16_t rsp_size = 0;
    uint8_t* p_rsp_buf = nullptr;
    uint16_t rsp_timeout = 0;
    tNFA_STATUS result = NFA_HciSendEvent(
            hci_handle, invalid_pipe, evt_code, evt_size, p_data, rsp_size, p_rsp_buf, rsp_timeout);
    EXPECT_EQ(result, NFA_STATUS_FAILED);
}

TEST_F(NFA_HciSendEventTest, InvalidEventData) {
    tNFA_HANDLE hci_handle = 0x1000;
    uint8_t pipe = 0x10;
    uint8_t evt_code = 0x01;
    uint16_t evt_size = 1;
    uint8_t* p_data = nullptr;
    uint16_t rsp_size = 0;
    uint8_t* p_rsp_buf = nullptr;
    uint16_t rsp_timeout = 0;
    tNFA_STATUS result = NFA_HciSendEvent(
            hci_handle, pipe, evt_code, evt_size, p_data, rsp_size, p_rsp_buf, rsp_timeout);
    EXPECT_EQ(result, NFA_STATUS_FAILED);
}

TEST_F(NFA_HciSendEventTest, InvalidResponseBuffer) {
    tNFA_HANDLE hci_handle = 0x1000;
    uint8_t pipe = 0x10;
    uint8_t evt_code = 0x01;
    uint16_t evt_size = 0;
    uint8_t* p_data = nullptr;
    uint16_t rsp_size = 1;
    uint8_t* p_rsp_buf = nullptr;
    uint16_t rsp_timeout = 0;
    tNFA_STATUS result = NFA_HciSendEvent(
            hci_handle, pipe, evt_code, evt_size, p_data, rsp_size, p_rsp_buf, rsp_timeout);
    EXPECT_EQ(result, NFA_STATUS_FAILED);
}

TEST_F(NFA_HciSendEventTest, ValidSendEvent) {
    tNFA_HANDLE hci_handle = 0x1000;
    uint8_t pipe = 0x10;
    uint8_t evt_code = 0x01;
    uint16_t evt_size = 0;
    uint8_t* p_data = nullptr;
    uint16_t rsp_size = 0;
    uint8_t* p_rsp_buf = nullptr;
    uint16_t rsp_timeout = 0;
    tNFA_HCI_API_SEND_EVENT_EVT* p_msg = (tNFA_HCI_API_SEND_EVENT_EVT*)malloc(
            sizeof(tNFA_HCI_API_SEND_EVENT_EVT));
    if (p_msg != nullptr) {
        p_msg->hdr.event = NFA_HCI_API_SEND_EVENT_EVT;
        p_msg->hci_handle = hci_handle;
        p_msg->pipe = pipe;
        p_msg->evt_code = evt_code;
        p_msg->evt_len = evt_size;
        p_msg->p_evt_buf = p_data;
        p_msg->rsp_len = rsp_size;
        p_msg->p_rsp_buf = p_rsp_buf;
        p_msg->rsp_timeout = rsp_timeout;
        nfa_sys_sendmsg(p_msg);
        free(p_msg);
        EXPECT_EQ(NFA_HciSendEvent(
                hci_handle, pipe, evt_code, evt_size, p_data, rsp_size, p_rsp_buf, rsp_timeout),
                  NFA_STATUS_FAILED);
    }
}

TEST_F(NFA_HciSendEventTest, GkiGetbufFailure) {
    tNFA_HANDLE hci_handle = 0x1000;
    uint8_t pipe = 0x10;
    uint8_t evt_code = 0x01;
    uint16_t evt_size = 0;
    uint8_t* p_data = nullptr;
    uint16_t rsp_size = 0;
    uint8_t* p_rsp_buf = nullptr;
    uint16_t rsp_timeout = 0;
    tNFA_HCI_API_SEND_EVENT_EVT* p_msg = nullptr;
    if (p_msg == nullptr) {
        EXPECT_EQ(NFA_HciSendEvent(
                hci_handle, pipe, evt_code, evt_size, p_data, rsp_size, p_rsp_buf, rsp_timeout),
                  NFA_STATUS_FAILED);}
}

TEST_F(NFA_HciSendEventTest, HciStateDisabled) {
    tNFA_HANDLE hci_handle = 0x1000;
    uint8_t pipe = 0x10;
    uint8_t evt_code = 0x01;
    uint16_t evt_size = 0;
    uint8_t* p_data = nullptr;
    uint16_t rsp_size = 0;
    uint8_t* p_rsp_buf = nullptr;
    uint16_t rsp_timeout = 0;
    nfa_hci_cb.hci_state = NFA_HCI_STATE_DISABLED;
    tNFA_STATUS result = NFA_HciSendEvent(
            hci_handle, pipe, evt_code, evt_size, p_data, rsp_size, p_rsp_buf, rsp_timeout);
    EXPECT_EQ(result, NFA_STATUS_FAILED);
}

TEST_F(NFA_HciSendEventTest, LowPowerMode) {
    tNFA_HANDLE hci_handle = 0x1000;
    uint8_t pipe = 0x10;
    uint8_t evt_code = 0x01;
    uint16_t evt_size = 0;
    uint8_t* p_data = nullptr;
    uint16_t rsp_size = 0;
    uint8_t* p_rsp_buf = nullptr;
    uint16_t rsp_timeout = 0;
    nfa_hci_cb.b_low_power_mode = true;
    tNFA_STATUS result = NFA_HciSendEvent(
            hci_handle, pipe, evt_code, evt_size, p_data, rsp_size, p_rsp_buf, rsp_timeout);
    EXPECT_EQ(result, NFA_STATUS_FAILED);
}

class NFA_HciGetRegistryTest : public ::testing::Test {
protected:
    void SetUp() override {
        memset(&nfa_hci_cb, 0, sizeof(nfa_hci_cb));
        nfa_hci_cb.b_low_power_mode = false;
    }
};

TEST_F(NFA_HciGetRegistryTest, InvalidHciHandle) {
    tNFA_HANDLE invalid_handle = 0x1234;
    uint8_t pipe = 0x10;
    uint8_t reg_inx = 0x01;
    tNFA_STATUS result = NFA_HciGetRegistry(invalid_handle, pipe, reg_inx);
    EXPECT_EQ(result, NFA_STATUS_FAILED);
}

TEST_F(NFA_HciGetRegistryTest, InvalidPipeBelow) {
    tNFA_HANDLE hci_handle = 0x1000;
    uint8_t invalid_pipe = NFA_HCI_FIRST_DYNAMIC_PIPE - 1;
    uint8_t reg_inx = 0x01;
    tNFA_STATUS result = NFA_HciGetRegistry(hci_handle, invalid_pipe, reg_inx);
    EXPECT_EQ(result, NFA_STATUS_FAILED);
}

TEST_F(NFA_HciGetRegistryTest, ValidScenario) {
    tNFA_HANDLE hci_handle = 0x1000;
    uint8_t valid_pipe = 0x10;
    uint8_t reg_inx = 0x01;
    tNFA_HCI_API_GET_REGISTRY* p_msg = (tNFA_HCI_API_GET_REGISTRY*)malloc(
            sizeof(tNFA_HCI_API_GET_REGISTRY));
    if (p_msg != nullptr) {
        p_msg->hdr.event = NFA_HCI_API_GET_REGISTRY_EVT;
        p_msg->hci_handle = hci_handle;
        p_msg->pipe = valid_pipe;
        p_msg->reg_inx = reg_inx;
        nfa_sys_sendmsg(p_msg);
        free(p_msg);
        EXPECT_EQ(NFA_HciGetRegistry(hci_handle, valid_pipe, reg_inx), NFA_STATUS_FAILED);
    }
}

TEST_F(NFA_HciGetRegistryTest, GkiGetbufFailure) {
    tNFA_HANDLE hci_handle = 0x1000;
    uint8_t valid_pipe = 0x10;
    uint8_t reg_inx = 0x01;
    tNFA_HCI_API_GET_REGISTRY* p_msg = nullptr;
    if (p_msg == nullptr) {
        EXPECT_EQ(NFA_HciGetRegistry(hci_handle, valid_pipe, reg_inx), NFA_STATUS_FAILED);
    }
}

TEST_F(NFA_HciGetRegistryTest, HciStateDisabled) {
    tNFA_HANDLE hci_handle = 0x1000;
    uint8_t valid_pipe = 0x10;
    uint8_t reg_inx = 0x01;
    nfa_hci_cb.hci_state = NFA_HCI_STATE_DISABLED;
    tNFA_STATUS result = NFA_HciGetRegistry(hci_handle, valid_pipe, reg_inx);
    EXPECT_EQ(result, NFA_STATUS_FAILED);
}

TEST_F(NFA_HciGetRegistryTest, LowPowerMode) {
    tNFA_HANDLE hci_handle = 0x1000;
    uint8_t valid_pipe = 0x10;
    uint8_t reg_inx = 0x01;
    nfa_hci_cb.b_low_power_mode = true;
    tNFA_STATUS result = NFA_HciGetRegistry(hci_handle, valid_pipe, reg_inx);
    EXPECT_EQ(result, NFA_STATUS_FAILED);
}

