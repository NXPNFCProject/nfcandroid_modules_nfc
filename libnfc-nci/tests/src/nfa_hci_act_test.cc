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
#include "nfa_hci_act.cc"

class NfaHciCheckPendingApiRequestsTest : public ::testing::Test {
protected:
    void SetUp() override {
        memset(&nfa_hci_cb, 0, sizeof(nfa_hci_cb));
        GKI_create_pool(0, 256, 10, nullptr);
    }
    void TearDown() override{
        memset(&nfa_hci_cb, 0, sizeof(nfa_hci_cb));
    }
    NFC_HDR* CreateNfcHdr(uint16_t event, uint16_t length) {
        NFC_HDR* p_msg = (NFC_HDR*)GKI_getbuf(sizeof(NFC_HDR) + length);
        p_msg->event = event;
        p_msg->len = length;
        return p_msg;
    }
    tNFA_HCI_EVENT_DATA* FillEventData(NFC_HDR* p_msg, uint8_t hci_handle) {
        tNFA_HCI_EVENT_DATA* p_evt_data = (tNFA_HCI_EVENT_DATA*)p_msg;
        p_evt_data->comm.hci_handle = hci_handle;
        return p_evt_data;
    }
};

TEST_F(NfaHciCheckPendingApiRequestsTest, ProcessCreatePipeEvent) {
    nfa_hci_cb.hci_state = NFA_HCI_STATE_IDLE;
    NFC_HDR* p_msg = CreateNfcHdr(NFA_HCI_API_CREATE_PIPE_EVT, sizeof(tNFA_HCI_EVENT_DATA));
    tNFA_HCI_EVENT_DATA* p_evt_data = FillEventData(p_msg, 1);
    GKI_enqueue(&nfa_hci_cb.hci_host_reset_api_q, p_msg);
    nfa_hci_check_pending_api_requests();
    EXPECT_EQ(nfa_hci_cb.app_in_use, p_evt_data->comm.hci_handle);
    EXPECT_TRUE(GKI_queue_is_empty(&nfa_hci_cb.hci_host_reset_api_q));
}

TEST_F(NfaHciCheckPendingApiRequestsTest, ProcessGetRegistryEvent) {
    nfa_hci_cb.hci_state = NFA_HCI_STATE_IDLE;
    NFC_HDR* p_msg = CreateNfcHdr(NFA_HCI_API_GET_REGISTRY_EVT, sizeof(tNFA_HCI_EVENT_DATA));
    tNFA_HCI_EVENT_DATA* p_evt_data = FillEventData(p_msg, 2);
    GKI_enqueue(&nfa_hci_cb.hci_host_reset_api_q, p_msg);
    nfa_hci_check_pending_api_requests();
    EXPECT_EQ(nfa_hci_cb.app_in_use, p_evt_data->comm.hci_handle);
    EXPECT_TRUE(GKI_queue_is_empty(&nfa_hci_cb.hci_host_reset_api_q));
}

TEST_F(NfaHciCheckPendingApiRequestsTest, ProcessSetRegistryEvent) {
    nfa_hci_cb.hci_state = NFA_HCI_STATE_IDLE;
    NFC_HDR* p_msg = CreateNfcHdr(NFA_HCI_API_SET_REGISTRY_EVT, sizeof(tNFA_HCI_EVENT_DATA));
    tNFA_HCI_EVENT_DATA* p_evt_data = FillEventData(p_msg, 3);
    GKI_enqueue(&nfa_hci_cb.hci_host_reset_api_q, p_msg);
    nfa_hci_check_pending_api_requests();
    EXPECT_EQ(nfa_hci_cb.app_in_use, p_evt_data->comm.hci_handle);
    EXPECT_TRUE(GKI_queue_is_empty(&nfa_hci_cb.hci_host_reset_api_q));
}

TEST_F(NfaHciCheckPendingApiRequestsTest, ProcessSendCmdEvent) {
    nfa_hci_cb.hci_state = NFA_HCI_STATE_IDLE;
    NFC_HDR* p_msg = CreateNfcHdr(NFA_HCI_API_SEND_CMD_EVT, sizeof(tNFA_HCI_EVENT_DATA));
    tNFA_HCI_EVENT_DATA* p_evt_data = FillEventData(p_msg, 4);
    GKI_enqueue(&nfa_hci_cb.hci_host_reset_api_q, p_msg);
    nfa_hci_check_pending_api_requests();
    EXPECT_EQ(nfa_hci_cb.app_in_use, p_evt_data->comm.hci_handle);
    EXPECT_TRUE(GKI_queue_is_empty(&nfa_hci_cb.hci_host_reset_api_q));
}

TEST_F(NfaHciCheckPendingApiRequestsTest, ProcessSendEventEvent) {
    nfa_hci_cb.hci_state = NFA_HCI_STATE_IDLE;
    NFC_HDR* p_msg = CreateNfcHdr(NFA_HCI_API_SEND_EVENT_EVT, sizeof(tNFA_HCI_EVENT_DATA));
    tNFA_HCI_EVENT_DATA* p_evt_data = FillEventData(p_msg, 5);
    GKI_enqueue(&nfa_hci_cb.hci_host_reset_api_q, p_msg);
    nfa_hci_check_pending_api_requests();
    EXPECT_EQ(nfa_hci_cb.app_in_use, p_evt_data->comm.hci_handle);
    EXPECT_TRUE(GKI_queue_is_empty(&nfa_hci_cb.hci_host_reset_api_q));
}

class NfaHciCheckApiRequestsTest : public ::testing::Test {
protected:
    void SetUp() override {
        memset(&nfa_hci_cb, 0, sizeof(nfa_hci_cb));
        GKI_create_pool(0, 256, 10, nullptr);
    }
    void TearDown() override {
        memset(&nfa_hci_cb, 0, sizeof(nfa_hci_cb));
    }
    NFC_HDR* CreateNfcHdr(uint16_t event, uint16_t length) {
        NFC_HDR* p_msg = (NFC_HDR*)GKI_getbuf(sizeof(NFC_HDR) + length);
        p_msg->event = event;
        p_msg->len = length;
        return p_msg;
    }
    tNFA_HCI_EVENT_DATA* FillEventData(NFC_HDR* p_msg, uint8_t hci_handle) {
        tNFA_HCI_EVENT_DATA* p_evt_data = (tNFA_HCI_EVENT_DATA*)p_msg;
        p_evt_data->comm.hci_handle = hci_handle;
        return p_evt_data;
    }
};

TEST_F(NfaHciCheckApiRequestsTest, ProcessDeregisterAppEvent) {
    nfa_hci_cb.hci_state = NFA_HCI_STATE_IDLE;
    NFC_HDR* p_msg = CreateNfcHdr(NFA_HCI_API_DEREGISTER_APP_EVT, sizeof(tNFA_HCI_EVENT_DATA));
    tNFA_HCI_EVENT_DATA* p_evt_data = FillEventData(p_msg, 2);
    GKI_enqueue(&nfa_hci_cb.hci_api_q, p_msg);
    nfa_hci_check_api_requests();
    EXPECT_EQ(nfa_hci_cb.app_in_use, p_evt_data->comm.hci_handle);
    EXPECT_TRUE(GKI_queue_is_empty(&nfa_hci_cb.hci_api_q));
}

TEST_F(NfaHciCheckApiRequestsTest, ProcessGetAppGatePipeEvent) {
    nfa_hci_cb.hci_state = NFA_HCI_STATE_IDLE;
    NFC_HDR* p_msg = CreateNfcHdr(NFA_HCI_API_GET_APP_GATE_PIPE_EVT, sizeof(tNFA_HCI_EVENT_DATA));
    tNFA_HCI_EVENT_DATA* p_evt_data = FillEventData(p_msg, 3);
    GKI_enqueue(&nfa_hci_cb.hci_api_q, p_msg);
    nfa_hci_check_api_requests();
    EXPECT_EQ(nfa_hci_cb.app_in_use, p_evt_data->comm.hci_handle);
    EXPECT_TRUE(GKI_queue_is_empty(&nfa_hci_cb.hci_api_q));
}

TEST_F(NfaHciCheckApiRequestsTest, ProcessAllocGateEvent) {
    nfa_hci_cb.hci_state = NFA_HCI_STATE_IDLE;
    NFC_HDR* p_msg = CreateNfcHdr(NFA_HCI_API_ALLOC_GATE_EVT, sizeof(tNFA_HCI_EVENT_DATA));
    tNFA_HCI_EVENT_DATA* p_evt_data = FillEventData(p_msg, 4);
    GKI_enqueue(&nfa_hci_cb.hci_api_q, p_msg);
    nfa_hci_check_api_requests();
    EXPECT_EQ(nfa_hci_cb.app_in_use, p_evt_data->comm.hci_handle);
    EXPECT_TRUE(GKI_queue_is_empty(&nfa_hci_cb.hci_api_q));
}

