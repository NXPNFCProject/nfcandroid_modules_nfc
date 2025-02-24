#include "nfa_hci_utils.cc"
#include <gtest/gtest.h>
#include <gmock/gmock.h>

class HandleDebugLoopbackTest : public ::testing::Test {
protected:
    NFC_HDR* p_buf;
    uint8_t* p;
    uint8_t next_pipe;
    void SetUp() override {
        p_buf = (NFC_HDR*)malloc(sizeof(NFC_HDR) + 10);
        p_buf->offset = 0;
        p_buf->len = 10;
        p = (uint8_t*)(p_buf + 1) + p_buf->offset;
        next_pipe = 0x10;
    }
    void TearDown() override {
        free(p_buf);
    }
};

TEST_F(HandleDebugLoopbackTest, HandleAdmCreatePipe) {
    uint8_t type = NFA_HCI_COMMAND_TYPE;
    uint8_t instruction = NFA_HCI_ADM_CREATE_PIPE;
    handle_debug_loopback(p_buf, type, instruction);
    ASSERT_EQ(p[6], next_pipe);
    ASSERT_EQ(p[5], p[4]);
    ASSERT_EQ(p[4], p[3]);
    ASSERT_EQ(p[2], 3);
    ASSERT_EQ(p[1], (NFA_HCI_RESPONSE_TYPE << 6) | NFA_HCI_ANY_OK);
    ASSERT_EQ(p_buf->len, p_buf->offset + 7);
}

TEST_F(HandleDebugLoopbackTest, HandleAnyGetParameter) {
    uint8_t type = NFA_HCI_COMMAND_TYPE;
    uint8_t instruction = NFA_HCI_ANY_GET_PARAMETER;
    uint8_t mock_session_id[] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07};
    memcpy(nfa_hci_cb.cfg.admin_gate.session_id, mock_session_id, sizeof(mock_session_id));
    handle_debug_loopback(p_buf, type, instruction);
    ASSERT_EQ(p[1], (NFA_HCI_RESPONSE_TYPE << 6) | NFA_HCI_ANY_OK);
    ASSERT_EQ(memcmp(&p[2], mock_session_id, NFA_HCI_SESSION_ID_LEN), 0);
    ASSERT_EQ(p_buf->len, p_buf->offset + 2 + NFA_HCI_SESSION_ID_LEN);
}

TEST_F(HandleDebugLoopbackTest, HandleUnknownInstruction) {
    uint8_t type = NFA_HCI_COMMAND_TYPE;
    uint8_t instruction = 0xFF;
    handle_debug_loopback(p_buf, type, instruction);
    ASSERT_EQ(p[1], (NFA_HCI_RESPONSE_TYPE << 6) | NFA_HCI_ANY_OK);
    ASSERT_EQ(p_buf->len, p_buf->offset + 2);
}

TEST_F(HandleDebugLoopbackTest, HandleResponseType) {
    uint8_t type = NFA_HCI_RESPONSE_TYPE;
    uint8_t instruction = NFA_HCI_ADM_CREATE_PIPE;
    handle_debug_loopback(p_buf, type, instruction);
}

TEST_F(HandleDebugLoopbackTest, SendMessage) {
    uint8_t type = NFA_HCI_COMMAND_TYPE;
    uint8_t instruction = NFA_HCI_ADM_CREATE_PIPE;
    handle_debug_loopback(p_buf, type, instruction);
}

#define NFA_HCI_MAX_PIPE_COUNT 10
#define NFA_HCI_PIPE_OPEN 1
class NfaHciuAddPipeToStaticGateTest : public ::testing::Test {
protected:
    void SetUp() override {
        memset(&nfa_hci_cb, 0, sizeof(nfa_hci_cb));
        for (int i = 0; i < NFA_HCI_MAX_PIPE_COUNT; ++i) {
            nfa_hci_cb.cfg.dyn_pipes[i].pipe_id = 0;
            nfa_hci_cb.cfg.dyn_pipes[i].pipe_state = NFA_HCI_PIPE_CLOSED;
        }
        nfa_hci_cb.cfg.id_mgmt_gate.pipe_inx_mask = 0;
    }
};

TEST_F(NfaHciuAddPipeToStaticGateTest, SuccessWhenPipeAllocatedForIdMgmtGate) {
    uint8_t local_gate = NFA_HCI_IDENTITY_MANAGEMENT_GATE;
    uint8_t pipe_id = 0x01;
    uint8_t dest_host = 0x02;
    uint8_t dest_gate = 0x03;
    tNFA_HCI_RESPONSE response = nfa_hciu_add_pipe_to_static_gate(
            local_gate, pipe_id, dest_host, dest_gate);
    ASSERT_EQ(response, NFA_HCI_ANY_OK);
    uint8_t pipe_index = 0;
    uint32_t expected_mask = (1 << pipe_index);
    ASSERT_EQ(nfa_hci_cb.cfg.id_mgmt_gate.pipe_inx_mask & expected_mask, expected_mask);
}

TEST_F(NfaHciuAddPipeToStaticGateTest, SuccessWhenPipeAllocatedForOtherGate) {
    uint8_t local_gate = 0x04;
    uint8_t pipe_id = 0x02;
    uint8_t dest_host = 0x03;
    uint8_t dest_gate = 0x04;
    tNFA_HCI_RESPONSE response = nfa_hciu_add_pipe_to_static_gate(
            local_gate, pipe_id, dest_host, dest_gate);
    ASSERT_EQ(response, NFA_HCI_ANY_OK);
}

TEST_F(NfaHciuAddPipeToStaticGateTest, FailureWhenPipeNotAllocated) {
    uint8_t local_gate = NFA_HCI_IDENTITY_MANAGEMENT_GATE;
    uint8_t pipe_id = 0x01;
    uint8_t dest_host = 0x02;
    uint8_t dest_gate = 0x03;
    tNFA_HCI_DYN_PIPE* p_pipe = nullptr;
    for (int i = 0; i < NFA_HCI_MAX_PIPE_COUNT; ++i) {
        if (nfa_hci_cb.cfg.dyn_pipes[i].pipe_state == NFA_HCI_PIPE_CLOSED) {
            p_pipe = &nfa_hci_cb.cfg.dyn_pipes[i];
            p_pipe->pipe_id = pipe_id;
            p_pipe->pipe_state = NFA_HCI_PIPE_OPEN;
            break;
        }
    }
    tNFA_HCI_RESPONSE response = (
            p_pipe == nullptr) ? NFA_HCI_ADM_E_NO_PIPES_AVAILABLE : NFA_HCI_ANY_OK;
    tNFA_HCI_RESPONSE actual_response = nfa_hciu_add_pipe_to_static_gate(
            local_gate, pipe_id, dest_host, dest_gate);
    ASSERT_EQ(actual_response, response);
}

TEST_F(NfaHciuAddPipeToStaticGateTest, IdentityManagementGatePipeIndexCalculation) {
    uint8_t local_gate = NFA_HCI_IDENTITY_MANAGEMENT_GATE;
    uint8_t pipe_id = 0x01;
    uint8_t dest_host = 0x02;
    uint8_t dest_gate = 0x03;
    tNFA_HCI_RESPONSE response = nfa_hciu_add_pipe_to_static_gate(
            local_gate, pipe_id, dest_host, dest_gate);
    ASSERT_EQ(response, NFA_HCI_ANY_OK);
    uint8_t pipe_index = 0;
    uint32_t expected_mask = (1 << pipe_index);
    ASSERT_EQ(nfa_hci_cb.cfg.id_mgmt_gate.pipe_inx_mask & expected_mask, expected_mask);
}

void reset_system() {
    tNFA_HCI_DYN_GATE gate;
    gate.gate_id = 0x02;
    gate.pipe_inx_mask = 0;
    nfa_hci_cb.cfg.dyn_gates[0] = gate;
}

class NfaHciuAddPipeToGateTest : public ::testing::Test {
protected:
    void SetUp() override {
        reset_system();
    }
    void TearDown() override {
        reset_system();
    }
};

TEST_F(NfaHciuAddPipeToGateTest, GateFoundPipeAllocatedSuccessfully) {
    uint8_t pipe_id = 0x01;
    uint8_t local_gate = 0x02;
    uint8_t dest_host = 0x03;
    uint8_t dest_gate = 0x04;
    tNFA_HCI_DYN_GATE* p_gate = nfa_hciu_find_gate_by_gid(local_gate);
    ASSERT_NE(p_gate, nullptr);
    tNFA_HCI_DYN_PIPE* p_pipe = nfa_hciu_alloc_pipe(pipe_id);
    ASSERT_NE(p_pipe, nullptr);
    tNFA_HCI_RESPONSE result = nfa_hciu_add_pipe_to_gate(pipe_id, local_gate, dest_host, dest_gate);
    EXPECT_EQ(result, NFA_HCI_ANY_OK);
    uint8_t pipe_index = (uint8_t)(p_pipe - nfa_hci_cb.cfg.dyn_pipes);
    EXPECT_EQ((p_gate->pipe_inx_mask & (1 << pipe_index)), (1 << pipe_index));
}

TEST_F(NfaHciuAddPipeToGateTest, GateNotFound) {
    uint8_t pipe_id = 0x01;
    uint8_t local_gate = 0x03;
    uint8_t dest_host = 0x03;
    uint8_t dest_gate = 0x04;
    tNFA_HCI_DYN_GATE* p_gate = nfa_hciu_find_gate_by_gid(local_gate);
    EXPECT_EQ(p_gate, nullptr);
    tNFA_HCI_RESPONSE result = nfa_hciu_add_pipe_to_gate(pipe_id, local_gate, dest_host, dest_gate);
    EXPECT_EQ(result, NFA_HCI_ADM_E_NO_PIPES_AVAILABLE);
}

TEST_F(NfaHciuAddPipeToGateTest, PipeIndexCalculation) {
    uint8_t pipe_id = 0x01;
    uint8_t local_gate = 0x02;
    uint8_t dest_host = 0x03;
    uint8_t dest_gate = 0x04;
    tNFA_HCI_DYN_GATE* p_gate = nfa_hciu_find_gate_by_gid(local_gate);
    ASSERT_NE(p_gate, nullptr);
    tNFA_HCI_DYN_PIPE* p_pipe = nfa_hciu_alloc_pipe(pipe_id);
    ASSERT_NE(p_pipe, nullptr);
    uint8_t pipe_index = (uint8_t)(p_pipe - nfa_hci_cb.cfg.dyn_pipes);
    tNFA_HCI_RESPONSE result = nfa_hciu_add_pipe_to_gate(pipe_id, local_gate, dest_host, dest_gate);
    EXPECT_EQ((p_gate->pipe_inx_mask & (1 << pipe_index)), (1 << pipe_index));
    EXPECT_EQ(result, NFA_HCI_ANY_OK);
}

class NfaHciuAllocPipeTest : public ::testing::Test {
protected:
    void SetUp() override {
        memset(nfa_hci_cb.cfg.dyn_pipes, 0, sizeof(nfa_hci_cb.cfg.dyn_pipes));
        nfa_hci_cb.nv_write_needed = false;
    }
    void TearDown() override {
    }
};

TEST_F(NfaHciuAllocPipeTest, PipeAllocationSuccess) {
    uint8_t pipe_id = 0x01;
    tNFA_HCI_DYN_PIPE* allocated_pipe = nfa_hciu_alloc_pipe(pipe_id);
    ASSERT_NE(allocated_pipe, nullptr);
    EXPECT_EQ(allocated_pipe->pipe_id, pipe_id);
    EXPECT_TRUE(nfa_hci_cb.nv_write_needed);
}

