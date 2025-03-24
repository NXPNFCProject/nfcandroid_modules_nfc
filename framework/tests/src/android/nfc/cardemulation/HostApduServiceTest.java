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

import static org.mockito.Mockito.CALLS_REAL_METHODS;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.verify;
import static org.mockito.Mockito.when;

import android.os.Bundle;
import android.os.Looper;
import android.os.Message;
import android.os.Messenger;

import org.junit.Before;
import org.junit.Test;
import org.mockito.MockitoAnnotations;

public class HostApduServiceTest {
    private byte[] sampleApdu = "EO00001800".getBytes();
    private HostApduService.MsgHandler mHandler;
    private SampleHostApduService mSampleService;

    class SampleHostApduService extends HostApduService {

        @Override
        public byte[] processCommandApdu(byte[] commandApdu, Bundle extras) {
            return new byte[0];
        }

        @Override
        public void onDeactivated(int reason) {

        }
    }

    @Before
    public void setUp() {
        MockitoAnnotations.initMocks(this);
        if (Looper.myLooper() == null) {
            Looper.prepare();
        }
        mSampleService = mock(SampleHostApduService.class, CALLS_REAL_METHODS);
        mHandler = mSampleService.getMsgHandler();
    }

    @Test
    public void testHandleMessageWithCmdApdu() {
        Bundle bundle = mock(Bundle.class);
        Message msg = mock(Message.class);
        Messenger mNfcService = mock(Messenger.class);
        msg.what = HostApduService.MSG_COMMAND_APDU;
        msg.replyTo = mNfcService;
        when(msg.getData()).thenReturn(bundle);
        when(bundle.getByteArray(HostApduService.KEY_DATA)).thenReturn(sampleApdu);

        mHandler.post(() -> {
            mHandler.handleMessage(msg);
            verify(msg).getData();
        });
    }

}
