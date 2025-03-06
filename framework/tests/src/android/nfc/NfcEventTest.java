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

package android.nfc;

import static org.junit.Assert.assertEquals;

import org.junit.Before;
import org.junit.Test;
import org.mockito.Mock;
import org.mockito.MockitoAnnotations;

public class NfcEventTest {
    private NfcEvent mNfcEvent;
    private final byte mPeerLlcpVersion = (byte) 1;
    @Mock
    private NfcAdapter mNfcAdapter;

    @Before
    public void setUp() {
        MockitoAnnotations.initMocks(this);
        mNfcEvent = new NfcEvent(mNfcAdapter, mPeerLlcpVersion);
    }

    @Test
    public void testGetNfcAdapter() {
        assertEquals(mNfcAdapter, mNfcEvent.getNfcAdapter());
    }

    @Test
    public void testGetPeerLlcpMajorVersion() {
        assertEquals((mPeerLlcpVersion & 0xF0) >> 4, mNfcEvent.getPeerLlcpMajorVersion());
    }

    @Test
    public void testGetPeerLlcpMinorVersion() {
        assertEquals((mPeerLlcpVersion & 0x0F), mNfcEvent.getPeerLlcpMinorVersion());
    }
}