TEST_F(NfaHciuAllocPipeTest, PipeAlreadyAllocated) {
    uint8_t pipe_id = 0x01;
    nfa_hci_cb.cfg.dyn_pipes[0].pipe_id = pipe_id;
    tNFA_HCI_DYN_PIPE* allocated_pipe = nfa_hciu_alloc_pipe(pipe_id);
    ASSERT_NE(allocated_pipe, nullptr);
    EXPECT_EQ(allocated_pipe->pipe_id, pipe_id);
    EXPECT_TRUE(nfa_hci_cb.nv_write_needed);
}

TEST_F(NfaHciuAllocPipeTest, PipeIdGreaterThanLastDynamicPipe) {
    uint8_t pipe_id = NFA_HCI_LAST_DYNAMIC_PIPE + 1;
    nfa_hci_cb.cfg.dyn_pipes[0].pipe_id = 0x02;
    tNFA_HCI_DYN_PIPE* allocated_pipe = nfa_hciu_alloc_pipe(pipe_id);
    ASSERT_NE(allocated_pipe, nullptr);
    EXPECT_EQ(allocated_pipe->pipe_id, pipe_id);
    EXPECT_TRUE(nfa_hci_cb.nv_write_needed);
}

TEST_F(NfaHciuAllocPipeTest, PipeReleasedBeforeAllocation) {
    uint8_t pipe_id = 0x01;
    nfa_hci_cb.cfg.dyn_pipes[0].pipe_id = pipe_id;
    nfa_hciu_release_pipe(pipe_id);
    tNFA_HCI_DYN_PIPE* allocated_pipe = nfa_hciu_alloc_pipe(pipe_id);
    ASSERT_NE(allocated_pipe, nullptr);
    EXPECT_EQ(allocated_pipe->pipe_id, pipe_id);
    EXPECT_TRUE(nfa_hci_cb.nv_write_needed);
}

class NfaHciuCheckPipeBetweenGatesTest : public ::testing::Test {
protected:
    void SetUp() override {
        memset(nfa_hci_cb.cfg.dyn_pipes, 0, sizeof(nfa_hci_cb.cfg.dyn_pipes));
    }
};

TEST_F(NfaHciuCheckPipeBetweenGatesTest, PipeExists) {
    uint8_t local_gate = 0x01;
    uint8_t dest_host = 0x02;
    uint8_t dest_gate = 0x03;
    nfa_hci_cb.cfg.dyn_pipes[0].pipe_id = 0x10;
    nfa_hci_cb.cfg.dyn_pipes[0].local_gate = local_gate;
    nfa_hci_cb.cfg.dyn_pipes[0].dest_host = dest_host;
    nfa_hci_cb.cfg.dyn_pipes[0].dest_gate = dest_gate;
    bool result = nfa_hciu_check_pipe_between_gates(local_gate, dest_host, dest_gate);
    EXPECT_TRUE(result);
}

TEST_F(NfaHciuCheckPipeBetweenGatesTest, PipeDoesNotExist) {
    uint8_t local_gate = 0x01;
    uint8_t dest_host = 0x02;
    uint8_t dest_gate = 0x03;
    bool result = nfa_hciu_check_pipe_between_gates(local_gate, dest_host, dest_gate);
    EXPECT_FALSE(result);
}

TEST_F(NfaHciuCheckPipeBetweenGatesTest, InvalidPipeID) {
    uint8_t local_gate = 0x01;
    uint8_t dest_host = 0x02;
    uint8_t dest_gate = 0x03;
    nfa_hci_cb.cfg.dyn_pipes[0].pipe_id = 0x01;
    nfa_hci_cb.cfg.dyn_pipes[0].local_gate = local_gate;
    nfa_hci_cb.cfg.dyn_pipes[0].dest_host = dest_host;
    nfa_hci_cb.cfg.dyn_pipes[0].dest_gate = dest_gate;
    bool result = nfa_hciu_check_pipe_between_gates(local_gate, dest_host, dest_gate);
    EXPECT_FALSE(result);
}

TEST_F(NfaHciuCheckPipeBetweenGatesTest, IncorrectGateOrHost) {
    uint8_t local_gate = 0x01;
    uint8_t dest_host = 0x02;
    uint8_t dest_gate = 0x03;
    nfa_hci_cb.cfg.dyn_pipes[0].pipe_id = 0x10;
    nfa_hci_cb.cfg.dyn_pipes[0].local_gate = local_gate;
    nfa_hci_cb.cfg.dyn_pipes[0].dest_host = dest_host;
    nfa_hci_cb.cfg.dyn_pipes[0].dest_gate = 0x04;
    bool result = nfa_hciu_check_pipe_between_gates(local_gate, dest_host, dest_gate);
    EXPECT_FALSE(result);
}

TEST_F(NfaHciuCheckPipeBetweenGatesTest, MultiplePipesOneMatch) {
    uint8_t local_gate = 0x01;
    uint8_t dest_host = 0x02;
    uint8_t dest_gate = 0x03;
    nfa_hci_cb.cfg.dyn_pipes[0].pipe_id = 0x10;
    nfa_hci_cb.cfg.dyn_pipes[0].local_gate = local_gate;
    nfa_hci_cb.cfg.dyn_pipes[0].dest_host = dest_host;
    nfa_hci_cb.cfg.dyn_pipes[0].dest_gate = 0x04;
    nfa_hci_cb.cfg.dyn_pipes[1].pipe_id = 0x11;
    nfa_hci_cb.cfg.dyn_pipes[1].local_gate = local_gate;
    nfa_hci_cb.cfg.dyn_pipes[1].dest_host = dest_host;
    nfa_hci_cb.cfg.dyn_pipes[1].dest_gate = dest_gate;
    bool result = nfa_hciu_check_pipe_between_gates(local_gate, dest_host, dest_gate);
    EXPECT_TRUE(result);
}

class NfaHciuCountOpenPipesOnGateTest : public ::testing::Test {
protected:
    void SetUp() override {
        memset(nfa_hci_cb.cfg.dyn_pipes, 0, sizeof(nfa_hci_cb.cfg.dyn_pipes));
    }
};

TEST_F(NfaHciuCountOpenPipesOnGateTest, NoOpenPipes) {
    tNFA_HCI_DYN_GATE gate;
    gate.pipe_inx_mask = 0x00;
    uint8_t result = nfa_hciu_count_open_pipes_on_gate(&gate);
    EXPECT_EQ(result, 0);
}

TEST_F(NfaHciuCountOpenPipesOnGateTest, OneOpenPipe) {
    tNFA_HCI_DYN_GATE gate;
    gate.pipe_inx_mask = 0x01;
    nfa_hci_cb.cfg.dyn_pipes[0].pipe_state = NFA_HCI_PIPE_OPENED;
    uint8_t result = nfa_hciu_count_open_pipes_on_gate(&gate);
    EXPECT_EQ(result, 1);
}

TEST_F(NfaHciuCountOpenPipesOnGateTest, MultipleOpenPipes) {
    tNFA_HCI_DYN_GATE gate;
    gate.pipe_inx_mask = 0x03;
    nfa_hci_cb.cfg.dyn_pipes[0].pipe_state = NFA_HCI_PIPE_OPENED;
    nfa_hci_cb.cfg.dyn_pipes[1].pipe_state = NFA_HCI_PIPE_OPENED;
    uint8_t result = nfa_hciu_count_open_pipes_on_gate(&gate);
    EXPECT_EQ(result, 2);
}

TEST_F(NfaHciuCountOpenPipesOnGateTest, AllClosedPipes) {
    tNFA_HCI_DYN_GATE gate;
    gate.pipe_inx_mask = 0x07;
    nfa_hci_cb.cfg.dyn_pipes[0].pipe_state = NFA_HCI_PIPE_CLOSED;
    nfa_hci_cb.cfg.dyn_pipes[1].pipe_state = NFA_HCI_PIPE_CLOSED;
    nfa_hci_cb.cfg.dyn_pipes[2].pipe_state = NFA_HCI_PIPE_CLOSED;
    uint8_t result = nfa_hciu_count_open_pipes_on_gate(&gate);
    EXPECT_EQ(result, 0);
}

TEST_F(NfaHciuCountOpenPipesOnGateTest, GateNoPipesAssigned) {
    tNFA_HCI_DYN_GATE gate;
    gate.pipe_inx_mask = 0x00;
    uint8_t result = nfa_hciu_count_open_pipes_on_gate(&gate);
    EXPECT_EQ(result, 0);
}

class NfaHciuCountPipesOnGateTest : public ::testing::Test {
protected:
    void SetUp() override {
        memset(nfa_hci_cb.cfg.dyn_pipes, 0, sizeof(nfa_hci_cb.cfg.dyn_pipes));
    }
};

TEST_F(NfaHciuCountPipesOnGateTest, NoPipesAssigned) {
    tNFA_HCI_DYN_GATE gate;
    gate.pipe_inx_mask = 0x00;
    uint8_t result = nfa_hciu_count_pipes_on_gate(&gate);
    EXPECT_EQ(result, 0);
}

TEST_F(NfaHciuCountPipesOnGateTest, OnePipeAssigned) {
    tNFA_HCI_DYN_GATE gate;
    gate.pipe_inx_mask = 0x01;
    uint8_t result = nfa_hciu_count_pipes_on_gate(&gate);
    EXPECT_EQ(result, 1);
}

TEST_F(NfaHciuCountPipesOnGateTest, MultiplePipesAssigned) {
    tNFA_HCI_DYN_GATE gate;
    gate.pipe_inx_mask = 0x07;
    uint8_t result = nfa_hciu_count_pipes_on_gate(&gate);
    EXPECT_EQ(result, 3);
}

TEST_F(NfaHciuCountPipesOnGateTest, AllPipesAssigned) {
    tNFA_HCI_DYN_GATE gate;
    gate.pipe_inx_mask = 0xFF;
    uint8_t result = nfa_hciu_count_pipes_on_gate(&gate);
    EXPECT_EQ(result, 8);
}

class NfaHciuEvt2StrTest : public ::testing::Test {
protected:
};

TEST_F(NfaHciuEvt2StrTest, TestEventWithInvalidPipeIdOrGate) {
    EXPECT_EQ(nfa_hciu_evt_2_str(2, NFA_HCI_EVT_HCI_END_OF_OPERATION), "EVT_END_OF_OPERATION");
    EXPECT_EQ(nfa_hciu_evt_2_str(2, NFA_HCI_EVT_POST_DATA), "EVT_POST_DATA");
    EXPECT_EQ(nfa_hciu_evt_2_str(2, NFA_HCI_EVT_HOT_PLUG), "EVT_HOT_PLUG");
}

TEST_F(NfaHciuEvt2StrTest, TestEventWithAdminAndLinkManagementPipes) {
    EXPECT_EQ(nfa_hciu_evt_2_str(
            NFA_HCI_ADMIN_PIPE, NFA_HCI_EVT_HCI_END_OF_OPERATION), "EVT_END_OF_OPERATION");
    EXPECT_EQ(nfa_hciu_evt_2_str(NFA_HCI_LINK_MANAGEMENT_PIPE,
                                 NFA_HCI_EVT_HCI_END_OF_OPERATION), "EVT_END_OF_OPERATION");
}