TEST_F(NfaHciCheckApiRequestsTest, ProcessDeallocGateEvent) {
    nfa_hci_cb.hci_state = NFA_HCI_STATE_IDLE;
    NFC_HDR* p_msg = CreateNfcHdr(NFA_HCI_API_DEALLOC_GATE_EVT, sizeof(tNFA_HCI_EVENT_DATA));
    tNFA_HCI_EVENT_DATA* p_evt_data = FillEventData(p_msg, 5);
    GKI_enqueue(&nfa_hci_cb.hci_api_q, p_msg);
    nfa_hci_check_api_requests();
    EXPECT_EQ(nfa_hci_cb.app_in_use, p_evt_data->comm.hci_handle);
    EXPECT_TRUE(GKI_queue_is_empty(&nfa_hci_cb.hci_api_q));
}

TEST_F(NfaHciCheckApiRequestsTest, ProcessGetHostListEvent) {
    nfa_hci_cb.hci_state = NFA_HCI_STATE_IDLE;
    NFC_HDR* p_msg = CreateNfcHdr(NFA_HCI_API_GET_HOST_LIST_EVT, sizeof(tNFA_HCI_EVENT_DATA));
    tNFA_HCI_EVENT_DATA* p_evt_data = FillEventData(p_msg, 6);
    GKI_enqueue(&nfa_hci_cb.hci_api_q, p_msg);
    nfa_hci_check_api_requests();
    EXPECT_EQ(nfa_hci_cb.app_in_use, p_evt_data->comm.hci_handle);
    EXPECT_TRUE(GKI_queue_is_empty(&nfa_hci_cb.hci_api_q));
}

TEST_F(NfaHciCheckApiRequestsTest, ProcessGetRegistryEventFailure) {
    nfa_hci_cb.hci_state = NFA_HCI_STATE_IDLE;
    NFC_HDR* p_msg = CreateNfcHdr(NFA_HCI_API_GET_REGISTRY_EVT, sizeof(tNFA_HCI_EVENT_DATA));
    tNFA_HCI_EVENT_DATA* p_evt_data = FillEventData(p_msg, 7);
    GKI_enqueue(&nfa_hci_cb.hci_api_q, p_msg);
    bool result = nfa_hci_api_get_reg_value(p_evt_data);
    EXPECT_TRUE(result);
    nfa_hci_check_api_requests();
    EXPECT_EQ(nfa_hci_cb.app_in_use, p_evt_data->comm.hci_handle);
    EXPECT_TRUE(GKI_queue_is_empty(&nfa_hci_cb.hci_api_q));
}

TEST_F(NfaHciCheckApiRequestsTest, ProcessSetRegistryEventFailure) {
    nfa_hci_cb.hci_state = NFA_HCI_STATE_IDLE;
    NFC_HDR* p_msg = CreateNfcHdr(NFA_HCI_API_SET_REGISTRY_EVT, sizeof(tNFA_HCI_EVENT_DATA));
    tNFA_HCI_EVENT_DATA* p_evt_data = FillEventData(p_msg, 8);
    GKI_enqueue(&nfa_hci_cb.hci_api_q, p_msg);
    bool result = nfa_hci_api_set_reg_value(p_evt_data);
    EXPECT_TRUE(result);
    nfa_hci_check_api_requests();
    EXPECT_EQ(nfa_hci_cb.app_in_use, p_evt_data->comm.hci_handle);
    EXPECT_TRUE(GKI_queue_is_empty(&nfa_hci_cb.hci_api_q));
}

TEST_F(NfaHciCheckApiRequestsTest, ProcessCreatePipeEventFailure) {
    nfa_hci_cb.hci_state = NFA_HCI_STATE_IDLE;
    NFC_HDR* p_msg = CreateNfcHdr(NFA_HCI_API_CREATE_PIPE_EVT, sizeof(tNFA_HCI_EVENT_DATA));
    tNFA_HCI_EVENT_DATA* p_evt_data = FillEventData(p_msg, 9);
    GKI_enqueue(&nfa_hci_cb.hci_api_q, p_msg);
    bool result = nfa_hci_api_create_pipe(p_evt_data);
    EXPECT_TRUE(result);
    nfa_hci_check_api_requests();
    EXPECT_EQ(nfa_hci_cb.app_in_use, p_evt_data->comm.hci_handle);
    EXPECT_TRUE(GKI_queue_is_empty(&nfa_hci_cb.hci_api_q));
}

TEST_F(NfaHciCheckApiRequestsTest, ProcessOpenPipeEvent) {
    nfa_hci_cb.hci_state = NFA_HCI_STATE_IDLE;
    NFC_HDR* p_msg = CreateNfcHdr(NFA_HCI_API_OPEN_PIPE_EVT, sizeof(tNFA_HCI_EVENT_DATA));
    tNFA_HCI_EVENT_DATA* p_evt_data = FillEventData(p_msg, 10);
    GKI_enqueue(&nfa_hci_cb.hci_api_q, p_msg);
    nfa_hci_check_api_requests();
    EXPECT_EQ(nfa_hci_cb.app_in_use, p_evt_data->comm.hci_handle);
    EXPECT_TRUE(GKI_queue_is_empty(&nfa_hci_cb.hci_api_q));
}

TEST_F(NfaHciCheckApiRequestsTest, ProcessClosePipeEvent) {
    nfa_hci_cb.hci_state = NFA_HCI_STATE_IDLE;
    NFC_HDR* p_msg = CreateNfcHdr(NFA_HCI_API_CLOSE_PIPE_EVT, sizeof(tNFA_HCI_EVENT_DATA));
    tNFA_HCI_EVENT_DATA* p_evt_data = FillEventData(p_msg, 11);
    GKI_enqueue(&nfa_hci_cb.hci_api_q, p_msg);
    nfa_hci_check_api_requests();
    EXPECT_EQ(nfa_hci_cb.app_in_use, p_evt_data->comm.hci_handle);
    EXPECT_TRUE(GKI_queue_is_empty(&nfa_hci_cb.hci_api_q));
}

TEST_F(NfaHciCheckApiRequestsTest, ProcessDeletePipeEvent) {
    nfa_hci_cb.hci_state = NFA_HCI_STATE_IDLE;
    NFC_HDR* p_msg = CreateNfcHdr(NFA_HCI_API_DELETE_PIPE_EVT, sizeof(tNFA_HCI_EVENT_DATA));
    tNFA_HCI_EVENT_DATA* p_evt_data = FillEventData(p_msg, 12);
    GKI_enqueue(&nfa_hci_cb.hci_api_q, p_msg);
    nfa_hci_check_api_requests();
    EXPECT_EQ(nfa_hci_cb.app_in_use, p_evt_data->comm.hci_handle);
    EXPECT_TRUE(GKI_queue_is_empty(&nfa_hci_cb.hci_api_q));
}

TEST_F(NfaHciCheckApiRequestsTest, ProcessSendRSPEvent) {
    nfa_hci_cb.hci_state = NFA_HCI_STATE_IDLE;
    NFC_HDR* p_msg = CreateNfcHdr(NFA_HCI_API_SEND_RSP_EVT, sizeof(tNFA_HCI_EVENT_DATA));
    tNFA_HCI_EVENT_DATA* p_evt_data = FillEventData(p_msg, 13);
    GKI_enqueue(&nfa_hci_cb.hci_api_q, p_msg);
    nfa_hci_check_api_requests();
    EXPECT_EQ(nfa_hci_cb.app_in_use, p_evt_data->comm.hci_handle);
    EXPECT_TRUE(GKI_queue_is_empty(&nfa_hci_cb.hci_api_q));
}

TEST_F(NfaHciCheckApiRequestsTest, ProcessAddStaticPipeEvent) {
    nfa_hci_cb.hci_state = NFA_HCI_STATE_IDLE;
    NFC_HDR* p_msg = CreateNfcHdr(NFA_HCI_API_ADD_STATIC_PIPE_EVT, sizeof(tNFA_HCI_EVENT_DATA));
    tNFA_HCI_EVENT_DATA* p_evt_data = FillEventData(p_msg, 14);
    GKI_enqueue(&nfa_hci_cb.hci_api_q, p_msg);
    nfa_hci_check_api_requests();
    EXPECT_EQ(nfa_hci_cb.app_in_use, p_evt_data->comm.hci_handle);
    EXPECT_TRUE(GKI_queue_is_empty(&nfa_hci_cb.hci_api_q));
}

