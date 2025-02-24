/*
 * Copyright (C) 2024 The Android Open Source Project
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

import static com.google.common.truth.Truth.assertThat;

import static org.junit.Assert.assertArrayEquals;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;
import static org.mockito.ArgumentMatchers.any;
import static org.mockito.ArgumentMatchers.eq;
import static org.mockito.Mockito.doAnswer;
import static org.mockito.Mockito.times;
import static org.mockito.Mockito.verify;
import static org.mockito.Mockito.when;

import android.os.Parcel;
import android.util.proto.ProtoOutputStream;

import androidx.test.ext.junit.runners.AndroidJUnit4;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.mockito.ArgumentCaptor;
import org.mockito.Mock;
import org.mockito.MockitoAnnotations;

import java.util.List;

@RunWith(AndroidJUnit4.class)
public class NdefMessageTest {
    private NdefMessage mNdefMessage;
    private NdefRecord mNdefRecord;

    @Mock
    private ProtoOutputStream mockProtoOutputStream;

    @Mock
    private NdefRecord mMockRecord1;

    @Mock
    private NdefRecord mMockRecord2;
    @Mock
    private Parcel mMockParcel;

    @Before
    public void setUp() {
        MockitoAnnotations.initMocks(this);
        mNdefRecord = NdefRecord.createUri("http://www.example.com");
        mNdefMessage = new NdefMessage(mNdefRecord);
    }

    @After
    public void tearDown() {
    }

    @Test
    public void testGetRecords() {
        NdefRecord[] records = mNdefMessage.getRecords();
        assertThat(records).isNotNull();
        assertThat(records).hasLength(1);
        assertThat(records[0]).isEqualTo(mNdefRecord);
    }

    @Test
    public void testToByteArray() throws FormatException {
        byte[] bytes = mNdefMessage.toByteArray();
        assertThat(bytes).isNotNull();
        assertThat(bytes.length).isGreaterThan(0);
        NdefMessage ndefMessage = new NdefMessage(bytes);
        assertThat(ndefMessage).isNotNull();
    }

    @Test(expected = NullPointerException.class)
    public void testNdefMessageWithNullRecord() {
        mNdefRecord = null;
        mNdefMessage = new NdefMessage(mNdefRecord);
    }

    @Test(expected = NullPointerException.class)
    public void testNdefMessageWithNullRecords() {
        mNdefMessage = new NdefMessage(mNdefRecord, new NdefRecord[] {null});
    }

    @Test
    public void testDumpDebug_CallsProtoMethodsCorrectly() {
        mNdefMessage = new NdefMessage(new NdefRecord[] {mMockRecord1, mMockRecord2});
        long token1 = 1001L;
        long token2 = 1002L;
        when(mockProtoOutputStream.start(NdefMessageProto.NDEF_RECORDS)).thenReturn(token1, token2);

        mNdefMessage.dumpDebug(mockProtoOutputStream);
        verify(mockProtoOutputStream, times(2)).start(NdefMessageProto.NDEF_RECORDS);
        verify(mMockRecord1).dumpDebug(mockProtoOutputStream);
        verify(mMockRecord2).dumpDebug(mockProtoOutputStream);
        ArgumentCaptor<Long> tokenCaptor = ArgumentCaptor.forClass(Long.class);
        verify(mockProtoOutputStream, times(2)).end(tokenCaptor.capture());
        List<Long> capturedTokens = tokenCaptor.getAllValues();
        assert capturedTokens.contains(token1);
        assert capturedTokens.contains(token2);
    }


    @Test
    public void testWriteToParcel_WritesCorrectData() {
        int flags = 0;
        mNdefMessage = new NdefMessage(new NdefRecord[] {mMockRecord1, mMockRecord2});

        mNdefMessage.writeToParcel(mMockParcel, flags);
        verify(mMockParcel).writeInt(2);
        verify(mMockParcel).writeTypedArray(mNdefMessage.getRecords(), flags);
    }

    @Test
    public void testDescribeContents() {
        assertEquals(0, mNdefMessage.describeContents());
    }

    @Test
    public void testCreateFromParcel() {
        NdefRecord[] testRecords = new NdefRecord[] {mMockRecord1, mMockRecord2};
        when(mMockParcel.readInt()).thenReturn(testRecords.length);
        doAnswer(invocation -> {
            Object[] array = invocation.getArgument(0);
            System.arraycopy(testRecords, 0, array, 0, testRecords.length);
            return null;
        }).when(mMockParcel).readTypedArray(any(NdefRecord[].class), eq(NdefRecord.CREATOR));

        NdefMessage createdMessage = NdefMessage.CREATOR.createFromParcel(mMockParcel);
        assertNotNull(createdMessage);
        assertArrayEquals(testRecords, createdMessage.getRecords());
    }

    @Test
    public void testNewArray() {
        int size = 5;
        NdefMessage[] ndefMessages = NdefMessage.CREATOR.newArray(size);

        assertNotNull(ndefMessages);
        assertEquals(size, ndefMessages.length);
    }
}