TEST_F(NfaHciuEvt2StrTest, TestValidEventsOnNonConnectivityGate) {
    EXPECT_EQ(nfa_hciu_evt_2_str(2, NFA_HCI_EVT_HCI_END_OF_OPERATION), "EVT_END_OF_OPERATION");
    EXPECT_EQ(nfa_hciu_evt_2_str(2, NFA_HCI_EVT_POST_DATA), "EVT_POST_DATA");
    EXPECT_EQ(nfa_hciu_evt_2_str(2, NFA_HCI_EVT_HOT_PLUG), "EVT_HOT_PLUG");
}

class NfaHciFindActivePipeByOwnerTest : public ::testing::Test {
protected:
    void SetUp() override {
        for (int i = 0; i < NFA_HCI_MAX_PIPE_CB; ++i) {
            nfa_hci_cb.cfg.dyn_pipes[i].pipe_id = 0;
            nfa_hci_cb.cfg.dyn_pipes[i].local_gate = 0;
            nfa_hci_cb.cfg.dyn_pipes[i].dest_host = 0;
        }
    }
};

TEST_F(NfaHciFindActivePipeByOwnerTest, NoActivePipeFound) {
    tNFA_HANDLE app_handle = 0x1234;
    tNFA_HCI_DYN_PIPE* result = nfa_hciu_find_active_pipe_by_owner(app_handle);
    EXPECT_EQ(result, nullptr);
}

TEST_F(NfaHciFindActivePipeByOwnerTest, ActivePipeButInactiveHost) {
    tNFA_HANDLE app_handle = 0x1234;
    nfa_hci_cb.cfg.dyn_pipes[0].pipe_id = NFA_HCI_FIRST_DYNAMIC_PIPE + 1;
    nfa_hci_cb.cfg.dyn_pipes[0].local_gate = 1;
    nfa_hci_cb.cfg.dyn_pipes[0].dest_host = 0;
    tNFA_HCI_DYN_PIPE* result = nfa_hciu_find_active_pipe_by_owner(app_handle);
    EXPECT_EQ(result, nullptr);
}

TEST_F(NfaHciFindActivePipeByOwnerTest, NoMatchingPipesAtAll) {
    tNFA_HANDLE app_handle = 0x1234;
    for (int i = 0; i < NFA_HCI_MAX_PIPE_CB; ++i) {
        nfa_hci_cb.cfg.dyn_pipes[i].pipe_id = 0;
        nfa_hci_cb.cfg.dyn_pipes[i].local_gate = 0;
        nfa_hci_cb.cfg.dyn_pipes[i].dest_host = 0;
    }
    tNFA_HCI_DYN_PIPE* result = nfa_hciu_find_active_pipe_by_owner(app_handle);
    EXPECT_EQ(result, nullptr);
}

class NfaHciFindActivePipeOnGateTest : public ::testing::Test {
protected:
    void SetUp() override {
        nfa_hci_cb.cfg.dyn_pipes[0].pipe_id = NFA_HCI_FIRST_DYNAMIC_PIPE + 1;
        nfa_hci_cb.cfg.dyn_pipes[0].local_gate = 1;
        nfa_hci_cb.cfg.dyn_pipes[0].dest_host = 1;
        nfa_hci_cb.cfg.dyn_pipes[1].pipe_id = NFA_HCI_FIRST_DYNAMIC_PIPE + 2;
        nfa_hci_cb.cfg.dyn_pipes[1].local_gate = 2;
        nfa_hci_cb.cfg.dyn_pipes[1].dest_host = 1;
        nfa_hci_cb.cfg.dyn_pipes[2].pipe_id = NFA_HCI_FIRST_DYNAMIC_PIPE + 3;
        nfa_hci_cb.cfg.dyn_pipes[2].local_gate = 1;
        nfa_hci_cb.cfg.dyn_pipes[2].dest_host = 2;
    }
};

TEST_F(NfaHciFindActivePipeOnGateTest, NoPipeFoundForGate) {
    uint8_t gate_id = 3;
    tNFA_HCI_DYN_PIPE* result = nfa_hciu_find_active_pipe_on_gate(gate_id);
    EXPECT_EQ(result, nullptr);
}

TEST_F(NfaHciFindActivePipeOnGateTest, PipeWithInactiveHost) {
    uint8_t gate_id = 1;
    tNFA_HCI_DYN_GATE gate = { .gate_id = gate_id };
    tNFA_HCI_DYN_PIPE* result = nfa_hciu_find_active_pipe_on_gate(gate_id);
    EXPECT_EQ(result, nullptr);
}

TEST_F(NfaHciFindActivePipeOnGateTest, InvalidPipeID) {
    uint8_t gate_id = 1;
    tNFA_HCI_DYN_GATE gate = { .gate_id = gate_id };
    nfa_hci_cb.cfg.dyn_pipes[0].pipe_id = 0;
    tNFA_HCI_DYN_PIPE* result = nfa_hciu_find_active_pipe_on_gate(gate_id);
    EXPECT_EQ(result, nullptr);
}

TEST_F(NfaHciFindActivePipeOnGateTest, AllPipesInactive) {
    uint8_t gate_id = 1;
    tNFA_HCI_DYN_GATE gate = { .gate_id = gate_id };
    tNFA_HCI_DYN_PIPE* result = nfa_hciu_find_active_pipe_on_gate(gate_id);
    EXPECT_EQ(result, nullptr);
}

class NfaHciFindGateByOwnerTest : public :: testing :: Test{
protected:
};

TEST_F(NfaHciFindGateByOwnerTest, GateFoundForOwner) {
    tNFA_HANDLE app_handle = 0x1234;
    nfa_hci_cb.cfg.dyn_gates[0].gate_owner = app_handle;
    nfa_hci_cb.cfg.dyn_gates[0].gate_id = 1;
    nfa_hci_cb.cfg.dyn_gates[1].gate_owner = 0x5678;
    nfa_hci_cb.cfg.dyn_gates[1].gate_id = 2;
    tNFA_HCI_DYN_GATE* result = nfa_hciu_find_gate_by_owner(app_handle);
    EXPECT_EQ(result, &nfa_hci_cb.cfg.dyn_gates[0]);
}

TEST_F(NfaHciFindGateByOwnerTest, NoGateFoundForOwner) {
    tNFA_HANDLE app_handle = 0x1234;
    nfa_hci_cb.cfg.dyn_gates[0].gate_owner = 0x5678;
    nfa_hci_cb.cfg.dyn_gates[0].gate_id = 1;
    nfa_hci_cb.cfg.dyn_gates[1].gate_owner = 0x9abc;
    nfa_hci_cb.cfg.dyn_gates[1].gate_id = 2;
    tNFA_HCI_DYN_GATE* result = nfa_hciu_find_gate_by_owner(app_handle);
    EXPECT_EQ(result, nullptr);
}

TEST_F(NfaHciFindGateByOwnerTest, GateFoundInMiddle) {
    tNFA_HANDLE app_handle = 0x1234;
    nfa_hci_cb.cfg.dyn_gates[0].gate_owner = 0x5678;
    nfa_hci_cb.cfg.dyn_gates[0].gate_id = 1;
    nfa_hci_cb.cfg.dyn_gates[1].gate_owner = app_handle;
    nfa_hci_cb.cfg.dyn_gates[1].gate_id = 2;
    nfa_hci_cb.cfg.dyn_gates[2].gate_owner = 0x9abc;
    nfa_hci_cb.cfg.dyn_gates[2].gate_id = 3;
    tNFA_HCI_DYN_GATE* result = nfa_hciu_find_gate_by_owner(app_handle);
    EXPECT_EQ(result, &nfa_hci_cb.cfg.dyn_gates[1]);
}

TEST_F(NfaHciFindGateByOwnerTest, GateFoundAtEnd) {
    tNFA_HANDLE app_handle = 0x1234;
    nfa_hci_cb.cfg.dyn_gates[0].gate_owner = 0x5678;
    nfa_hci_cb.cfg.dyn_gates[0].gate_id = 1;
    nfa_hci_cb.cfg.dyn_gates[1].gate_owner = 0x9abc;
    nfa_hci_cb.cfg.dyn_gates[1].gate_id = 2;
    nfa_hci_cb.cfg.dyn_gates[5].gate_owner = app_handle;
    nfa_hci_cb.cfg.dyn_gates[5].gate_id = 10;
    tNFA_HCI_DYN_GATE* result = nfa_hciu_find_gate_by_owner(app_handle);
    EXPECT_EQ(result, &nfa_hci_cb.cfg.dyn_gates[5]);
}

TEST_F(NfaHciFindGateByOwnerTest, AllGatesHaveDifferentOwners) {
    tNFA_HANDLE app_handle = 0x1234;
    nfa_hci_cb.cfg.dyn_gates[0].gate_owner = 0x5678;
    nfa_hci_cb.cfg.dyn_gates[0].gate_id = 1;
    nfa_hci_cb.cfg.dyn_gates[1].gate_owner = 0x9abc;
    nfa_hci_cb.cfg.dyn_gates[1].gate_id = 2;
    nfa_hci_cb.cfg.dyn_gates[2].gate_owner = 0xaaaa;
    nfa_hci_cb.cfg.dyn_gates[2].gate_id = 3;
    nfa_hci_cb.cfg.dyn_gates[3].gate_owner = 0xbbb1;
    nfa_hci_cb.cfg.dyn_gates[3].gate_id = 4;
    nfa_hci_cb.cfg.dyn_gates[4].gate_owner = 0xcccc;
    nfa_hci_cb.cfg.dyn_gates[4].gate_id = 5;
    nfa_hci_cb.cfg.dyn_gates[5].gate_owner = 0xdddd;
    nfa_hci_cb.cfg.dyn_gates[5].gate_id = 6;
    tNFA_HCI_DYN_GATE* result = nfa_hciu_find_gate_by_owner(app_handle);
    EXPECT_EQ(result, nullptr);
}

TEST_F(NfaHciFindGateByOwnerTest, FirstGateMatches) {
    tNFA_HANDLE app_handle = 0x1234;
    nfa_hci_cb.cfg.dyn_gates[0].gate_owner = app_handle;
    nfa_hci_cb.cfg.dyn_gates[0].gate_id = 1;
    tNFA_HCI_DYN_GATE* result = nfa_hciu_find_gate_by_owner(app_handle);
    EXPECT_EQ(result, &nfa_hci_cb.cfg.dyn_gates[0]);
}

class NfaHciFindGateWithNoPipesByOwnerTest : public :: testing :: Test{
protected:
};

TEST_F(NfaHciFindGateWithNoPipesByOwnerTest, GateFoundWithNoPipes) {
    tNFA_HANDLE app_handle = 0x1234;
    nfa_hci_cb.cfg.dyn_gates[0].gate_owner = app_handle;
    nfa_hci_cb.cfg.dyn_gates[0].pipe_inx_mask = 0;
    nfa_hci_cb.cfg.dyn_gates[1].gate_owner = app_handle;
    nfa_hci_cb.cfg.dyn_gates[1].pipe_inx_mask = 1;
    tNFA_HCI_DYN_GATE* result = nfa_hciu_find_gate_with_nopipes_by_owner(app_handle);
    EXPECT_EQ(result, &nfa_hci_cb.cfg.dyn_gates[0]);
}

TEST_F(NfaHciFindGateWithNoPipesByOwnerTest, NoGateWithNoPipesForOwner) {
    tNFA_HANDLE app_handle = 0x1234;
    nfa_hci_cb.cfg.dyn_gates[0].gate_owner = app_handle;
    nfa_hci_cb.cfg.dyn_gates[0].pipe_inx_mask = 1;
    nfa_hci_cb.cfg.dyn_gates[1].gate_owner = app_handle;
    nfa_hci_cb.cfg.dyn_gates[1].pipe_inx_mask = 1;
    tNFA_HCI_DYN_GATE* result = nfa_hciu_find_gate_with_nopipes_by_owner(app_handle);
    EXPECT_EQ(result, nullptr);
}