class NfaHciApiAddStaticPipeTest : public ::testing::Test {
protected:
    tNFA_HCI_EVENT_DATA p_evt_data;
    tNFA_HCI_EVT_DATA evt_data;
    void SetUp() override {
        p_evt_data.add_static_pipe.hci_handle = 1;
        p_evt_data.add_static_pipe.gate = 2;
        p_evt_data.add_static_pipe.pipe = 3;
        p_evt_data.add_static_pipe.host = 4;
    }
};

TEST_F(NfaHciApiAddStaticPipeTest, AddStaticPipeSuccess) {
    tNFA_HCI_DYN_GATE mock_gate;
    mock_gate.gate_owner = p_evt_data.add_static_pipe.hci_handle;
    tNFA_HCI_DYN_PIPE mock_pipe;
    mock_pipe.pipe_state = NFA_HCI_PIPE_OPENED;
    nfa_hci_api_add_static_pipe(&p_evt_data);
    EXPECT_EQ(mock_pipe.pipe_state, NFA_HCI_PIPE_OPENED);
    EXPECT_EQ(evt_data.pipe_added.status, NFA_STATUS_OK);
}

TEST_F(NfaHciApiAddStaticPipeTest, AddStaticPipeFailedToAddPipe) {
    tNFA_HCI_DYN_GATE mock_gate;
    mock_gate.gate_owner = p_evt_data.add_static_pipe.hci_handle;
    bool pipe_added = false;
    if (!pipe_added) {
        nfa_hciu_release_gate(mock_gate.gate_id);
        evt_data.pipe_added.status = NFA_STATUS_FAILED;
    }
    nfa_hci_api_add_static_pipe(&p_evt_data);
    EXPECT_EQ(evt_data.pipe_added.status, NFA_STATUS_FAILED);
}

TEST_F(NfaHciApiAddStaticPipeTest, AddStaticPipeFailedToAllocateGate) {
    tNFA_HCI_DYN_GATE* pg = nullptr;
    nfa_hci_api_add_static_pipe(&p_evt_data);
    EXPECT_EQ(evt_data.pipe_added.status, NFA_HCI_ANY_OK);
}

TEST_F(NfaHciApiAddStaticPipeTest, AddStaticPipePipeNotFound) {
    tNFA_HCI_DYN_GATE mock_gate;
    mock_gate.gate_owner = p_evt_data.add_static_pipe.hci_handle;
    tNFA_HCI_DYN_PIPE* pp = nullptr;
    nfa_hci_api_add_static_pipe(&p_evt_data);
    EXPECT_EQ(evt_data.pipe_added.status, NFA_HCI_ANY_OK);
}

TEST_F(NfaHciApiAddStaticPipeTest, AddStaticPipePipeStateNotOpened) {
    tNFA_HCI_DYN_GATE mock_gate;
    mock_gate.gate_owner = p_evt_data.add_static_pipe.hci_handle;
    tNFA_HCI_DYN_PIPE mock_pipe;
    mock_pipe.pipe_state = NFA_HCI_PIPE_CLOSED;
    nfa_hci_api_add_static_pipe(&p_evt_data);
    EXPECT_EQ(evt_data.pipe_added.status, NFA_HCI_ANY_OK);
}

TEST_F(NfaHciApiAddStaticPipeTest, AddStaticPipeReleaseGateOnFailure) {
    tNFA_HCI_DYN_GATE mock_gate;
    mock_gate.gate_owner = p_evt_data.add_static_pipe.hci_handle;
    bool pipe_added = false;
    if (!pipe_added) {
        nfa_hciu_release_gate(mock_gate.gate_id);
        evt_data.pipe_added.status = NFA_STATUS_FAILED;
    }
    nfa_hci_api_add_static_pipe(&p_evt_data);
    EXPECT_EQ(evt_data.pipe_added.status, NFA_STATUS_FAILED);
}


class NfaHciApiClosePipeTest : public ::testing::Test {
protected:
    tNFA_HCI_EVT_DATA evt_data;
    tNFA_HCI_EVENT_DATA p_evt_data;
    void SetUp() override {
        p_evt_data.close_pipe.pipe = 0x01;
        p_evt_data.close_pipe.hci_handle = 0x02;
    }
};

TEST_F(NfaHciApiClosePipeTest, ValidPipeAndGate_OpenPipe_HostActive) {
    tNFA_HCI_DYN_PIPE pipe = {0x01, NFA_HCI_PIPE_OPENED, 0x03, 0x04};
    tNFA_HCI_DYN_GATE gate = {0x03, 0x02};
    nfa_hci_cb.cfg.dyn_pipes[0] = pipe;
    nfa_hci_cb.cfg.dyn_gates[0] = gate;
    nfa_hci_api_close_pipe(&p_evt_data);
    EXPECT_EQ(evt_data.closed.status, NFA_STATUS_OK);
}

TEST_F(NfaHciApiClosePipeTest, ValidPipeAndGate_ClosedPipe_HostActive) {
    tNFA_HCI_DYN_PIPE pipe = {0x01, NFA_HCI_PIPE_CLOSED, 0x03, 0x04};
    tNFA_HCI_DYN_GATE gate = {0x03, 0x02};
    nfa_hci_cb.cfg.dyn_pipes[0] = pipe;
    nfa_hci_cb.cfg.dyn_gates[0] = gate;
    nfa_hci_api_close_pipe(&p_evt_data);
    EXPECT_EQ(evt_data.closed.status, NFA_STATUS_OK);
    EXPECT_EQ(evt_data.closed.pipe, 0x00);
}

TEST_F(NfaHciApiClosePipeTest, ValidPipe_InvalidGate) {
    tNFA_HCI_DYN_PIPE pipe = {0x01, NFA_HCI_PIPE_OPENED, 0x03, 0x04};
    tNFA_HCI_DYN_GATE gate = {0x03, 0x02};
    nfa_hci_cb.cfg.dyn_pipes[0] = pipe;
    nfa_hci_api_close_pipe(&p_evt_data);
    EXPECT_EQ(evt_data.closed.status, NFA_STATUS_OK);
    EXPECT_EQ(evt_data.closed.pipe, 0x00);
}

TEST_F(NfaHciApiClosePipeTest, InvalidPipe) {
    nfa_hci_cb.cfg.dyn_pipes[0] = {};
    nfa_hci_api_close_pipe(&p_evt_data);
    EXPECT_EQ(evt_data.closed.status, NFA_STATUS_OK);
    EXPECT_EQ(evt_data.closed.pipe, 0x00);
}

TEST_F(NfaHciApiClosePipeTest, InactiveHost) {
    tNFA_HCI_DYN_PIPE pipe = {0x01, NFA_HCI_PIPE_OPENED, 0x03, 0x05};
    tNFA_HCI_DYN_GATE gate = {0x03, 0x02};
    nfa_hci_cb.cfg.dyn_pipes[0] = pipe;
    nfa_hci_cb.cfg.dyn_gates[0] = gate;
    nfa_hci_api_close_pipe(&p_evt_data);
    EXPECT_EQ(evt_data.closed.status, NFA_STATUS_OK);
    EXPECT_EQ(evt_data.closed.pipe, 0x00);
}

class NfaHciApiCreatePipeTest : public ::testing::Test {
protected:
    tNFA_HCI_EVENT_DATA p_evt_data;
    void SetUp() override {
        p_evt_data.create_pipe.source_gate = 0x01;
        p_evt_data.create_pipe.dest_host = 0x02;
        p_evt_data.create_pipe.dest_gate = 0x03;
        p_evt_data.create_pipe.hci_handle = 0x04;
    }
};

TEST_F(NfaHciApiCreatePipeTest, ValidPipeCreation) {
    tNFA_HCI_DYN_GATE gate = {0x01, 0x04};
    nfa_hci_cb.cfg.dyn_gates[0] = gate;
    nfa_hci_cb.cfg.dyn_pipes[0] = {};
    bool result = nfa_hci_api_create_pipe(&p_evt_data);
    EXPECT_TRUE(result);
    EXPECT_EQ(nfa_hci_cb.local_gate_in_use, 0x01);
    EXPECT_EQ(nfa_hci_cb.remote_gate_in_use, 0x03);
    EXPECT_EQ(nfa_hci_cb.remote_host_in_use, 0x02);
}

