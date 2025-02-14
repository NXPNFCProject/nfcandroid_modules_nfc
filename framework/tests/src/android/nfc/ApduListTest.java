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

import static org.junit.Assert.assertArrayEquals;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;
import static org.junit.Assert.assertTrue;

import android.os.Parcel;

import org.junit.Before;
import org.junit.Test;

import java.util.List;

public class ApduListTest {
    private final byte[] command1 = new byte[] {0x00, (byte) 0xA4, 0x04, 0x00};
    private final byte[] command2 = new byte[] {(byte) 0x80, (byte) 0xCA, (byte) 0x9F, 0x7F};

    private ApduList apduList;

    @Before
    public void setUp() {
        apduList = new ApduList();
    }

    @Test
    public void testAddAndGet() {
        apduList.add(command1);
        apduList.add(command2);
        List<byte[]> commands = apduList.get();

        assertEquals(2, commands.size());
        assertArrayEquals(command1, commands.get(0));
        assertArrayEquals(command2, commands.get(1));
    }

    @Test
    public void testParcelWriteAndRead() {
        apduList.add(command1);
        apduList.add(command2);
        Parcel parcel = Parcel.obtain();
        apduList.writeToParcel(parcel, 0);
        parcel.setDataPosition(0);

        ApduList createdFromParcel = ApduList.CREATOR.createFromParcel(parcel);
        List<byte[]> commands = createdFromParcel.get();
        assertEquals(2, commands.size());
        assertArrayEquals(command1, commands.get(0));
        assertArrayEquals(command2, commands.get(1));
        parcel.recycle();
    }

    @Test
    public void testEmptyApduList() {
        ApduList emptyList = new ApduList();
        List<byte[]> commands = emptyList.get();

        assertNotNull(commands);
        assertTrue(commands.isEmpty());
    }

    @Test
    public void testDescribeContents() {
        assertEquals(0, apduList.describeContents());
    }
}