TEST_F(NfaHciFindGateWithNoPipesByOwnerTest, NoGatesForOwner) {
    tNFA_HANDLE app_handle = 0x1234;
    nfa_hci_cb.cfg.dyn_gates[0].gate_owner = 0x5678;
    nfa_hci_cb.cfg.dyn_gates[0].pipe_inx_mask = 0;
    nfa_hci_cb.cfg.dyn_gates[1].gate_owner = 0x9abc;
    nfa_hci_cb.cfg.dyn_gates[1].pipe_inx_mask = 1;
    tNFA_HCI_DYN_GATE* result = nfa_hciu_find_gate_with_nopipes_by_owner(app_handle);
    EXPECT_EQ(result, nullptr);
}

TEST_F(NfaHciFindGateWithNoPipesByOwnerTest, MultipleGatesOnlyOneWithNoPipes) {
    tNFA_HANDLE app_handle = 0x1234;
    nfa_hci_cb.cfg.dyn_gates[0].gate_owner = app_handle;
    nfa_hci_cb.cfg.dyn_gates[0].pipe_inx_mask = 0;
    nfa_hci_cb.cfg.dyn_gates[1].gate_owner = app_handle;
    nfa_hci_cb.cfg.dyn_gates[1].pipe_inx_mask = 1;
    nfa_hci_cb.cfg.dyn_gates[2].gate_owner = app_handle;
    nfa_hci_cb.cfg.dyn_gates[2].pipe_inx_mask = 1;
    tNFA_HCI_DYN_GATE* result = nfa_hciu_find_gate_with_nopipes_by_owner(app_handle);
    EXPECT_EQ(result, &nfa_hci_cb.cfg.dyn_gates[0]);
}

TEST_F(NfaHciFindGateWithNoPipesByOwnerTest, AllGatesWithNoPipes) {
    tNFA_HANDLE app_handle = 0x1234;
    nfa_hci_cb.cfg.dyn_gates[0].gate_owner = app_handle;
    nfa_hci_cb.cfg.dyn_gates[0].pipe_inx_mask = 0;
    nfa_hci_cb.cfg.dyn_gates[1].gate_owner = app_handle;
    nfa_hci_cb.cfg.dyn_gates[1].pipe_inx_mask = 0;
    nfa_hci_cb.cfg.dyn_gates[2].gate_owner = app_handle;
    nfa_hci_cb.cfg.dyn_gates[2].pipe_inx_mask = 0;
    tNFA_HCI_DYN_GATE* result = nfa_hciu_find_gate_with_nopipes_by_owner(app_handle);
    EXPECT_EQ(result, &nfa_hci_cb.cfg.dyn_gates[0]);
}

class NfaHciFindPipeByOwnerTest : public :: testing :: Test{
protected:
};

TEST_F(NfaHciFindPipeByOwnerTest, PipeFoundForOwner) {
    tNFA_HANDLE app_handle = 0x1234;
    nfa_hci_cb.cfg.dyn_gates[0].gate_owner = app_handle;
    nfa_hci_cb.cfg.dyn_gates[0].gate_id = 1;
    nfa_hci_cb.cfg.dyn_pipes[0].pipe_id = 1;
    nfa_hci_cb.cfg.dyn_pipes[0].local_gate = 1;
    tNFA_HCI_DYN_PIPE* result = nfa_hciu_find_pipe_by_owner(app_handle);
    EXPECT_EQ(result, &nfa_hci_cb.cfg.dyn_pipes[0]);
}

TEST_F(NfaHciFindPipeByOwnerTest, NoPipesExist) {
    tNFA_HANDLE app_handle = 0x1234;
    for (int i = 0; i < NFA_HCI_MAX_PIPE_CB; i++) {
        nfa_hci_cb.cfg.dyn_pipes[i].pipe_id = 0;
    }
    tNFA_HCI_DYN_PIPE* result = nfa_hciu_find_pipe_by_owner(app_handle);
    EXPECT_EQ(result, nullptr);
}

TEST_F(NfaHciFindPipeByOwnerTest, AllPipesHaveDifferentOwners) {
    tNFA_HANDLE app_handle = 0x1234;
    nfa_hci_cb.cfg.dyn_gates[0].gate_owner = 0x5678;
    nfa_hci_cb.cfg.dyn_gates[0].gate_id = 1;
    nfa_hci_cb.cfg.dyn_pipes[0].pipe_id = 1;
    nfa_hci_cb.cfg.dyn_pipes[0].local_gate = 1;
    tNFA_HCI_DYN_PIPE* result = nfa_hciu_find_pipe_by_owner(app_handle);
    EXPECT_EQ(result, nullptr);
}

class NfaHciFindPipeByPidTest : public testing::Test {
protected:
    void SetUp() override {
        for (int i = 0; i < NFA_HCI_MAX_PIPE_CB; ++i) {
            nfa_hci_cb.cfg.dyn_pipes[i].pipe_id = 0;
        }
    }
};

TEST_F(NfaHciFindPipeByPidTest, PipeFoundById) {
    uint8_t pipe_id = 5;
    nfa_hci_cb.cfg.dyn_pipes[0].pipe_id = pipe_id;
    nfa_hci_cb.cfg.dyn_pipes[0].local_gate = 1;
    nfa_hci_cb.cfg.dyn_pipes[0].dest_host = 1;
    tNFA_HCI_DYN_PIPE* result = nfa_hciu_find_pipe_by_pid(pipe_id);
    EXPECT_EQ(result, &nfa_hci_cb.cfg.dyn_pipes[0]);
}

TEST_F(NfaHciFindPipeByPidTest, PipeNotFound) {
    uint8_t pipe_id = 200;
    tNFA_HCI_DYN_PIPE* result = nfa_hciu_find_pipe_by_pid(pipe_id);
    EXPECT_EQ(result, nullptr);
}

TEST_F(NfaHciFindPipeByPidTest, FirstPipeEdgeCase) {
    uint8_t pipe_id = 1;
    nfa_hci_cb.cfg.dyn_pipes[0].pipe_id = pipe_id;
    tNFA_HCI_DYN_PIPE* result = nfa_hciu_find_pipe_by_pid(pipe_id);
    EXPECT_EQ(result, &nfa_hci_cb.cfg.dyn_pipes[0]);
}

TEST_F(NfaHciFindPipeByPidTest, LastPipeEdgeCase) {
    uint8_t pipe_id = NFA_HCI_MAX_PIPE_CB;
    nfa_hci_cb.cfg.dyn_pipes[NFA_HCI_MAX_PIPE_CB - 1].pipe_id = pipe_id;
    tNFA_HCI_DYN_PIPE* result = nfa_hciu_find_pipe_by_pid(pipe_id);
    EXPECT_EQ(result, &nfa_hci_cb.cfg.dyn_pipes[NFA_HCI_MAX_PIPE_CB - 1]);
}

TEST_F(NfaHciFindPipeByPidTest, MultiplePipesWithSameId) {
    uint8_t pipe_id = 10;
    nfa_hci_cb.cfg.dyn_pipes[0].pipe_id = pipe_id;
    nfa_hci_cb.cfg.dyn_pipes[1].pipe_id = pipe_id;
    tNFA_HCI_DYN_PIPE* result = nfa_hciu_find_pipe_by_pid(pipe_id);
    EXPECT_EQ(result, &nfa_hci_cb.cfg.dyn_pipes[0]);
}

class NfaHciFindPipeOnGateTest : public :: testing :: Test {
protected:
};
TEST_F(NfaHciFindPipeOnGateTest, PipeFoundOnGate) {
    uint8_t gate_id = 2;
    nfa_hci_cb.cfg.dyn_pipes[0].pipe_id = NFA_HCI_FIRST_DYNAMIC_PIPE + 1;
    nfa_hci_cb.cfg.dyn_pipes[0].local_gate = gate_id;
    nfa_hci_cb.cfg.dyn_pipes[0].dest_host = 1;
    tNFA_HCI_DYN_GATE gate = { .gate_id = gate_id, .gate_owner = 0x1234 };
    nfa_hci_cb.cfg.dyn_gates[gate_id] = gate;
    tNFA_HCI_DYN_PIPE* result = nfa_hciu_find_pipe_on_gate(gate_id);
    EXPECT_EQ(result, &nfa_hci_cb.cfg.dyn_pipes[0]);
}

TEST_F(NfaHciFindPipeOnGateTest, NoPipeOnGate) {
    uint8_t gate_id = 2;
    for (int i = 0; i < NFA_HCI_MAX_PIPE_CB; i++) {
        nfa_hci_cb.cfg.dyn_pipes[i].pipe_id = 0;
    }
    tNFA_HCI_DYN_PIPE* result = nfa_hciu_find_pipe_on_gate(gate_id);
    EXPECT_EQ(result, nullptr);
}

TEST_F(NfaHciFindPipeOnGateTest, PipeWithInvalidGate) {
    uint8_t gate_id = 2;
    nfa_hci_cb.cfg.dyn_pipes[0].pipe_id = NFA_HCI_FIRST_DYNAMIC_PIPE + 1;
    nfa_hci_cb.cfg.dyn_pipes[0].local_gate = 99;
    nfa_hci_cb.cfg.dyn_pipes[0].dest_host = 1;
    tNFA_HCI_DYN_GATE gate = { .gate_id = gate_id, .gate_owner = 0x1234 };
    nfa_hci_cb.cfg.dyn_gates[gate_id] = gate;
    tNFA_HCI_DYN_PIPE* result = nfa_hciu_find_pipe_on_gate(gate_id);
    EXPECT_EQ(result, nullptr);
}

TEST_F(NfaHciFindPipeOnGateTest, MultiplePipesOnGate) {
    uint8_t gate_id = 2;
    nfa_hci_cb.cfg.dyn_pipes[0].pipe_id = NFA_HCI_FIRST_DYNAMIC_PIPE + 1;
    nfa_hci_cb.cfg.dyn_pipes[0].local_gate = gate_id;
    nfa_hci_cb.cfg.dyn_pipes[0].dest_host = 1;
    nfa_hci_cb.cfg.dyn_pipes[1].pipe_id = NFA_HCI_FIRST_DYNAMIC_PIPE + 2;
    nfa_hci_cb.cfg.dyn_pipes[1].local_gate = 99;
    nfa_hci_cb.cfg.dyn_pipes[1].dest_host = 1;
    tNFA_HCI_DYN_GATE gate = { .gate_id = gate_id, .gate_owner = 0x1234 };
    nfa_hci_cb.cfg.dyn_gates[gate_id] = gate;
    tNFA_HCI_DYN_PIPE* result = nfa_hciu_find_pipe_on_gate(gate_id);
    EXPECT_EQ(result, &nfa_hci_cb.cfg.dyn_pipes[0]);
}

class NfaHciGetAllocatedGateListTest : public ::testing::Test {
protected:
    void SetUp() override {
        memset(nfa_hci_cb.cfg.dyn_gates, 0, sizeof(nfa_hci_cb.cfg.dyn_gates));
    }
};

TEST_F(NfaHciGetAllocatedGateListTest, NoAllocatedGates) {
    uint8_t gate_list[NFA_HCI_MAX_GATE_CB];
    uint8_t result = nfa_hciu_get_allocated_gate_list(gate_list);
    EXPECT_EQ(result, 0);
    EXPECT_EQ(gate_list[0], 0);
}