TEST_F(NfaHciApiCreatePipeTest, PipeExistsBetweenGates) {
    tNFA_HCI_DYN_GATE gate = {0x01, 0x04};
    nfa_hci_cb.cfg.dyn_gates[0] = gate;
    nfa_hci_cb.cfg.dyn_pipes[0] = {0x01, NFA_HCI_PIPE_OPENED, 0x02, 0x03};
    bool result = nfa_hci_api_create_pipe(&p_evt_data);
    EXPECT_TRUE(result);
}

TEST_F(NfaHciApiCreatePipeTest, HostIsResetting) {
    tNFA_HCI_DYN_GATE gate = {0x01, 0x04};
    nfa_hci_cb.cfg.dyn_gates[0] = gate;
    nfa_hci_cb.cfg.dyn_pipes[0] = {};
    nfa_hci_cb.reset_host[0] = 0xFF;
    bool result = nfa_hci_api_create_pipe(&p_evt_data);
    EXPECT_TRUE(result);
}

TEST_F(NfaHciApiCreatePipeTest, ValidPipeCreationMultipleGatesHosts) {
    tNFA_HCI_DYN_GATE gate1 = {0x01, 0x04};
    tNFA_HCI_DYN_GATE gate2 = {0x03, 0x04};
    nfa_hci_cb.cfg.dyn_gates[0] = gate1;
    nfa_hci_cb.cfg.dyn_gates[1] = gate2;
    nfa_hci_cb.cfg.dyn_pipes[0] = {};
    bool result = nfa_hci_api_create_pipe(&p_evt_data);
    EXPECT_TRUE(result);
    EXPECT_EQ(nfa_hci_cb.local_gate_in_use, 0x01);
    EXPECT_EQ(nfa_hci_cb.remote_gate_in_use, 0x03);
    EXPECT_EQ(nfa_hci_cb.remote_host_in_use, 0x02);
}

class NfaHciApiDeallocGateTest : public ::testing::Test {
protected:
    tNFA_HCI_EVT_DATA evt_data;
    tNFA_HCI_EVENT_DATA p_evt_data;
    void SetUp() override {
        p_evt_data.gate_dealloc.gate = 0x01;
        p_evt_data.gate_dealloc.hci_handle = 0x02;
    }
};

TEST_F(NfaHciApiDeallocGateTest, ValidGate_NoActivePipes) {
    tNFA_HCI_DYN_GATE gate = {0x01, 0x02};
    tNFA_HCI_DYN_PIPE pipe = {0x01, 0x03, 0x04, 0x05};
    nfa_hci_cb.cfg.dyn_gates[0] = gate;
    nfa_hci_api_dealloc_gate(&p_evt_data);
    EXPECT_EQ(evt_data.deallocated.status, NFA_STATUS_OK);
}

TEST_F(NfaHciApiDeallocGateTest, GateOwnedByOtherApp) {
    tNFA_HCI_DYN_GATE gate = {0x01, 0x03};
    nfa_hci_cb.cfg.dyn_gates[0] = gate;
    nfa_hci_api_dealloc_gate(&p_evt_data);
    EXPECT_EQ(evt_data.deallocated.status, NFA_STATUS_OK);
}

TEST_F(NfaHciApiDeallocGateTest, GateHasActivePipes) {
    tNFA_HCI_DYN_GATE gate = {0x01, 0x02};
    tNFA_HCI_DYN_PIPE pipe = {0x01, 0x03, 0x04, 0x05};
    nfa_hci_cb.cfg.dyn_gates[0] = gate;
    nfa_hci_cb.cfg.dyn_pipes[0] = pipe;
    nfa_hci_api_dealloc_gate(&p_evt_data);
    EXPECT_EQ(nfa_hci_cb.hci_state, NFA_HCI_STATE_DISABLED);
}

TEST_F(NfaHciApiDeallocGateTest, HostResetting) {
    tNFA_HCI_DYN_GATE gate = {0x01, 0x02};
    uint8_t resetting_host = 0x03;
    nfa_hci_cb.reset_host[0] = resetting_host;
    nfa_hci_cb.cfg.dyn_gates[0] = gate;
    nfa_hci_api_dealloc_gate(&p_evt_data);
    EXPECT_EQ(evt_data.deallocated.status, NFA_STATUS_OK);
}

TEST_F(NfaHciApiDeallocGateTest, TimerRunning) {
    tNFA_HCI_DYN_GATE gate = {0x01, 0x02};
    nfa_hci_cb.cfg.dyn_gates[0] = gate;
    nfa_hci_cb.timer = {};
    nfa_hci_api_dealloc_gate(&p_evt_data);
    EXPECT_EQ(nfa_hci_cb.timer.ticks, 0);
}

class NfaHciApiDeletePipeTest : public ::testing::Test {
protected:
    tNFA_HCI_EVENT_DATA p_evt_data;
    tNFA_HCI_EVT_DATA evt_data;
    void SetUp() override {
        p_evt_data.delete_pipe.pipe = 0x01;
        p_evt_data.delete_pipe.hci_handle = 0x02;
    }
};

TEST_F(NfaHciApiDeletePipeTest, ValidPipeGateOwnershipHostActive) {
    tNFA_HCI_DYN_PIPE pipe = {0x01, 0x03, 0x04, 0x05};
    tNFA_HCI_DYN_GATE gate = {0x03, 0x02};
    nfa_hci_cb.cfg.dyn_pipes[0] = pipe;
    nfa_hci_cb.cfg.dyn_gates[0] = gate;
    nfa_hci_api_delete_pipe(&p_evt_data);
    EXPECT_EQ(evt_data.deleted.status, NFA_STATUS_OK);
    EXPECT_EQ(evt_data.deleted.pipe, 0x00);
}

class NfaHciHandlePipeOpenCloseCmdTest : public ::testing::Test {
protected:
    tNFA_HCI_DYN_PIPE* p_pipe;
    uint8_t data[1];
    uint8_t rsp_len = 0;
    void SetUp() override {
        memset(&nfa_hci_cb, 0, sizeof(nfa_hci_cb));
    }
    void reset_pipe_state(tNFA_HCI_DYN_PIPE& p_pipe) {
        p_pipe.pipe_state = NFA_HCI_PIPE_CLOSED;}
};

TEST_F(NfaHciHandlePipeOpenCloseCmdTest, OpenPipeGateExists) {
    tNFA_HCI_DYN_PIPE p_pipe = {0x01, NFA_HCI_PIPE_CLOSED, 0x02};
    tNFA_HCI_DYN_GATE gate = {0x02, 2};
    nfa_hci_cb.inst = NFA_HCI_ANY_OPEN_PIPE;
    nfa_hci_cb.cfg.dyn_gates[0] = gate;
    nfa_hci_handle_pipe_open_close_cmd(&p_pipe);
    EXPECT_EQ(p_pipe.pipe_state, NFA_HCI_PIPE_OPENED);
}

TEST_F(NfaHciHandlePipeOpenCloseCmdTest, OpenPipeGateDoesNotExist) {
    tNFA_HCI_DYN_PIPE p_pipe = {0x01, NFA_HCI_PIPE_CLOSED, 0x02};
    nfa_hci_cb.inst = NFA_HCI_ANY_OPEN_PIPE;
    nfa_hci_handle_pipe_open_close_cmd(&p_pipe);
    EXPECT_EQ(p_pipe.pipe_state, NFA_HCI_PIPE_OPENED);
}

TEST_F(NfaHciHandlePipeOpenCloseCmdTest, ClosePipeGateExists) {
    tNFA_HCI_DYN_PIPE p_pipe = {0x01, NFA_HCI_PIPE_OPENED, 0x02};
    tNFA_HCI_DYN_GATE gate = {0x02, 2};
    nfa_hci_cb.inst = NFA_HCI_ANY_CLOSE_PIPE;
    nfa_hci_cb.cfg.dyn_gates[0] = gate;
    nfa_hci_handle_pipe_open_close_cmd(&p_pipe);
    EXPECT_EQ(p_pipe.pipe_state, NFA_HCI_PIPE_CLOSED);
    EXPECT_EQ(rsp_len, 0);
}

TEST_F(NfaHciHandlePipeOpenCloseCmdTest, ClosePipeGateDoesNotExist) {
    tNFA_HCI_DYN_PIPE p_pipe = {0x01, NFA_HCI_PIPE_OPENED, 0x02};
    nfa_hci_cb.inst = NFA_HCI_ANY_CLOSE_PIPE;
    nfa_hci_handle_pipe_open_close_cmd(&p_pipe);
    EXPECT_EQ(p_pipe.pipe_state, NFA_HCI_PIPE_CLOSED);
    EXPECT_EQ(rsp_len, 0);
}

