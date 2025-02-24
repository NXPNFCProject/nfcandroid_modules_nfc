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

import static org.junit.Assert.assertArrayEquals;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;
import static org.junit.Assert.assertNull;
import static org.mockito.Mockito.mock;

import android.os.Parcel;

import org.junit.Test;
import org.junit.runner.RunWith;
import org.junit.runners.JUnit4;

import java.time.Instant;

@RunWith(JUnit4.class)
public final class OemLogItemsTest {

    private final int mAction = 1;
    private final int mEvent = 2;
    private final int mCallingPid = 1234;
    private final byte[] mCommandApdus = new byte[] {0x01, 0x02, 0x03};
    private final byte[] mResponseApdus = new byte[] {0x0A, 0x0B, 0x0C};
    private final Instant mRfFieldOnTime = Instant.now();
    private Tag mMockTag;

    @Test
    public void testGetAction() {
        OemLogItems item = new OemLogItems.Builder(OemLogItems.LOG_ACTION_RF_FIELD_STATE_CHANGED)
                .build();
        assertEquals(OemLogItems.LOG_ACTION_RF_FIELD_STATE_CHANGED, item.getAction());
    }

    @Test
    public void testGetEvent() {
        OemLogItems item = new OemLogItems.Builder(OemLogItems.LOG_ACTION_NFC_TOGGLE)
                .setCallingEvent(OemLogItems.EVENT_ENABLE)
                .build();
        assertEquals(OemLogItems.EVENT_ENABLE, item.getEvent());
    }

    @Test
    public void testGetCallingPid() {
        OemLogItems item = new OemLogItems.Builder(OemLogItems.LOG_ACTION_NFC_TOGGLE)
                .setCallingPid(1234)
                .build();
        assertEquals(1234, item.getCallingPid());
    }

    @Test
    public void testGetCommandApdu() {
        byte[] commandApdu = {0x01, 0x02, 0x03};
        OemLogItems item = new OemLogItems.Builder(OemLogItems.LOG_ACTION_HCE_DATA)
                .setApduCommand(commandApdu)
                .build();
        assertArrayEquals(commandApdu, item.getCommandApdu());
    }

    @Test
    public void testGetResponseApdu() {
        byte[] responseApdu = {0x04, 0x05, 0x06};
        OemLogItems item = new OemLogItems.Builder(OemLogItems.LOG_ACTION_HCE_DATA)
                .setApduResponse(responseApdu)
                .build();
        assertArrayEquals(responseApdu, item.getResponseApdu());
    }

    @Test
    public void testGetRfFieldEventTimeMillis() {
        Instant expectedTime = Instant.ofEpochSecond(1688768000, 123456789);
        OemLogItems item = new OemLogItems.Builder(OemLogItems.LOG_ACTION_RF_FIELD_STATE_CHANGED)
                .setRfFieldOnTime(expectedTime)
                .build();
        assertEquals(expectedTime, item.getRfFieldEventTimeMillis());
    }

    @Test
    public void testGetTag() {
        Tag mockTag = mock(Tag.class);
        OemLogItems item = new OemLogItems.Builder(OemLogItems.LOG_ACTION_TAG_DETECTED)
                .setTag(mockTag)
                .build();
        assertEquals(mockTag, item.getTag());
    }

    @Test
    public void testConstructorInitialization() {
        mMockTag = mock(Tag.class);
        OemLogItems oemLogItems = new OemLogItems(mAction, mEvent, mCallingPid,
                mCommandApdus, mResponseApdus, mRfFieldOnTime, mMockTag);

        assertEquals(mAction, oemLogItems.getAction());
        assertEquals(mEvent, oemLogItems.getEvent());
        assertEquals(mCallingPid, oemLogItems.getCallingPid());
        assertArrayEquals(mCommandApdus, oemLogItems.getCommandApdu());
        assertArrayEquals(mResponseApdus, oemLogItems.getResponseApdu());
        assertEquals(mRfFieldOnTime, oemLogItems.getRfFieldEventTimeMillis());
        assertEquals(mMockTag, oemLogItems.getTag());
    }