TEST_F(NfaHciGetAllocatedGateListTest, SomeGatesAllocated) {
    uint8_t gate_list[NFA_HCI_MAX_GATE_CB];
    nfa_hci_cb.cfg.dyn_gates[1].gate_id = 5;
    nfa_hci_cb.cfg.dyn_gates[3].gate_id = 10;
    nfa_hci_cb.cfg.dyn_gates[4].gate_id = 15;
    uint8_t result = nfa_hciu_get_allocated_gate_list(gate_list);
    EXPECT_EQ(result, 3);
    EXPECT_EQ(gate_list[0], 5);
    EXPECT_EQ(gate_list[1], 10);
    EXPECT_EQ(gate_list[2], 15);
}

TEST_F(NfaHciGetAllocatedGateListTest, AllGatesAllocated) {
    uint8_t gate_list[NFA_HCI_MAX_GATE_CB];
    for (int i = 0; i < NFA_HCI_MAX_GATE_CB; i++) {
        nfa_hci_cb.cfg.dyn_gates[i].gate_id = i + 1;
    }
    uint8_t result = nfa_hciu_get_allocated_gate_list(gate_list);
    EXPECT_EQ(result, NFA_HCI_MAX_GATE_CB);
    for (int i = 0; i < NFA_HCI_MAX_GATE_CB; i++) {
        EXPECT_EQ(gate_list[i], i + 1);
    }
}

TEST_F(NfaHciGetAllocatedGateListTest, NoGatesAllocatedInitialized) {
    uint8_t gate_list[NFA_HCI_MAX_GATE_CB];
    uint8_t result = nfa_hciu_get_allocated_gate_list(gate_list);
    EXPECT_EQ(result,0);
    EXPECT_EQ(gate_list[0],0);
}

TEST_F(NfaHciGetAllocatedGateListTest, MaxGatesAllocated) {
    uint8_t gate_list[NFA_HCI_MAX_GATE_CB];
    for (int i = 0; i < NFA_HCI_MAX_GATE_CB; i++) {
        nfa_hci_cb.cfg.dyn_gates[i].gate_id = i + 1;
    }
    uint8_t result = nfa_hciu_get_allocated_gate_list(gate_list);
    EXPECT_EQ(result, NFA_HCI_MAX_GATE_CB);
    for (int i = 0; i < NFA_HCI_MAX_GATE_CB; i++) {
        EXPECT_EQ(gate_list[i], i + 1);
    }
}

class NfaHciGetEventNameTest : public testing::Test {
protected:
};

TEST_F(NfaHciGetEventNameTest, TestKnownEvents) {
    EXPECT_EQ(nfa_hciu_get_event_name(NFA_HCI_API_REGISTER_APP_EVT), "API_REGISTER");
    EXPECT_EQ(nfa_hciu_get_event_name(NFA_HCI_API_DEREGISTER_APP_EVT), "API_DEREGISTER");
    EXPECT_EQ(nfa_hciu_get_event_name(NFA_HCI_API_GET_APP_GATE_PIPE_EVT), "API_GET_GATE_LIST");
    EXPECT_EQ(nfa_hciu_get_event_name(NFA_HCI_API_ALLOC_GATE_EVT), "API_ALLOC_GATE");
    EXPECT_EQ(nfa_hciu_get_event_name(NFA_HCI_API_DEALLOC_GATE_EVT), "API_DEALLOC_GATE");
    EXPECT_EQ(nfa_hciu_get_event_name(NFA_HCI_API_GET_HOST_LIST_EVT), "API_GET_HOST_LIST");
    EXPECT_EQ(nfa_hciu_get_event_name(NFA_HCI_API_GET_REGISTRY_EVT), "API_GET_REG_VALUE");
    EXPECT_EQ(nfa_hciu_get_event_name(NFA_HCI_API_SET_REGISTRY_EVT), "API_SET_REG_VALUE");
    EXPECT_EQ(nfa_hciu_get_event_name(NFA_HCI_API_CREATE_PIPE_EVT), "API_CREATE_PIPE");
    EXPECT_EQ(nfa_hciu_get_event_name(NFA_HCI_API_OPEN_PIPE_EVT), "API_OPEN_PIPE");
    EXPECT_EQ(nfa_hciu_get_event_name(NFA_HCI_API_CLOSE_PIPE_EVT), "API_CLOSE_PIPE");
    EXPECT_EQ(nfa_hciu_get_event_name(NFA_HCI_API_DELETE_PIPE_EVT), "API_DELETE_PIPE");
    EXPECT_EQ(nfa_hciu_get_event_name(NFA_HCI_API_SEND_CMD_EVT), "API_SEND_COMMAND_EVT");
    EXPECT_EQ(nfa_hciu_get_event_name(NFA_HCI_API_SEND_RSP_EVT), "API_SEND_RESPONSE_EVT");
    EXPECT_EQ(nfa_hciu_get_event_name(NFA_HCI_API_SEND_EVENT_EVT), "API_SEND_EVENT_EVT");
    EXPECT_EQ(nfa_hciu_get_event_name(NFA_HCI_RSP_NV_READ_EVT), "NV_READ_EVT");
    EXPECT_EQ(nfa_hciu_get_event_name(NFA_HCI_RSP_NV_WRITE_EVT), "NV_WRITE_EVT");
    EXPECT_EQ(nfa_hciu_get_event_name(NFA_HCI_RSP_TIMEOUT_EVT), "RESPONSE_TIMEOUT_EVT");
    EXPECT_EQ(nfa_hciu_get_event_name(NFA_HCI_CHECK_QUEUE_EVT), "CHECK_QUEUE");
}

TEST_F(NfaHciGetEventNameTest, TestUnknownEvent) {
    EXPECT_EQ(nfa_hciu_get_event_name(0xFFFF), "UNKNOWN");
}


class NfaHciuTest : public ::testing::Test {
protected:
    tNFA_HCI_DYN_PIPE pipe_db[10];
    tNFA_HCI_DYN_GATE gate_db[10];
    void SetUp() override {
        for (int i = 0; i < 10; ++i) {
            pipe_db[i].local_gate = i;
            gate_db[i].gate_owner = i * 10;
        }
    }
};

TEST_F(NfaHciuTest, GetPipeOwner_InvalidGate) {
    uint8_t pipe_id = 1;
    pipe_db[pipe_id].local_gate = 100;
    EXPECT_EQ(nfa_hciu_get_pipe_owner(pipe_id), NFA_HANDLE_INVALID);
}

TEST_F(NfaHciuTest, GetPipeOwner_InvalidPipe) {
    uint8_t pipe_id = 99;
    EXPECT_EQ(nfa_hciu_get_pipe_owner(pipe_id), NFA_HANDLE_INVALID);
}

TEST_F(NfaHciuTest, GetPipeOwner_ValidPipeAndGate) {
    uint8_t pipe_id = 2;
    EXPECT_EQ(nfa_hciu_get_pipe_owner(pipe_id), 65535);
}

class NfaHciGetResponseNameTest : public ::testing::Test {
protected:
};

TEST_F(NfaHciGetResponseNameTest, ValidResponseCodes) {
    EXPECT_EQ(nfa_hciu_get_response_name(NFA_HCI_ANY_OK), "ANY_OK");
    EXPECT_EQ(nfa_hciu_get_response_name(NFA_HCI_ANY_E_NOT_CONNECTED), "ANY_E_NOT_CONNECTED");
    EXPECT_EQ(nfa_hciu_get_response_name(NFA_HCI_ANY_E_CMD_PAR_UNKNOWN), "ANY_E_CMD_PAR_UNKNOWN");
    EXPECT_EQ(nfa_hciu_get_response_name(NFA_HCI_ANY_E_NOK), "ANY_E_NOK");
    EXPECT_EQ(nfa_hciu_get_response_name(
            NFA_HCI_ADM_E_NO_PIPES_AVAILABLE), "ADM_E_NO_PIPES_AVAILABLE");
    EXPECT_EQ(nfa_hciu_get_response_name(NFA_HCI_ANY_E_REG_PAR_UNKNOWN), "ANY_E_REG_PAR_UNKNOWN");
    EXPECT_EQ(nfa_hciu_get_response_name(NFA_HCI_ANY_E_PIPE_NOT_OPENED), "ANY_E_PIPE_NOT_OPENED");
    EXPECT_EQ(nfa_hciu_get_response_name(
            NFA_HCI_ANY_E_CMD_NOT_SUPPORTED), "ANY_E_CMD_NOT_SUPPORTED");
    EXPECT_EQ(nfa_hciu_get_response_name(NFA_HCI_ANY_E_INHIBITED), "ANY_E_INHIBITED");
    EXPECT_EQ(nfa_hciu_get_response_name(NFA_HCI_ANY_E_TIMEOUT), "ANY_E_TIMEOUT");
    EXPECT_EQ(nfa_hciu_get_response_name(
            NFA_HCI_ANY_E_REG_ACCESS_DENIED), "ANY_E_REG_ACCESS_DENIED");
    EXPECT_EQ(nfa_hciu_get_response_name(
            NFA_HCI_ANY_E_PIPE_ACCESS_DENIED), "ANY_E_PIPE_ACCESS_DENIED");
}

TEST_F(NfaHciGetResponseNameTest, MaxResponseCode) {
    uint8_t rsp_code = 0xFF;
    std::string expected_response = "UNKNOWN";
    EXPECT_EQ(nfa_hciu_get_response_name(rsp_code), expected_response);
}

class NfaHciGetStateNameTest : public ::testing::Test {
protected:
};

TEST_F(NfaHciGetStateNameTest, ValidStateCodes) {
    EXPECT_EQ(nfa_hciu_get_state_name(NFA_HCI_STATE_DISABLED), "DISABLED");
    EXPECT_EQ(nfa_hciu_get_state_name(NFA_HCI_STATE_STARTUP), "STARTUP");
    EXPECT_EQ(nfa_hciu_get_state_name(NFA_HCI_STATE_WAIT_NETWK_ENABLE), "WAIT_NETWK_ENABLE");
    EXPECT_EQ(nfa_hciu_get_state_name(NFA_HCI_STATE_IDLE), "IDLE");
    EXPECT_EQ(nfa_hciu_get_state_name(NFA_HCI_STATE_WAIT_RSP), "WAIT_RSP");
    EXPECT_EQ(nfa_hciu_get_state_name(NFA_HCI_STATE_REMOVE_GATE), "REMOVE_GATE");
    EXPECT_EQ(nfa_hciu_get_state_name(NFA_HCI_STATE_APP_DEREGISTER), "APP_DEREGISTER");
    EXPECT_EQ(nfa_hciu_get_state_name(NFA_HCI_STATE_RESTORE), "RESTORE");
    EXPECT_EQ(nfa_hciu_get_state_name(
            NFA_HCI_STATE_RESTORE_NETWK_ENABLE), "WAIT_NETWK_ENABLE_AFTER_RESTORE");
}

TEST_F(NfaHciGetStateNameTest, MaxStateCode) {
    uint8_t state = 0xFF;
    std::string expected_state = "UNKNOWN";
EXPECT_EQ(nfa_hciu_get_state_name(state), expected_state);
}

class NfaHciuInstrTest : public ::testing::Test {
protected:
    void SetUp() override {
    }
};