TEST_F(NfaHciHandlePipeOpenCloseCmdTest, PipeStateAfterOpenOperation) {
    tNFA_HCI_DYN_PIPE p_pipe = {0x01, NFA_HCI_PIPE_CLOSED, 0x02};
    tNFA_HCI_DYN_GATE gate = {0x02, 2};
    nfa_hci_cb.inst = NFA_HCI_ANY_OPEN_PIPE;
    nfa_hci_cb.cfg.dyn_gates[0] = gate;
    nfa_hci_handle_pipe_open_close_cmd(&p_pipe);
    EXPECT_EQ(p_pipe.pipe_state, NFA_HCI_PIPE_OPENED);
}

TEST_F(NfaHciHandlePipeOpenCloseCmdTest, PipeStateAfterCloseOperation) {
    tNFA_HCI_DYN_PIPE p_pipe = {0x01, NFA_HCI_PIPE_OPENED, 0x02};
    tNFA_HCI_DYN_GATE gate = {0x02, 2};
    nfa_hci_cb.inst = NFA_HCI_ANY_CLOSE_PIPE;
    nfa_hci_cb.cfg.dyn_gates[0] = gate;
    nfa_hci_handle_pipe_open_close_cmd(&p_pipe);
    EXPECT_EQ(p_pipe.pipe_state, NFA_HCI_PIPE_CLOSED);
}

class NfaHciTest : public::testing::Test{
protected:
    tNFA_HCI_EVT_DATA evt_data;
};

TEST_F(NfaHciTest, HandleOpenPipeCommand) {
    tNFA_HCI_DYN_PIPE pipe;
    pipe.pipe_state = NFA_HCI_PIPE_CLOSED;
    uint8_t data[] = {0};
    uint16_t data_len = sizeof(data);
    nfa_hci_cb.type = NFA_HCI_COMMAND_TYPE;
    nfa_hci_cb.inst = NFA_HCI_ANY_OPEN_PIPE;
    nfa_hci_handle_loopback_gate_pkt(data, data_len, &pipe);
    EXPECT_EQ(pipe.pipe_state, NFA_HCI_PIPE_OPENED);
    EXPECT_EQ(nfa_hci_cb.inst, NFA_HCI_ANY_OPEN_PIPE);
}

TEST_F(NfaHciTest, HandleClosePipeCommand) {
    tNFA_HCI_DYN_PIPE pipe;
    pipe.pipe_state = NFA_HCI_PIPE_OPENED;
    uint8_t data[] = {0};
    uint16_t data_len = sizeof(data);
    nfa_hci_cb.type = NFA_HCI_COMMAND_TYPE;
    nfa_hci_cb.inst = NFA_HCI_ANY_CLOSE_PIPE;
    nfa_hci_handle_loopback_gate_pkt(data, data_len, &pipe);
    EXPECT_EQ(pipe.pipe_state, NFA_HCI_PIPE_CLOSED);
}

TEST_F(NfaHciTest, HandleOpenPipeResponse) {
    tNFA_HCI_DYN_PIPE pipe;
    pipe.pipe_state = NFA_HCI_PIPE_CLOSED;
    uint8_t data[] = {0};
    uint16_t data_len = sizeof(data);
    nfa_hci_cb.type = NFA_HCI_RESPONSE_TYPE;
    nfa_hci_cb.cmd_sent = NFA_HCI_ANY_OPEN_PIPE;
    nfa_hci_cb.inst = NFA_HCI_ANY_OK;
    nfa_hci_handle_loopback_gate_pkt(data, data_len, &pipe);
    EXPECT_EQ(pipe.pipe_state, NFA_HCI_PIPE_OPENED);
}

TEST_F(NfaHciTest, HandleClosePipeResponse) {
    tNFA_HCI_DYN_PIPE pipe;
    pipe.pipe_state = NFA_HCI_PIPE_OPENED;
    uint8_t data[] = {0};
    uint16_t data_len = sizeof(data);
    nfa_hci_cb.type = NFA_HCI_RESPONSE_TYPE;
    nfa_hci_cb.cmd_sent = NFA_HCI_ANY_CLOSE_PIPE;
    nfa_hci_cb.inst = NFA_HCI_ANY_OK;
    nfa_hci_handle_loopback_gate_pkt(data, data_len, &pipe);
    EXPECT_EQ(pipe.pipe_state, NFA_HCI_PIPE_CLOSED);
}

TEST_F(NfaHciTest, HandleEventTypeWhenWaitingForResponseEvent) {
    uint8_t data[] = {0};
    uint16_t data_len = sizeof(data);
    tNFA_HCI_DYN_PIPE pipe;
    nfa_hci_cb.type = NFA_HCI_EVENT_TYPE;
    nfa_hci_cb.w4_rsp_evt = true;
    nfa_hci_cb.inst = NFA_HCI_ANY_OK;
    nfa_hci_handle_loopback_gate_pkt(data, data_len, &pipe);
    EXPECT_EQ(evt_data.rcvd_evt.evt_code, nfa_hci_cb.inst);
}

TEST_F(NfaHciTest, HandlePostDataEvent) {
    tNFA_HCI_DYN_PIPE pipe;
    uint8_t data[] = {0};
    uint16_t data_len = sizeof(data);
    nfa_hci_cb.type = NFA_HCI_EVENT_TYPE;
    nfa_hci_cb.inst = NFA_HCI_EVT_POST_DATA;
    nfa_hci_handle_loopback_gate_pkt(data, data_len, &pipe);
    EXPECT_EQ(nfa_hci_cb.inst, NFA_HCI_EVT_POST_DATA);
}

TEST_F(NfaHciTest, HandleDataLengthExceedsMax) {
    tNFA_HCI_DYN_PIPE pipe;
    uint8_t data[255];
    uint16_t data_len = sizeof(data);
    nfa_hci_cb.type = NFA_HCI_EVENT_TYPE;
    nfa_hci_cb.inst = 0x01;
    nfa_hci_handle_loopback_gate_pkt(data, data_len, &pipe);
    EXPECT_LE(data_len, NFA_MAX_HCI_RSP_LEN);
}

class NfaHciHandleLinkMgmtGateCmdTest : public ::testing::Test {
protected:
    void SetUp() override {
        nfa_hci_cb.cfg.link_mgmt_gate.pipe00_state = NFA_HCI_PIPE_CLOSED;
        nfa_hci_cb.inst = 0;
    }
};

TEST_F(NfaHciHandleLinkMgmtGateCmdTest, HandlePipeNotOpened) {
    uint8_t p_data[] = { 0x01, 0x12, 0x34 };
    uint16_t data_len = sizeof(p_data);
    nfa_hci_cb.inst = NFA_HCI_ANY_SET_PARAMETER;
    nfa_hci_handle_link_mgm_gate_cmd(p_data, data_len);
    EXPECT_EQ(nfa_hci_cb.cfg.link_mgmt_gate.pipe00_state, NFA_HCI_PIPE_CLOSED);
}

TEST_F(NfaHciHandleLinkMgmtGateCmdTest, HandleSetParameterInvalidData) {
    uint8_t p_data[] = { 0x01 };
    uint16_t data_len = sizeof(p_data);
    nfa_hci_cb.inst = NFA_HCI_ANY_SET_PARAMETER;
    nfa_hci_handle_link_mgm_gate_cmd(p_data, data_len);
    EXPECT_EQ(nfa_hci_cb.cfg.link_mgmt_gate.rec_errors, 0);
}

TEST_F(NfaHciHandleLinkMgmtGateCmdTest, HandleGetParameterInvalidData) {
    uint8_t p_data[] = { 0x01 };
    uint16_t data_len = sizeof(p_data);
    nfa_hci_cb.inst = NFA_HCI_ANY_GET_PARAMETER;
    nfa_hci_handle_link_mgm_gate_cmd(p_data, data_len);
    EXPECT_EQ(nfa_hci_cb.cfg.link_mgmt_gate.rec_errors, 0);
}

TEST_F(NfaHciHandleLinkMgmtGateCmdTest, HandleGetParameterValidData) {
    uint8_t p_data[] = { 0x01 };
    uint16_t data_len = sizeof(p_data);
    nfa_hci_cb.cfg.link_mgmt_gate.rec_errors = 0x1234;
    nfa_hci_cb.inst = NFA_HCI_ANY_GET_PARAMETER;
    nfa_hci_handle_link_mgm_gate_cmd(p_data, data_len);
    uint8_t expected_data[] = { 0x12, 0x34 };
    EXPECT_EQ(expected_data[0], 0x12);
    EXPECT_EQ(expected_data[1], 0x34);
}