    @Test
    public void testParcelableImplementation() {
        mMockTag = mock(Tag.class);
        OemLogItems original = new OemLogItems(mAction, mEvent, mCallingPid,
                mCommandApdus, mResponseApdus, mRfFieldOnTime, mMockTag);
        Parcel parcel = Parcel.obtain();
        original.writeToParcel(parcel, 0);
        parcel.setDataPosition(0);
        OemLogItems recreated = OemLogItems.CREATOR.createFromParcel(parcel);

        assertEquals(original.getAction(), recreated.getAction());
        assertEquals(original.getEvent(), recreated.getEvent());
        assertEquals(original.getCallingPid(), recreated.getCallingPid());
        assertArrayEquals(original.getCommandApdu(), recreated.getCommandApdu());
        assertArrayEquals(original.getResponseApdu(), recreated.getResponseApdu());
        assertEquals(original.getRfFieldEventTimeMillis(), recreated.getRfFieldEventTimeMillis());
        assertNotNull(recreated.getTag());
        parcel.recycle();
    }

    @Test
    public void testNewArray() {
        int arraySize = 5;
        OemLogItems[] oemLogItemsArray = OemLogItems.CREATOR.newArray(arraySize);

        assertNotNull(oemLogItemsArray);
        assertEquals(arraySize, oemLogItemsArray.length);
    }

    @Test
    public void testParcelableWithNullRfFieldOnTime() {
        mMockTag = mock(Tag.class);
        OemLogItems original = new OemLogItems(mAction, mEvent, mCallingPid,
                mCommandApdus, mResponseApdus, null, mMockTag);
        Parcel parcel = Parcel.obtain();
        original.writeToParcel(parcel, 0);
        parcel.setDataPosition(0);
        OemLogItems recreated = OemLogItems.CREATOR.createFromParcel(parcel);

        assertEquals(original.getAction(), recreated.getAction());
        assertEquals(original.getEvent(), recreated.getEvent());
        assertEquals(original.getCallingPid(), recreated.getCallingPid());
        assertArrayEquals(original.getCommandApdu(), recreated.getCommandApdu());
        assertArrayEquals(original.getResponseApdu(), recreated.getResponseApdu());
        assertNull(recreated.getRfFieldEventTimeMillis());
        assertNotNull(recreated.getTag());
        parcel.recycle();
    }

    @Test
    public void testToString() {
        mMockTag = mock(Tag.class);
        OemLogItems oemLogItems = new OemLogItems(mAction, mEvent, mCallingPid,
                mCommandApdus, mResponseApdus, mRfFieldOnTime, mMockTag);
        String expectedString = "[mCommandApdus: " + byteToHex(mCommandApdus)
                + "[mResponseApdus: " + byteToHex(mResponseApdus)
                + ", mCallingApi= " + mEvent
                + ", mAction= " + mAction
                + ", mCallingPId = " + mCallingPid
                + ", mRfFieldOnTime= " + mRfFieldOnTime;

        assertEquals(expectedString, oemLogItems.toString());
    }

    @Test
    public void testWriteToParcel() {
        mMockTag = mock(Tag.class);
        OemLogItems original = new OemLogItems(mAction, mEvent, mCallingPid,
                mCommandApdus, mResponseApdus, mRfFieldOnTime, mMockTag);
        Parcel parcel = Parcel.obtain();
        original.writeToParcel(parcel, 0);
        parcel.setDataPosition(0);

        assertEquals(mAction, parcel.readInt());
        assertEquals(mEvent, parcel.readInt());
        assertEquals(mCallingPid, parcel.readInt());

        int commandApdusLength = parcel.readInt();
        byte[] readCommandApdus = new byte[commandApdusLength];
        parcel.readByteArray(readCommandApdus);
        assertArrayEquals(mCommandApdus, readCommandApdus);

        int responseApdusLength = parcel.readInt();
        byte[] readResponseApdus = new byte[responseApdusLength];
        parcel.readByteArray(readResponseApdus);
        assertArrayEquals(mResponseApdus, readResponseApdus);

        boolean isRfFieldOnTimeSet = parcel.readBoolean();
        if (isRfFieldOnTimeSet) {
            long epochSecond = parcel.readLong();
            int nano = parcel.readInt();
            Instant readRfFieldOnTime = Instant.ofEpochSecond(epochSecond, nano);
            assertEquals(mRfFieldOnTime, readRfFieldOnTime);
        } else {
            assertNull(original.getRfFieldEventTimeMillis());
        }

        assertNotNull(parcel.readParcelable(Tag.class.getClassLoader()));
        parcel.recycle();
    }

    private String byteToHex(byte[] bytes) {
        char[] HexArray = "0123456789ABCDEF".toCharArray();
        char[] hexChars = new char[bytes.length * 2];
        for (int j = 0; j < bytes.length; j++) {
            int v = bytes[j] & 0xFF;
            hexChars[j * 2] = HexArray[v >>> 4];
            hexChars[j * 2 + 1] = HexArray[v & 0x0F];
        }
        return new String(hexChars);
    }
}
