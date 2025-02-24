/*
 * Copyright (C) 2025 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package android.nfc.cardemulation;

import static org.junit.Assert.assertArrayEquals;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;
import static org.mockito.ArgumentMatchers.any;
import static org.mockito.ArgumentMatchers.eq;
import static org.mockito.Mockito.never;
import static org.mockito.Mockito.times;
import static org.mockito.Mockito.verify;
import static org.mockito.Mockito.verifyZeroInteractions;
import static org.mockito.Mockito.when;

import android.os.Bundle;
import android.os.Looper;
import android.os.Message;
import android.os.Messenger;
import android.os.RemoteException;

import org.junit.Before;
import org.junit.Test;
import org.mockito.ArgumentCaptor;
import org.mockito.Mock;
import org.mockito.MockitoAnnotations;

public class HostNfcFServiceTest {
    private HostNfcFServiceMsgHandler mhandler;
    @Mock
    private HostNfcFService mHostNfcFService;
    @Mock
    private Messenger mMockNfcService;
    @Mock
    private Messenger mMockReplyMessenger;
    @Mock
    private Message mMockMessage;
    @Mock
    private Bundle mMockBundle;

    @Before
    public void setUp() {
        MockitoAnnotations.initMocks(this);
        if (Looper.myLooper() == null) {
            Looper.prepare();
        }
        mhandler = new HostNfcFServiceMsgHandler(mHostNfcFService);
    }

    @Test
    public void testHandleMessageCommandPacketWithValidData() throws RemoteException {
        byte[] requestData = new byte[] {0x01, 0x02, 0x03};
        byte[] responseData = new byte[] {0x04, 0x05, 0x06};
        mMockMessage.what = HostNfcFService.MSG_COMMAND_PACKET;
        when(mMockMessage.getData()).thenReturn(mMockBundle);
        when(mMockBundle.getByteArray(HostNfcFService.KEY_DATA)).thenReturn(requestData);
        when(mHostNfcFService.getNfcService()).thenReturn(mMockNfcService);
        when(mHostNfcFService.getMessenger()).thenReturn(mMockReplyMessenger);
        when(mHostNfcFService.processNfcFPacket(eq(requestData), any())).thenReturn(
                responseData);

        mhandler.handleMessage(mMockMessage);
        ArgumentCaptor<Message> responseCaptor = ArgumentCaptor.forClass(Message.class);
        verify(mMockNfcService).send(responseCaptor.capture());
        Message responseMessage = responseCaptor.getValue();
        assertNotNull(responseMessage);
        assertEquals(HostNfcFService.MSG_RESPONSE_PACKET, responseMessage.what);
        Bundle responseBundle = responseMessage.getData();
        assertNotNull(responseBundle);
        assertArrayEquals(responseData, responseBundle.getByteArray(HostNfcFService.KEY_DATA));
    }

    @Test
    public void testHandleMessageCommandPacketWithoutData() {
        mMockMessage.what = HostNfcFService.MSG_COMMAND_PACKET;
        when(mMockMessage.getData()).thenReturn(null);

        mhandler.handleMessage(mMockMessage);
        verifyZeroInteractions(mHostNfcFService);
    }

    @Test
    public void testHandleMessageResponsePacket() throws RemoteException {
        mMockMessage.what = HostNfcFService.MSG_RESPONSE_PACKET;
        when(mHostNfcFService.getNfcService()).thenReturn(mMockNfcService);
        when(mHostNfcFService.getMessenger()).thenReturn(mMockReplyMessenger);

        mhandler.handleMessage(mMockMessage);
        verify(mMockNfcService).send(mMockMessage);
        assertEquals(mMockReplyMessenger, mMockMessage.replyTo);
    }

    @Test
    public void testHandleMessageDeactivated() {
        mMockMessage.what = HostNfcFService.MSG_DEACTIVATED;
        mMockMessage.arg1 = 1;

        mhandler.handleMessage(mMockMessage);
        verify(mHostNfcFService).setNfcService(null);
        verify(mHostNfcFService).onDeactivated(1);
    }

    @Test
    public void testHandleMessageCommandPacketServiceDeactivated() {
        byte[] requestData = new byte[]{0x01, 0x02, 0x03};
        byte[] responseData = new byte[]{0x04, 0x05, 0x06};
        mMockMessage.what = HostNfcFService.MSG_COMMAND_PACKET;
        when(mMockMessage.getData()).thenReturn(mMockBundle);
        when(mMockBundle.getByteArray(HostNfcFService.KEY_DATA)).thenReturn(requestData);
        when(mHostNfcFService.getNfcService()).thenReturn(null);
        when(mHostNfcFService.processNfcFPacket(eq(requestData), any())).thenReturn(
                responseData);

        mhandler.handleMessage(mMockMessage);
        verify(mHostNfcFService, never()).getMessenger();
    }
}