TEST_F(NfaHciHandleLinkMgmtGateCmdTest, HandleOpenPipe) {
    uint8_t p_data[] = { 0x01 };
    uint16_t data_len = sizeof(p_data);
    nfa_hci_cb.inst = NFA_HCI_ANY_OPEN_PIPE;
    nfa_hci_handle_link_mgm_gate_cmd(p_data, data_len);
    EXPECT_EQ(nfa_hci_cb.cfg.link_mgmt_gate.pipe00_state, NFA_HCI_PIPE_OPENED);
}

TEST_F(NfaHciHandleLinkMgmtGateCmdTest, HandleClosePipe) {
    uint8_t p_data[] = { 0x01 };
    uint16_t data_len = sizeof(p_data);
    nfa_hci_cb.inst = NFA_HCI_ANY_CLOSE_PIPE;
    nfa_hci_handle_link_mgm_gate_cmd(p_data, data_len);
    EXPECT_EQ(nfa_hci_cb.cfg.link_mgmt_gate.pipe00_state, NFA_HCI_PIPE_CLOSED);
}

TEST_F(NfaHciHandleLinkMgmtGateCmdTest, HandleUnsupportedCommand) {
    uint8_t p_data[] = { 0x01 };
    uint16_t data_len = sizeof(p_data);
    nfa_hci_cb.inst = 0xFF;
    nfa_hci_handle_link_mgm_gate_cmd(p_data, data_len);
    EXPECT_EQ(nfa_hci_cb.cfg.link_mgmt_gate.pipe00_state, NFA_HCI_PIPE_CLOSED);
}

class NfaHciHandleGenericGateRspTest : public ::testing::Test {
protected:
    tNFA_HCI_EVT_DATA evt_data;
    void SetUp() override {
        memset(&nfa_hci_cb, 0, sizeof(nfa_hci_cb));
        nfa_hci_cb.app_in_use = 0;
    }
};

TEST_F(NfaHciHandleGenericGateRspTest, HandleGenericGateRsp_InstNotOk) {
    nfa_hci_cb.inst = 1;
    uint8_t data[] = {0x01, 0x02};
    uint8_t data_len = sizeof(data);
    tNFA_HCI_DYN_PIPE pipe = {0};
    nfa_hci_handle_generic_gate_rsp(data, data_len, &pipe);
    ASSERT_EQ(pipe.pipe_state, NFA_HCI_PIPE_CLOSED);
}

TEST_F(NfaHciHandleGenericGateRspTest, HandleGenericGateRsp_OpenPipe) {
    nfa_hci_cb.cmd_sent = NFA_HCI_ANY_OPEN_PIPE;
    uint8_t data[] = {0x01, 0x02};
    uint8_t data_len = sizeof(data);
    tNFA_HCI_DYN_PIPE pipe = {0};
    nfa_hci_handle_generic_gate_rsp(data, data_len, &pipe);
    ASSERT_EQ(pipe.pipe_state, NFA_HCI_PIPE_OPENED);
}

TEST_F(NfaHciHandleGenericGateRspTest, HandleGenericGateRsp_ClosePipe) {
    nfa_hci_cb.cmd_sent = NFA_HCI_ANY_CLOSE_PIPE;
    uint8_t data[] = {0x01, 0x02};
    uint8_t data_len = sizeof(data);
    tNFA_HCI_DYN_PIPE pipe = {0};
    nfa_hci_handle_generic_gate_rsp(data, data_len, &pipe);
    ASSERT_EQ(pipe.pipe_state, NFA_HCI_PIPE_CLOSED);
}

TEST_F(NfaHciHandleGenericGateRspTest, HandleGenericGateRsp_SetParameter) {
    nfa_hci_cb.cmd_sent = NFA_HCI_ANY_SET_PARAMETER;
    uint8_t data[] = {0x01, 0x02};
    uint8_t data_len = sizeof(data);
    tNFA_HCI_DYN_PIPE pipe = {0};
    nfa_hci_handle_generic_gate_rsp(data, data_len, &pipe);
    ASSERT_EQ(evt_data.registry.status, NFA_STATUS_OK);
}

TEST_F(NfaHciHandleGenericGateRspTest, HandleGenericGateRsp_DefaultCase) {
    nfa_hci_cb.cmd_sent = 0;
    nfa_hci_cb.inst = 0;
    uint8_t data[] = {0x01, 0x02};
    uint8_t data_len = sizeof(data);
    tNFA_HCI_DYN_PIPE pipe = {0};
    nfa_hci_handle_generic_gate_rsp(data, data_len, &pipe);
    ASSERT_EQ(evt_data.rsp_rcvd.status, NFA_STATUS_OK);
}

class NfaHciHandleGenericGateEvtTest : public ::testing::Test {
protected:
    tNFA_HCI_EVT_DATA evt_data;
    void SetUp() override {
        memset(&nfa_hci_cb, 0, sizeof(nfa_hci_cb));
        nfa_hci_cb.inst = 1;
    }
};

TEST_F(NfaHciHandleGenericGateEvtTest, HandleGenericGateEvt_NoAssemblyFailure) {
    nfa_hci_cb.assembly_failed = false;
    uint8_t data[] = {0x01, 0x02};
    uint16_t data_len = sizeof(data);
    tNFA_HCI_DYN_GATE gate = {0};
    tNFA_HCI_DYN_PIPE pipe = {100};
    nfa_hci_handle_generic_gate_evt(data, data_len, &gate, &pipe);
    ASSERT_EQ(evt_data.rcvd_evt.status, NFA_STATUS_OK);
    ASSERT_EQ(nfa_hci_cb.rsp_buf_size, 0);
    ASSERT_EQ(nfa_hci_cb.p_rsp_buf, nullptr);
}

TEST_F(NfaHciHandleGenericGateEvtTest, HandleGenericGateEvt_AssemblyFailure) {
    nfa_hci_cb.assembly_failed = true;
    uint8_t data[] = {0x01, 0x02};
    uint16_t data_len = sizeof(data);
    tNFA_HCI_DYN_GATE gate = {0};
    tNFA_HCI_DYN_PIPE pipe = {100};
    nfa_hci_handle_generic_gate_evt(data, data_len, &gate, &pipe);
    ASSERT_EQ(nfa_hci_cb.rsp_buf_size, 0);
    ASSERT_EQ(nfa_hci_cb.p_rsp_buf, nullptr);
}

TEST_F(NfaHciHandleGenericGateEvtTest, HandleGenericGateEvt_EmptyRspBuf) {
    nfa_hci_cb.rsp_buf_size = 0;
    nfa_hci_cb.p_rsp_buf = nullptr;
    uint8_t data[] = {0x01, 0x02};
    uint16_t data_len = sizeof(data);
    tNFA_HCI_DYN_GATE gate = {0};
    tNFA_HCI_DYN_PIPE pipe = {100};
    nfa_hci_handle_generic_gate_evt(data, data_len, &gate, &pipe);
    ASSERT_EQ(nfa_hci_cb.rsp_buf_size, 0);
    ASSERT_EQ(nfa_hci_cb.p_rsp_buf, nullptr);
}

TEST_F(NfaHciHandleGenericGateEvtTest, HandleGenericGateEvt_MaxDataLength) {
    nfa_hci_cb.assembly_failed = false;
    uint8_t data[255];
    std::fill(data, data + sizeof(data), 0xFF);
    uint16_t data_len = sizeof(data);
    tNFA_HCI_DYN_GATE gate = {0};
    tNFA_HCI_DYN_PIPE pipe = {100};
    nfa_hci_handle_generic_gate_evt(data, data_len, &gate, &pipe);
    ASSERT_EQ(nfa_hci_cb.rsp_buf_size, 0);
    ASSERT_EQ(nfa_hci_cb.p_rsp_buf, nullptr);
}

class NfaHciHandleGenericGateCmdTest : public ::testing::Test {
protected:
    void SetUp() override {
        memset(&nfa_hci_cb, 0, sizeof(nfa_hci_cb));
    }
};

TEST_F(NfaHciHandleGenericGateCmdTest, HandleSetParameter) {
    nfa_hci_cb.inst = NFA_HCI_ANY_SET_PARAMETER;
    uint8_t data[] = {0x01, 0x02, 0x03};
    uint8_t data_len = 2;
    tNFA_HCI_DYN_PIPE pipe = {100};
    nfa_hci_handle_generic_gate_cmd(data, data_len, &pipe);
    ASSERT_EQ(nfa_hci_cb.inst, NFA_HCI_ANY_SET_PARAMETER);
}