TEST_F(NfaHciuInstrTest, AnySetParameter) {
    uint8_t instruction = NFA_HCI_ANY_SET_PARAMETER;
    std::string result = nfa_hciu_instr_2_str(instruction);
    EXPECT_EQ(result, "ANY_SET_PARAMETER");
}

TEST_F(NfaHciuInstrTest, AnyGetParameter) {
    uint8_t instruction = NFA_HCI_ANY_GET_PARAMETER;
    std::string result = nfa_hciu_instr_2_str(instruction);
    EXPECT_EQ(result, "ANY_GET_PARAMETER");
}

TEST_F(NfaHciuInstrTest, AnyOpenPipe) {
    uint8_t instruction = NFA_HCI_ANY_OPEN_PIPE;
    std::string result = nfa_hciu_instr_2_str(instruction);
    EXPECT_EQ(result, "ANY_OPEN_PIPE");
}

TEST_F(NfaHciuInstrTest, AnyClosePipe) {
    uint8_t instruction = NFA_HCI_ANY_CLOSE_PIPE;
    std::string result = nfa_hciu_instr_2_str(instruction);
    EXPECT_EQ(result, "ANY_CLOSE_PIPE");
}

TEST_F(NfaHciuInstrTest, AdmCreatePipe) {
    uint8_t instruction = NFA_HCI_ADM_CREATE_PIPE;
    std::string result = nfa_hciu_instr_2_str(instruction);
    EXPECT_EQ(result, "ADM_CREATE_PIPE");
}

TEST_F(NfaHciuInstrTest, AdmDeletePipe) {
    uint8_t instruction = NFA_HCI_ADM_DELETE_PIPE;
    std::string result = nfa_hciu_instr_2_str(instruction);
    EXPECT_EQ(result, "ADM_DELETE_PIPE");
}

TEST_F(NfaHciuInstrTest, AdmNotifyPipeCreated) {
    uint8_t instruction = NFA_HCI_ADM_NOTIFY_PIPE_CREATED;
    std::string result = nfa_hciu_instr_2_str(instruction);
    EXPECT_EQ(result, "ADM_NOTIFY_PIPE_CREATED");
}

TEST_F(NfaHciuInstrTest, AdmNotifyPipeDeleted) {
    uint8_t instruction = NFA_HCI_ADM_NOTIFY_PIPE_DELETED;
    std::string result = nfa_hciu_instr_2_str(instruction);
    EXPECT_EQ(result, "ADM_NOTIFY_PIPE_DELETED");
}

TEST_F(NfaHciuInstrTest, AdmClearAllPipe) {
    uint8_t instruction = NFA_HCI_ADM_CLEAR_ALL_PIPE;
    std::string result = nfa_hciu_instr_2_str(instruction);
    EXPECT_EQ(result, "ADM_CLEAR_ALL_PIPE");
}

TEST_F(NfaHciuInstrTest, AdmNotifyAllPipeCleared) {
    uint8_t instruction = NFA_HCI_ADM_NOTIFY_ALL_PIPE_CLEARED;
    std::string result = nfa_hciu_instr_2_str(instruction);
    EXPECT_EQ(result, "ADM_NOTIFY_ALL_PIPE_CLEARED");
}

TEST_F(NfaHciuInstrTest, UnknownInstruction) {
    uint8_t instruction = 0xFF;  // Unknown instruction value
    std::string result = nfa_hciu_instr_2_str(instruction);
    EXPECT_EQ(result, "UNKNOWN");
}

class NfaHciuIsActiveHostTest : public ::testing::Test {
protected:
    void SetUp() override {
        memset(nfa_hci_cb.active_host, 0, sizeof(nfa_hci_cb.active_host));
    }
};

TEST_F(NfaHciuIsActiveHostTest, HostIdUicc0) {
    uint8_t host_id = NFA_HCI_HOST_ID_UICC0;
    nfa_hci_cb.active_host[0] = host_id;
    bool result = nfa_hciu_is_active_host(host_id);
    EXPECT_TRUE(result);
}

TEST_F(NfaHciuIsActiveHostTest, HostIdDynamicallyAllocated) {
    uint8_t host_id = NFA_HCI_HOST_ID_FIRST_DYNAMICALLY_ALLOCATED;
    nfa_hci_cb.active_host[1] = host_id;
    bool result = nfa_hciu_is_active_host(host_id);
    EXPECT_TRUE(result);
}

TEST_F(NfaHciuIsActiveHostTest, HostIdOutOfRange) {
    uint8_t host_id = 0xFF;
    bool result = nfa_hciu_is_active_host(host_id);
    EXPECT_FALSE(result);
}

TEST_F(NfaHciuIsActiveHostTest, HostIdActiveNotUicc0OrDynamicallyAllocated) {
    uint8_t host_id = 0x05;
    nfa_hci_cb.active_host[2] = host_id;
    bool result = nfa_hciu_is_active_host(host_id);
    EXPECT_FALSE(result);
}

TEST_F(NfaHciuIsActiveHostTest, NoHostActive) {
    uint8_t host_id = NFA_HCI_HOST_ID_UICC0;
    bool result = nfa_hciu_is_active_host(host_id);
    EXPECT_FALSE(result);
}

TEST_F(NfaHciuIsActiveHostTest, MultipleHostsActive) {
    uint8_t host_id_1 = NFA_HCI_HOST_ID_UICC0;
    uint8_t host_id_2 = NFA_HCI_HOST_ID_FIRST_DYNAMICALLY_ALLOCATED;
    nfa_hci_cb.active_host[0] = host_id_1;
    nfa_hci_cb.active_host[1] = host_id_2;
    bool result_1 = nfa_hciu_is_active_host(host_id_1);
    bool result_2 = nfa_hciu_is_active_host(host_id_2);
    EXPECT_TRUE(result_1);
    EXPECT_TRUE(result_2);
}

class NfaHciuIsHostResetingTest : public ::testing::Test {
protected:
    void SetUp() override {
        memset(nfa_hci_cb.reset_host, 0, sizeof(nfa_hci_cb.reset_host));
    }
};

TEST_F(NfaHciuIsHostResetingTest, HostIdUicc0Reseting) {
    uint8_t host_id = NFA_HCI_HOST_ID_UICC0;
    nfa_hci_cb.reset_host[0] = host_id;
    bool result = nfa_hciu_is_host_reseting(host_id);
    EXPECT_TRUE(result);
}

TEST_F(NfaHciuIsHostResetingTest, HostIdDynamicallyAllocatedReseting) {
    uint8_t host_id = NFA_HCI_HOST_ID_FIRST_DYNAMICALLY_ALLOCATED;
    nfa_hci_cb.reset_host[1] = host_id;
    bool result = nfa_hciu_is_host_reseting(host_id);
    EXPECT_TRUE(result);
}

TEST_F(NfaHciuIsHostResetingTest, HostIdNotReseting) {
    uint8_t host_id = 0x10;
    bool result = nfa_hciu_is_host_reseting(host_id);
    EXPECT_FALSE(result);
}

TEST_F(NfaHciuIsHostResetingTest, HostIdOutOfRange) {
    uint8_t host_id = 0xFF;
    bool result = nfa_hciu_is_host_reseting(host_id);
    EXPECT_FALSE(result);
}

TEST_F(NfaHciuIsHostResetingTest, HostIdResetingNotUicc0OrDynamicallyAllocated) {
    uint8_t host_id = 0x05;
    nfa_hci_cb.reset_host[2] = host_id;
    bool result = nfa_hciu_is_host_reseting(host_id);
    EXPECT_FALSE(result);
}

TEST_F(NfaHciuIsHostResetingTest, NoHostReseting) {
    uint8_t host_id = NFA_HCI_HOST_ID_UICC0;
    bool result = nfa_hciu_is_host_reseting(host_id);
    EXPECT_FALSE(result);
}

TEST_F(NfaHciuIsHostResetingTest, MultipleHostsReseting) {
    uint8_t host_id_1 = NFA_HCI_HOST_ID_UICC0;
    uint8_t host_id_2 = NFA_HCI_HOST_ID_FIRST_DYNAMICALLY_ALLOCATED;
    nfa_hci_cb.reset_host[0] = host_id_1;
    nfa_hci_cb.reset_host[1] = host_id_2;
    bool result_1 = nfa_hciu_is_host_reseting(host_id_1);
    bool result_2 = nfa_hciu_is_host_reseting(host_id_2);
    EXPECT_TRUE(result_1);
    EXPECT_TRUE(result_2);
}

class NfaHciuReleaseGateTest : public ::testing::Test {
protected:
    void SetUp() override {
        memset(nfa_hci_cb.cfg.dyn_gates, 0, sizeof(nfa_hci_cb.cfg.dyn_gates));
        nfa_hci_cb.nv_write_needed = false;
    }
    void AddGate(uint8_t gate_id, uint16_t gate_owner, uint32_t pipe_inx_mask) {
        tNFA_HCI_DYN_GATE* p_gate = &nfa_hci_cb.cfg.dyn_gates[0];
        p_gate->gate_id = gate_id;
        p_gate->gate_owner = gate_owner;
        p_gate->pipe_inx_mask = pipe_inx_mask;
    }
    bool IsGateReleased(uint8_t gate_id) {
        tNFA_HCI_DYN_GATE* p_gate = nfa_hciu_find_gate_by_gid(gate_id);
        return (p_gate == nullptr) || (
                p_gate->gate_id == 0 && p_gate->gate_owner == 0 && p_gate->pipe_inx_mask == 0);
    }
};

TEST_F(NfaHciuReleaseGateTest, ReleaseExistingGate) {
    uint8_t gate_id = 0x01;
    uint16_t gate_owner = 0x1234;
    uint16_t pipe_inx_mask = 0x5678;
    AddGate(gate_id, gate_owner, pipe_inx_mask);
    nfa_hciu_release_gate(gate_id);
    EXPECT_TRUE(IsGateReleased(gate_id));
    EXPECT_TRUE(nfa_hci_cb.nv_write_needed);
}

TEST_F(NfaHciuReleaseGateTest, ReleaseNonExistentGate) {
    uint8_t gate_id = 0x02;
    nfa_hciu_release_gate(gate_id);
    EXPECT_TRUE(IsGateReleased(gate_id));
    EXPECT_FALSE(nfa_hci_cb.nv_write_needed);
}

TEST_F(NfaHciuReleaseGateTest, ReleaseMultipleGates) {
    uint8_t gate_id_1 = 0x01, gate_id_2 = 0x02;
    uint16_t gate_owner = 0x1234;
    uint16_t pipe_inx_mask = 0x5678;
    AddGate(gate_id_1, gate_owner, pipe_inx_mask);
    AddGate(gate_id_2, gate_owner, pipe_inx_mask);
    nfa_hciu_release_gate(gate_id_1);
    nfa_hciu_release_gate(gate_id_2);
    EXPECT_TRUE(IsGateReleased(gate_id_1));
    EXPECT_TRUE(IsGateReleased(gate_id_2));
    EXPECT_TRUE(nfa_hci_cb.nv_write_needed);
}

TEST_F(NfaHciuReleaseGateTest, ReleaseInvalidGateId) {
    uint8_t invalid_gate_id = 0xFF;
    nfa_hciu_release_gate(invalid_gate_id);
    EXPECT_TRUE(IsGateReleased(invalid_gate_id));
    EXPECT_FALSE(nfa_hci_cb.nv_write_needed);
}

TEST_F(NfaHciuReleaseGateTest, ReleaseWithoutInitializedGates) {
    uint8_t gate_id = 0x01;
    nfa_hciu_release_gate(gate_id);
    EXPECT_TRUE(IsGateReleased(gate_id));
    EXPECT_FALSE(nfa_hci_cb.nv_write_needed);
}

class NfaHciuReleasePipeTest : public ::testing::Test {
protected:
    void SetUp() override {
        memset(nfa_hci_cb.cfg.dyn_pipes, 0, sizeof(nfa_hci_cb.cfg.dyn_pipes));
        nfa_hci_cb.nv_write_needed = false;
    }
    void AddPipe(uint8_t pipe_id, uint8_t local_gate) {
        tNFA_HCI_DYN_PIPE* p_pipe = &nfa_hci_cb.cfg.dyn_pipes[0];
        p_pipe->pipe_id = pipe_id;
        p_pipe->local_gate = local_gate;
    }
    bool IsPipeRemoved(uint8_t pipe_id) {
        tNFA_HCI_DYN_PIPE* p_pipe = nfa_hciu_find_pipe_by_pid(pipe_id);
        return (p_pipe == nullptr || p_pipe->pipe_id == 0);
    }
};

TEST_F(NfaHciuReleasePipeTest, ReleaseExistingPipe) {
    uint8_t pipe_id = 0x01;
    uint8_t local_gate = 0x02;
    uint32_t pipe_mask = 0x01;
    AddPipe(pipe_id, local_gate);
    tNFA_HCI_RESPONSE result = nfa_hciu_release_pipe(pipe_id);
    EXPECT_EQ(result, NFA_HCI_ANY_E_NOK);
    EXPECT_TRUE(IsPipeRemoved(pipe_id));
    EXPECT_FALSE(nfa_hci_cb.nv_write_needed);
}

TEST_F(NfaHciuReleasePipeTest, ReleaseNonExistentPipe) {
    uint8_t pipe_id = 0xFF;
    tNFA_HCI_RESPONSE result = nfa_hciu_release_pipe(pipe_id);
    EXPECT_EQ(result, NFA_HCI_ANY_E_NOK);
    EXPECT_FALSE(nfa_hci_cb.nv_write_needed);
}

TEST_F(NfaHciuReleasePipeTest, ReleaseInvalidPipeId) {
    uint8_t invalid_pipe_id = NFA_HCI_LAST_DYNAMIC_PIPE + 1;
    tNFA_HCI_RESPONSE result = nfa_hciu_release_pipe(invalid_pipe_id);
    EXPECT_EQ(result, NFA_HCI_ANY_E_NOK);
    EXPECT_FALSE(nfa_hci_cb.nv_write_needed);
}

TEST_F(NfaHciuReleasePipeTest, ReleasePipeWithIdMgmtGate) {
    uint8_t pipe_id = 0x01;
    uint8_t local_gate = NFA_HCI_IDENTITY_MANAGEMENT_GATE;
    uint32_t pipe_mask = 0x01;
    AddPipe(pipe_id, local_gate);
    tNFA_HCI_RESPONSE result = nfa_hciu_release_pipe(pipe_id);
    EXPECT_EQ(result, NFA_HCI_ANY_OK);
    EXPECT_TRUE(IsPipeRemoved(pipe_id));
    EXPECT_TRUE(nfa_hci_cb.nv_write_needed);
}

TEST_F(NfaHciuReleasePipeTest, ReleasePipeWithNonExistentGate) {
    uint8_t pipe_id = 0x02;
    uint8_t local_gate = 0x03;
    uint32_t pipe_mask = 0x01;
    AddPipe(pipe_id, local_gate);
    tNFA_HCI_RESPONSE result = nfa_hciu_release_pipe(pipe_id);
    EXPECT_EQ(result, NFA_HCI_ANY_E_NOK);
    EXPECT_TRUE(IsPipeRemoved(pipe_id));
    EXPECT_FALSE(nfa_hci_cb.nv_write_needed);
}

TEST_F(NfaHciuReleasePipeTest, ReleaseWithoutInitializedPipes) {
    uint8_t pipe_id = 0x01;
    tNFA_HCI_RESPONSE result = nfa_hciu_release_pipe(pipe_id);
    EXPECT_EQ(result, NFA_HCI_ANY_E_NOK);
    EXPECT_FALSE(nfa_hci_cb.nv_write_needed);
}

class NfaHciuRemoveAllPipesFromHostTest : public ::testing::Test {
protected:
    void SetUp() override {
        memset(nfa_hci_cb.cfg.dyn_pipes, 0, sizeof(nfa_hci_cb.cfg.dyn_pipes));
        nfa_hci_cb.nv_write_needed = false;
    }
    void AddPipe(uint8_t pipe_id, uint8_t host, uint8_t local_gate) {
        tNFA_HCI_DYN_PIPE* p_pipe = &nfa_hci_cb.cfg.dyn_pipes[0];
        p_pipe->pipe_id = pipe_id;
        p_pipe->dest_host = host;
        p_pipe->local_gate = local_gate;
    }
    bool IsPipeRemoved(uint8_t pipe_id) {
        tNFA_HCI_DYN_PIPE* p_pipe = nfa_hciu_find_pipe_by_pid(pipe_id);
        return (p_pipe == nullptr || p_pipe->pipe_id == 0);
    }
};

TEST_F(NfaHciuRemoveAllPipesFromHostTest, RemovePipesFromHost) {
    uint8_t host_id = 0x01;
    uint8_t pipe_id_1 = 0x01;
    uint8_t pipe_id_2 = 0x02;
    uint8_t local_gate = 0x02;
    AddPipe(pipe_id_1, host_id, local_gate);
    AddPipe(pipe_id_2, host_id, local_gate);
    nfa_hciu_remove_all_pipes_from_host(host_id);
    EXPECT_TRUE(IsPipeRemoved(pipe_id_1));
    EXPECT_TRUE(IsPipeRemoved(pipe_id_2));
    EXPECT_FALSE(nfa_hci_cb.nv_write_needed);
}

TEST_F(NfaHciuRemoveAllPipesFromHostTest, NoPipesToRemove) {
    uint8_t host_id = 0x01;
    nfa_hciu_remove_all_pipes_from_host(host_id);
    EXPECT_FALSE(nfa_hci_cb.nv_write_needed);
}

TEST_F(NfaHciuRemoveAllPipesFromHostTest, NoPipesAtAll) {
    uint8_t host_id = 0x01;
    nfa_hciu_remove_all_pipes_from_host(host_id);
    EXPECT_FALSE(nfa_hci_cb.nv_write_needed);
}

TEST_F(NfaHciuRemoveAllPipesFromHostTest, OnlySomePipesBelongToHost) {
    uint8_t host_id = 0x01;
    uint8_t pipe_id_1 = 0x01;
    uint8_t pipe_id_2 = 0x02;
    uint8_t local_gate = 0x02;
    AddPipe(pipe_id_1, host_id, local_gate);
    AddPipe(pipe_id_2, 0x02, local_gate);
    nfa_hciu_remove_all_pipes_from_host(host_id);
    EXPECT_TRUE(IsPipeRemoved(pipe_id_1));
    EXPECT_FALSE(IsPipeRemoved(pipe_id_2));
    EXPECT_FALSE(nfa_hci_cb.nv_write_needed);
}

TEST_F(NfaHciuRemoveAllPipesFromHostTest, InvalidPipeId) {
    uint8_t host_id = 0x01;
    uint8_t pipe_id_invalid = NFA_HCI_LAST_DYNAMIC_PIPE + 1;
    AddPipe(pipe_id_invalid, host_id, 0x02);
    nfa_hciu_remove_all_pipes_from_host(host_id);
    EXPECT_FALSE(IsPipeRemoved(pipe_id_invalid));
}

TEST_F(NfaHciuRemoveAllPipesFromHostTest, HostWithPipesNoGates) {
    uint8_t host_id = 0x01;
    uint8_t pipe_id = 0x01;
    uint8_t local_gate = 0x02;
    AddPipe(pipe_id, host_id, local_gate);
    nfa_hciu_remove_all_pipes_from_host(host_id);
    EXPECT_TRUE(IsPipeRemoved(pipe_id));
    EXPECT_FALSE(nfa_hci_cb.nv_write_needed);
}

class NfaHciuSendToAllAppsTest : public ::testing::Test {
protected:
    static void app_callback(tNFA_HCI_EVT event, tNFA_HCI_EVT_DATA* p_evt) {
        (void)event;
        (void)p_evt;
        callback_called = true;
    }
    static bool callback_called;
    void SetUp() override {
        memset(&nfa_hci_cb, 0, sizeof(nfa_hci_cb));
    }
    void TearDown() override {
        callback_called = false;
    }
};
#define NFA_HCI_EVT_DATA_READY 1
bool NfaHciuSendToAllAppsTest::callback_called = false;

TEST_F(NfaHciuSendToAllAppsTest, SendToAllAppsWithValidCallbacks) {
    tNFA_HCI_EVT event = NFA_HCI_EVT_DATA_READY;
    tNFA_HCI_EVT_DATA evt_data;
    nfa_hci_cb.p_app_cback[0] = app_callback;
    nfa_hci_cb.p_app_cback[1] = app_callback;
    nfa_hciu_send_to_all_apps(event, &evt_data);
    EXPECT_TRUE(callback_called);
}

TEST_F(NfaHciuSendToAllAppsTest, SendToAllAppsWithSomeNullCallbacks) {
    tNFA_HCI_EVT event = NFA_HCI_EVT_DATA_READY;
    tNFA_HCI_EVT_DATA evt_data;
    nfa_hci_cb.p_app_cback[0] = app_callback;
    nfa_hci_cb.p_app_cback[1] = nullptr;
    nfa_hciu_send_to_all_apps(event, &evt_data);
    EXPECT_TRUE(callback_called);
}

TEST_F(NfaHciuSendToAllAppsTest, SendToAllAppsWithNoValidCallbacks) {
    tNFA_HCI_EVT event = NFA_HCI_EVT_DATA_READY;
    tNFA_HCI_EVT_DATA evt_data;
    nfa_hci_cb.p_app_cback[0] = nullptr;
    nfa_hci_cb.p_app_cback[1] = nullptr;
    nfa_hciu_send_to_all_apps(event, &evt_data);
    EXPECT_FALSE(callback_called);
}

TEST_F(NfaHciuSendToAllAppsTest, SendToAllAppsWithNullEventData) {
    tNFA_HCI_EVT event = NFA_HCI_EVT_DATA_READY;
    nfa_hci_cb.p_app_cback[0] = app_callback;
    nfa_hciu_send_to_all_apps(event, nullptr);
    EXPECT_TRUE(callback_called);
}

TEST_F(NfaHciuSendToAllAppsTest, SendToAllAppsWithValidEventData) {
    tNFA_HCI_EVT event = NFA_HCI_EVT_DATA_READY;
    tNFA_HCI_EVT_DATA evt_data;
    nfa_hci_cb.p_app_cback[0] = app_callback;
    nfa_hci_cb.p_app_cback[1] = app_callback;
    nfa_hciu_send_to_all_apps(event, &evt_data);
    EXPECT_TRUE(callback_called);
}

class NfaHciuSendToAppTest : public ::testing::Test {
protected:
    static bool callback_called;
    static void app_callback(tNFA_HCI_EVT event, tNFA_HCI_EVT_DATA* p_evt) {
        (void)event;
        (void)p_evt;
        callback_called = true;
    }
    void SetUp() override {
        memset(&nfa_hci_cb, 0, sizeof(nfa_hci_cb));
        callback_called = false;
    }
    void TearDown() override {
        callback_called = false;
    }
};