TEST_F(NfaHciHandleGenericGateCmdTest, HandleGetParameter) {
    nfa_hci_cb.inst = NFA_HCI_ANY_GET_PARAMETER;
    uint8_t data[] = {0x01};
    uint8_t data_len = 1;
    tNFA_HCI_DYN_PIPE pipe = {100};
    nfa_hci_handle_generic_gate_cmd(data, data_len, &pipe);
    ASSERT_EQ(nfa_hci_cb.inst, NFA_HCI_ANY_GET_PARAMETER);
}

TEST_F(NfaHciHandleGenericGateCmdTest, HandleOpenPipe) {
    nfa_hci_cb.inst = NFA_HCI_ANY_OPEN_PIPE;
    uint8_t data[] = {};
    uint8_t data_len = 0;
    tNFA_HCI_DYN_PIPE pipe = {100};
    nfa_hci_handle_generic_gate_cmd(data, data_len, &pipe);
    ASSERT_EQ(nfa_hci_cb.inst, NFA_HCI_ANY_OPEN_PIPE);
}

TEST_F(NfaHciHandleGenericGateCmdTest, HandleClosePipe) {
    nfa_hci_cb.inst = NFA_HCI_ANY_CLOSE_PIPE;
    uint8_t data[] = {};
    uint8_t data_len = 0;
    tNFA_HCI_DYN_PIPE pipe = {100};
    nfa_hci_handle_generic_gate_cmd(data, data_len, &pipe);
    ASSERT_EQ(nfa_hci_cb.inst, NFA_HCI_ANY_CLOSE_PIPE);
}

TEST_F(NfaHciHandleGenericGateCmdTest, HandleDefaultCommand) {
    nfa_hci_cb.inst = 0xFF;
    uint8_t data[] = {0x01, 0x02};
    uint8_t data_len = 2;
    tNFA_HCI_DYN_PIPE pipe = {100};
    nfa_hci_handle_generic_gate_cmd(data, data_len, &pipe);
    ASSERT_EQ(nfa_hci_cb.inst, 0xFF);
}

class NfaHciHandleConnectivityGatePktTest : public ::testing::Test {
protected:
    void SetUp() override {
        nfa_hci_cb.type = NFA_HCI_COMMAND_TYPE;
        nfa_hci_cb.inst = 0;
        nfa_hci_cb.cmd_sent = 0;
        nfa_hci_cb.app_in_use = 1;
        std::memset(&nfa_hci_cb.cfg, 0, sizeof(nfa_hci_cb.cfg));
    }
    bool CheckMessageSent(uint8_t pipe_id, uint8_t response_type, uint16_t cmd_code) {
        std::cout << "Message sent for Pipe ID: " << static_cast<int>(pipe_id)
                  << ", Response Type: " << static_cast<int>(response_type)
                  << ", Cmd Code: " << cmd_code << std::endl;
        return true;
    }
};

TEST_F(NfaHciHandleConnectivityGatePktTest, HandleOpenPipeCommand) {
    uint8_t pipe_id = 1;
    uint8_t data[] = {0x01};
    uint16_t data_len = sizeof(data);
    tNFA_HCI_DYN_PIPE pipe = {pipe_id, 0, NFA_HCI_PIPE_CLOSED};
    nfa_hci_cb.type = NFA_HCI_COMMAND_TYPE;
    nfa_hci_cb.inst = NFA_HCI_ANY_OPEN_PIPE;
    nfa_hci_handle_connectivity_gate_pkt(data, data_len, &pipe);
    EXPECT_EQ(pipe.pipe_state, NFA_HCI_PIPE_OPENED);
    ASSERT_EQ(pipe.pipe_state, NFA_HCI_PIPE_OPENED);
}

TEST_F(NfaHciHandleConnectivityGatePktTest, HandleClosePipeCommand) {
    uint8_t pipe_id = 2;
    uint8_t data[] = {0x01};
    uint16_t data_len = sizeof(data);
    tNFA_HCI_DYN_PIPE pipe = {pipe_id, 0, NFA_HCI_PIPE_OPENED};
    nfa_hci_cb.type = NFA_HCI_COMMAND_TYPE;
    nfa_hci_cb.inst = NFA_HCI_ANY_CLOSE_PIPE;
    nfa_hci_handle_connectivity_gate_pkt(data, data_len, &pipe);
    EXPECT_EQ(pipe.pipe_state, NFA_HCI_PIPE_CLOSED);
    ASSERT_EQ(pipe.pipe_state, NFA_HCI_PIPE_CLOSED);
}

TEST_F(NfaHciHandleConnectivityGatePktTest, HandleUnsupportedCommand) {
    uint8_t pipe_id = 3;
    uint8_t data[] = {0x01};
    uint16_t data_len = sizeof(data);
    tNFA_HCI_DYN_PIPE pipe = {pipe_id, 0, 0};
    nfa_hci_cb.type = NFA_HCI_COMMAND_TYPE;
    nfa_hci_cb.inst = NFA_HCI_CON_PRO_HOST_REQUEST;
    nfa_hci_handle_connectivity_gate_pkt(data, data_len, &pipe);
    ASSERT_TRUE(CheckMessageSent(
            pipe_id, NFA_HCI_RESPONSE_TYPE, NFA_HCI_ANY_E_CMD_NOT_SUPPORTED));
}

TEST_F(NfaHciHandleConnectivityGatePktTest, HandleOpenPipeResponse) {
    uint8_t pipe_id = 4;
    uint8_t data[] = {0x01};
    uint16_t data_len = sizeof(data);
    tNFA_HCI_DYN_PIPE pipe = {pipe_id, 0, NFA_HCI_PIPE_CLOSED};
    nfa_hci_cb.type = NFA_HCI_RESPONSE_TYPE;
    nfa_hci_cb.inst = NFA_HCI_ANY_OK;
    nfa_hci_cb.cmd_sent = NFA_HCI_ANY_OPEN_PIPE;
    nfa_hci_handle_connectivity_gate_pkt(data, data_len, &pipe);
    ASSERT_EQ(pipe.pipe_state, NFA_HCI_PIPE_OPENED);
    ASSERT_TRUE(CheckMessageSent(pipe_id, NFA_HCI_RESPONSE_TYPE, NFA_HCI_ANY_OK));
}

TEST_F(NfaHciHandleConnectivityGatePktTest, HandleClosePipeResponse) {
    uint8_t pipe_id = 5;
    uint8_t data[] = {0x01};
    uint16_t data_len = sizeof(data);
    tNFA_HCI_DYN_PIPE pipe = {pipe_id, 0, NFA_HCI_PIPE_OPENED};
    nfa_hci_cb.type = NFA_HCI_RESPONSE_TYPE;
    nfa_hci_cb.inst = NFA_HCI_ANY_OK;
    nfa_hci_cb.cmd_sent = NFA_HCI_ANY_CLOSE_PIPE;
    nfa_hci_handle_connectivity_gate_pkt(data, data_len, &pipe);
    ASSERT_EQ(pipe.pipe_state, NFA_HCI_PIPE_CLOSED);
    ASSERT_TRUE(CheckMessageSent(pipe_id, NFA_HCI_RESPONSE_TYPE, NFA_HCI_ANY_OK));
}

TEST_F(NfaHciHandleConnectivityGatePktTest, HandleGenericResponse) {
    uint8_t pipe_id = 6;
    uint8_t data[] = {0x01};
    uint16_t data_len = sizeof(data);
    tNFA_HCI_DYN_PIPE pipe = {pipe_id, 0, 0};
    nfa_hci_cb.type = NFA_HCI_RESPONSE_TYPE;
    nfa_hci_cb.inst = NFA_HCI_ANY_OK;
    nfa_hci_cb.cmd_sent = 100;
    nfa_hci_handle_connectivity_gate_pkt(data, data_len, &pipe);
    ASSERT_TRUE(CheckMessageSent(pipe_id, NFA_HCI_RESPONSE_TYPE, 100));
}

TEST_F(NfaHciHandleConnectivityGatePktTest, HandleEventReception) {
    uint8_t pipe_id = 7;
    uint8_t data[] = {0x01};
    uint16_t data_len = sizeof(data);
    tNFA_HCI_DYN_PIPE pipe = {pipe_id, 0, 0};
    nfa_hci_cb.type = NFA_HCI_EVENT_TYPE;
    nfa_hci_cb.inst = 10;
    nfa_hci_handle_connectivity_gate_pkt(data, data_len, &pipe);
    ASSERT_TRUE(CheckMessageSent(pipe_id, NFA_HCI_EVENT_TYPE, 10));
}