bool NfaHciuSendToAppTest::callback_called = false;

TEST_F(NfaHciuSendToAppTest, SendToAppWithValidHandleAndCallback) {
    tNFA_HCI_EVT event = NFA_HCI_EVT_DATA_READY;
    tNFA_HCI_EVT_DATA evt_data;
    tNFA_HANDLE app_handle = 0x01;
    nfa_hci_cb.p_app_cback[app_handle] = app_callback;
    nfa_hciu_send_to_app(event, &evt_data, app_handle);
    EXPECT_FALSE(callback_called);
}
TEST_F(NfaHciuSendToAppTest, SendToAppWithValidHandleButNoCallback) {
    tNFA_HCI_EVT event = NFA_HCI_EVT_DATA_READY;
    tNFA_HCI_EVT_DATA evt_data;
    tNFA_HANDLE app_handle = 0x01;
    nfa_hci_cb.p_app_cback[app_handle] = nullptr;
    nfa_hciu_send_to_app(event, &evt_data, app_handle);
    EXPECT_FALSE(callback_called);
}

TEST_F(NfaHciuSendToAppTest, SendToAppWithInvalidHandle) {
    tNFA_HCI_EVT event = NFA_HCI_EVT_DATA_READY;
    tNFA_HCI_EVT_DATA evt_data;
    tNFA_HANDLE app_handle = 0xFF;
    nfa_hciu_send_to_app(event, &evt_data, app_handle);
    EXPECT_FALSE(callback_called);
}

TEST_F(NfaHciuSendToAppTest, SendToAppWithInvalidHandleGroup) {
    tNFA_HCI_EVT event = NFA_HCI_EVT_DATA_READY;
    tNFA_HCI_EVT_DATA evt_data;
    tNFA_HANDLE app_handle = 0x02;
    nfa_hciu_send_to_app(event, &evt_data, app_handle);
    EXPECT_FALSE(callback_called);
}
TEST_F(NfaHciuSendToAppTest, SendToAppWithNoCallbackAndInvalidHandle) {
    tNFA_HCI_EVT event = NFA_HCI_EVT_DATA_READY;
    tNFA_HCI_EVT_DATA evt_data;
    tNFA_HANDLE app_handle = NFA_HANDLE_INVALID;
    nfa_hciu_send_to_app(event, &evt_data, app_handle);
    EXPECT_FALSE(callback_called);
}


class NfaHciuSendToAppsHandlingConnectivityEvtsTest : public ::testing::Test {
protected:
    static bool callback_called;
    static void app_callback(tNFA_HCI_EVT event, tNFA_HCI_EVT_DATA* p_evt) {
        (void)event;
        (void)p_evt;
        callback_called = true;
    }
    void SetUp() override {
        memset(&nfa_hci_cb, 0, sizeof(nfa_hci_cb));
        callback_called = false;
    }

    void TearDown() override {
        callback_called = false;
    }
};

bool NfaHciuSendToAppsHandlingConnectivityEvtsTest::callback_called = false;

TEST_F(NfaHciuSendToAppsHandlingConnectivityEvtsTest, NoAppsInterested) {
    memset(nfa_hci_cb.cfg.b_send_conn_evts, 0, sizeof(nfa_hci_cb.cfg.b_send_conn_evts));
    tNFA_HCI_EVT event = NFA_HCI_EVT_DATA_READY;
    tNFA_HCI_EVT_DATA event_data;
    nfa_hciu_send_to_apps_handling_connectivity_evts(event, &event_data);
    EXPECT_EQ(callback_called, false);
}

TEST_F(NfaHciuSendToAppsHandlingConnectivityEvtsTest, OneAppInterested) {
    memset(nfa_hci_cb.cfg.b_send_conn_evts, 0, sizeof(nfa_hci_cb.cfg.b_send_conn_evts));
    nfa_hci_cb.cfg.b_send_conn_evts[0] = true;
    nfa_hci_cb.p_app_cback[0] = app_callback;
    tNFA_HCI_EVT event = NFA_HCI_EVT_DATA_READY;
    tNFA_HCI_EVT_DATA event_data;
    nfa_hciu_send_to_apps_handling_connectivity_evts(event, &event_data);
    EXPECT_EQ(callback_called, true);
}

TEST_F(NfaHciuSendToAppsHandlingConnectivityEvtsTest, MultipleAppsInterested) {
    memset(nfa_hci_cb.cfg.b_send_conn_evts, 0, sizeof(nfa_hci_cb.cfg.b_send_conn_evts));
    nfa_hci_cb.cfg.b_send_conn_evts[0] = true;
    nfa_hci_cb.cfg.b_send_conn_evts[1] = true;
    nfa_hci_cb.p_app_cback[0] = app_callback;
    nfa_hci_cb.p_app_cback[1] = app_callback;
    tNFA_HCI_EVT event = NFA_HCI_EVT_DATA_READY;
    tNFA_HCI_EVT_DATA event_data;
    nfa_hciu_send_to_apps_handling_connectivity_evts(event, &event_data);
    EXPECT_EQ(callback_called, true);
}

TEST_F(NfaHciuSendToAppsHandlingConnectivityEvtsTest, CallbackIsNull) {
    memset(nfa_hci_cb.cfg.b_send_conn_evts, 0, sizeof(nfa_hci_cb.cfg.b_send_conn_evts));
    nfa_hci_cb.cfg.b_send_conn_evts[0] = true;
    nfa_hci_cb.p_app_cback[0] = nullptr;
    tNFA_HCI_EVT event = NFA_HCI_EVT_DATA_READY;
    tNFA_HCI_EVT_DATA event_data;
    nfa_hciu_send_to_apps_handling_connectivity_evts(event, &event_data);
    EXPECT_EQ(callback_called, false);
}

TEST_F(NfaHciuSendToAppsHandlingConnectivityEvtsTest, NoConnectivityEventsSentIfFlagsFalse) {
    memset(nfa_hci_cb.cfg.b_send_conn_evts, 0, sizeof(nfa_hci_cb.cfg.b_send_conn_evts));
    nfa_hci_cb.p_app_cback[0] = app_callback;
    nfa_hci_cb.p_app_cback[1] = app_callback;
    tNFA_HCI_EVT event = NFA_HCI_EVT_DATA_READY;
    tNFA_HCI_EVT_DATA event_data;
    nfa_hciu_send_to_apps_handling_connectivity_evts(event, &event_data);
    EXPECT_EQ(callback_called, false);
}

TEST_F(NfaHciuSendToAppsHandlingConnectivityEvtsTest, EmptyEventData) {
    memset(nfa_hci_cb.cfg.b_send_conn_evts, 0, sizeof(nfa_hci_cb.cfg.b_send_conn_evts));
    nfa_hci_cb.cfg.b_send_conn_evts[0] = true;
    nfa_hci_cb.p_app_cback[0] = app_callback;
    tNFA_HCI_EVT event = NFA_HCI_EVT_DATA_READY;
    tNFA_HCI_EVT_DATA event_data = {};
    nfa_hciu_send_to_apps_handling_connectivity_evts(event, &event_data);
    EXPECT_EQ(callback_called, true);
}

class NfaHciuType2StrTest : public ::testing::Test {
protected:
};

TEST_F(NfaHciuType2StrTest, CommandType) {
    uint8_t type = NFA_HCI_COMMAND_TYPE;
    std::string result = nfa_hciu_type_2_str(type);
    EXPECT_EQ(result, "COMMAND");
}

TEST_F(NfaHciuType2StrTest, EventType) {
    uint8_t type = NFA_HCI_EVENT_TYPE;
    std::string result = nfa_hciu_type_2_str(type);
    EXPECT_EQ(result, "EVENT");
}

TEST_F(NfaHciuType2StrTest, ResponseType) {
    uint8_t type = NFA_HCI_RESPONSE_TYPE;
    std::string result = nfa_hciu_type_2_str(type);
    EXPECT_EQ(result, "RESPONSE");
}

TEST_F(NfaHciuType2StrTest, UnknownType) {
    uint8_t type = 0xFF;  // An unknown type (invalid)
    std::string result = nfa_hciu_type_2_str(type);
    EXPECT_EQ(result, "UNKNOWN");
}

class NfaHciuFindGateByGidTest : public ::testing::Test {
protected:
    void SetUp() override {
        for (int i = 0; i < NFA_HCI_MAX_GATE_CB; i++) {
            nfa_hci_cb.cfg.dyn_gates[i].gate_id = i + 1;
        }
    }
    void TearDown() override {}
};

TEST_F(NfaHciuFindGateByGidTest, FindExistingGate) {
    uint8_t gate_id = 3;
    tNFA_HCI_DYN_GATE* gate = nfa_hciu_find_gate_by_gid(gate_id);
    EXPECT_NE(gate, nullptr);
    EXPECT_EQ(gate->gate_id, gate_id);
}

TEST_F(NfaHciuFindGateByGidTest, FindNonExistingGate) {
    uint8_t gate_id = 230;
    tNFA_HCI_DYN_GATE* gate = nfa_hciu_find_gate_by_gid(gate_id);
    EXPECT_EQ(gate, nullptr);
}

TEST_F(NfaHciuFindGateByGidTest, FindFirstGate) {
    uint8_t gate_id = 1;
    tNFA_HCI_DYN_GATE* gate = nfa_hciu_find_gate_by_gid(gate_id);
    EXPECT_NE(gate, nullptr);
    EXPECT_EQ(gate->gate_id, gate_id);
}

TEST_F(NfaHciuFindGateByGidTest, FindLastGate) {
    uint8_t gate_id = NFA_HCI_MAX_GATE_CB;
    tNFA_HCI_DYN_GATE* gate = nfa_hciu_find_gate_by_gid(gate_id);
    EXPECT_NE(gate, nullptr);
    EXPECT_EQ(gate->gate_id, gate_id);
}

TEST_F(NfaHciuFindGateByGidTest, EmptyGateArray) {
    for (int i = 0; i < NFA_HCI_MAX_GATE_CB; i++) {
        nfa_hci_cb.cfg.dyn_gates[i].gate_id = 0;
    }
    uint8_t gate_id = 5;
    tNFA_HCI_DYN_GATE* gate = nfa_hciu_find_gate_by_gid(gate_id);
    EXPECT_EQ(gate, nullptr);
}

class NfaHciuResettingTest : public ::testing::Test {
protected:
    void SetUp() override {
        memset(nfa_hci_cb.reset_host, 0, sizeof(nfa_hci_cb.reset_host));
    }
    void SetHostResetting(int host_index) {
        if (host_index >= 0 && host_index < NFA_HCI_MAX_HOST_IN_NETWORK) {
            nfa_hci_cb.reset_host[host_index] = 1;
        }
    }
};

TEST_F(NfaHciuResettingTest, NoHostResetting) {
    EXPECT_TRUE(nfa_hciu_is_no_host_resetting());
}

TEST_F(NfaHciuResettingTest, OneHostResetting) {
    SetHostResetting(2);
    EXPECT_FALSE(nfa_hciu_is_no_host_resetting());
}

TEST_F(NfaHciuResettingTest, MultipleHostsResetting) {
    SetHostResetting(1);
    SetHostResetting(3);
    EXPECT_FALSE(nfa_hciu_is_no_host_resetting());
}

TEST_F(NfaHciuResettingTest, AllHostsResetting) {
    for (int i = 0; i < NFA_HCI_MAX_HOST_IN_NETWORK; ++i) {
        SetHostResetting(i);
    }
    EXPECT_FALSE(nfa_hciu_is_no_host_resetting());
}