class NfaHciHandleAdminGateCmdTest : public ::testing::Test {
protected:
    tNFA_HCI_RESPONSE response = NFA_HCI_ANY_OK;
    void SetUp() override {
        memset(&nfa_hci_cb, 0, sizeof(nfa_hci_cb));
    }
};

TEST_F(NfaHciHandleAdminGateCmdTest, OpenPipe) {
    nfa_hci_cb.inst = NFA_HCI_ANY_OPEN_PIPE;
    uint8_t data = 0;
    uint16_t data_len = 0;
    uint8_t rsp_len = 0;
    uint8_t p_data = 0;
    nfa_hci_handle_admin_gate_cmd(&p_data, data_len);
    EXPECT_EQ(nfa_hci_cb.cfg.admin_gate.pipe01_state, NFA_HCI_PIPE_OPENED);
    EXPECT_EQ(data, 0);
}

TEST_F(NfaHciHandleAdminGateCmdTest, ClosePipe) {
    nfa_hci_cb.inst = NFA_HCI_ANY_CLOSE_PIPE;
    uint8_t data = 0;
    uint16_t data_len = 0;
    uint8_t p_data = 0;
    nfa_hci_handle_admin_gate_cmd(&p_data, data_len);
    EXPECT_EQ(nfa_hci_cb.cfg.admin_gate.pipe01_state, NFA_HCI_PIPE_CLOSED);
    EXPECT_EQ(nfa_hci_cb.app_in_use, NFA_HANDLE_INVALID);
}

TEST_F(NfaHciHandleAdminGateCmdTest, PipeCreated_ValidDataWithStaticGate) {
    nfa_hci_cb.inst = NFA_HCI_ADM_NOTIFY_PIPE_CREATED;
    uint8_t p_data[] = {1, 2, 3, 4, 5};
    uint16_t data_len = sizeof(p_data);
    nfa_hci_handle_admin_gate_cmd(p_data, data_len);
    EXPECT_EQ(response, NFA_HCI_ANY_OK);
}

TEST_F(NfaHciHandleAdminGateCmdTest, PipeCreated_ValidDataWithDynamicGate) {
    nfa_hci_cb.inst = NFA_HCI_ADM_NOTIFY_PIPE_CREATED;
    uint8_t p_data[] = {1, 2, 3, 4, 5};
    uint16_t data_len = sizeof(p_data);
    nfa_hci_handle_admin_gate_cmd(p_data, data_len);
    EXPECT_EQ(response, NFA_HCI_ANY_OK);
}

TEST_F(NfaHciHandleAdminGateCmdTest, PipeDeleted_ValidData) {
    nfa_hci_cb.inst = NFA_HCI_ADM_NOTIFY_PIPE_DELETED;
    uint8_t p_data[] = {5};
    uint16_t data_len = sizeof(p_data);
    nfa_hci_handle_admin_gate_cmd(p_data, data_len);
    EXPECT_EQ(response, NFA_HCI_ANY_OK);
}

TEST_F(NfaHciHandleAdminGateCmdTest, AllPipesCleared_ValidHost) {
    nfa_hci_cb.inst = NFA_HCI_ADM_NOTIFY_ALL_PIPE_CLEARED;
    uint8_t p_data[] = {2};
    uint16_t data_len = sizeof(p_data);
    nfa_hci_handle_admin_gate_cmd(p_data, data_len);
    EXPECT_EQ(nfa_hci_cb.cfg.admin_gate.pipe01_state, NFA_HCI_PIPE_CLOSED);
    EXPECT_EQ(nfa_hci_cb.cfg.link_mgmt_gate.pipe00_state, NFA_HCI_PIPE_CLOSED);
}


TEST_F(NfaHciHandleAdminGateCmdTest, AllPipeCleared_WithoutHost) {
    nfa_hci_cb.inst = NFA_HCI_ADM_NOTIFY_ALL_PIPE_CLEARED;
    uint8_t p_data[] = {0};
    uint16_t data_len = sizeof(p_data);
    nfa_hci_handle_admin_gate_cmd(p_data, data_len);
    EXPECT_EQ(nfa_hci_cb.cfg.admin_gate.pipe01_state, NFA_HCI_PIPE_CLOSED);
    EXPECT_EQ(nfa_hci_cb.cfg.link_mgmt_gate.pipe00_state, NFA_HCI_PIPE_CLOSED);
    EXPECT_EQ(nfa_hci_cb.app_in_use, NFA_HANDLE_INVALID);
}

class NfaHciApiSendEventTest : public ::testing::Test {
protected:
    tNFA_HCI_EVENT_DATA p_evt_data;
    tNFA_HCI_DYN_PIPE* CreatePipe(
            uint8_t pipe_id, tNFA_HCI_PIPE_STATE pipe_state, uint8_t local_gate) {
        static tNFA_HCI_DYN_PIPE pipe;
        pipe.pipe_id = pipe_id;
        pipe.pipe_state = pipe_state;
        pipe.local_gate = local_gate;
        return &pipe;
    }
};

TEST_F(NfaHciApiSendEventTest, PipeNotFound) {
    p_evt_data.send_evt.pipe = 99;
    bool result = nfa_hci_api_send_event(&p_evt_data);
    EXPECT_TRUE(result);
}

TEST_F(NfaHciApiSendEventTest, PipeInactive) {
    uint8_t pipe_id = 1;
    tNFA_HCI_DYN_PIPE* p_pipe = CreatePipe(
            pipe_id, NFA_HCI_PIPE_OPENED, NFA_HCI_CONNECTIVITY_GATE);
    p_evt_data.send_evt.pipe = p_pipe->pipe_id;
    bool result = nfa_hci_api_send_event(&p_evt_data);
    EXPECT_TRUE(result);
}

TEST_F(NfaHciApiSendEventTest, OwnedByDifferentApp) {
    uint8_t pipe_id = 1;
    tNFA_HCI_DYN_PIPE* p_pipe = CreatePipe(
            pipe_id, NFA_HCI_PIPE_OPENED, NFA_HCI_CONNECTIVITY_GATE);
    p_evt_data.send_evt.pipe = p_pipe->pipe_id;
    p_evt_data.send_evt.hci_handle = 2;
    bool result = nfa_hci_api_send_event(&p_evt_data);
    EXPECT_TRUE(result);
}

TEST_F(NfaHciApiSendEventTest, PipeOpenedAndEventSent) {
    uint8_t pipe_id = 1;
    tNFA_HCI_DYN_PIPE* p_pipe = CreatePipe(
            pipe_id, NFA_HCI_PIPE_OPENED, NFA_HCI_CONNECTIVITY_GATE);
    p_evt_data.send_evt.pipe = p_pipe->pipe_id;
    p_evt_data.send_evt.hci_handle = 1;
    p_evt_data.send_evt.evt_code = 0x01;
    bool result = nfa_hci_api_send_event(&p_evt_data);
    EXPECT_TRUE(result);
}

TEST_F(NfaHciApiSendEventTest, NoResponseExpected) {
    uint8_t pipe_id = 1;
    tNFA_HCI_DYN_PIPE* p_pipe = CreatePipe(
            pipe_id, NFA_HCI_PIPE_OPENED, NFA_HCI_CONNECTIVITY_GATE);
    p_evt_data.send_evt.pipe = p_pipe->pipe_id;
    p_evt_data.send_evt.rsp_len = 0;
    bool result = nfa_hci_api_send_event(&p_evt_data);
    EXPECT_TRUE(result);
}

TEST_F(NfaHciApiSendEventTest, LoopbackPipeAndResponseTimeout) {
    uint8_t pipe_id = 1;
    tNFA_HCI_DYN_PIPE* p_pipe = CreatePipe(
            pipe_id, NFA_HCI_PIPE_OPENED, NFA_HCI_LOOP_BACK_GATE);
    p_evt_data.send_evt.pipe = p_pipe->pipe_id;
    p_evt_data.send_evt.rsp_len = 10;
    p_evt_data.send_evt.rsp_timeout = 100;
    bool result = nfa_hci_api_send_event(&p_evt_data);
    EXPECT_TRUE(result);
}

TEST_F(NfaHciApiSendEventTest, PipeNotOpen) {
    uint8_t pipe_id = 1;
    tNFA_HCI_DYN_PIPE* p_pipe = CreatePipe(
            pipe_id, NFA_HCI_PIPE_CLOSED, NFA_HCI_CONNECTIVITY_GATE);
    p_evt_data.send_evt.pipe = p_pipe->pipe_id;
    bool result = nfa_hci_api_send_event(&p_evt_data);
    EXPECT_TRUE(result);
}
