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

import android.os.Parcel;

import org.junit.Test;

import java.io.IOException;

public class TransceiveResultTest {
    private final int mResult = TransceiveResult.RESULT_SUCCESS;
    private final byte[] mData = new byte[] {0x01, 0x02, 0x03};

    @Test
    public void testConstructorAndGetters() throws IOException {
        TransceiveResult transceiveResult = new TransceiveResult(mResult, mData);
        assertArrayEquals(mData, transceiveResult.getResponseOrThrow());
    }

    @Test(expected = IOException.class)
    public void testGetResponseOrThrowFailure() throws IOException {
        TransceiveResult transceiveResult = new TransceiveResult(TransceiveResult.RESULT_FAILURE,
                null);
        transceiveResult.getResponseOrThrow();
    }

    @Test(expected = TagLostException.class)
    public void testGetResponseOrThrowTagLost() throws IOException {
        TransceiveResult transceiveResult = new TransceiveResult(TransceiveResult.RESULT_TAGLOST,
                null);
        transceiveResult.getResponseOrThrow();
    }

    @Test(expected = IOException.class)
    public void testGetResponseOrThrowExceededLength() throws IOException {
        TransceiveResult transceiveResult = new TransceiveResult(
                TransceiveResult.RESULT_EXCEEDED_LENGTH, null);
        transceiveResult.getResponseOrThrow();
    }

    @Test
    public void testParcelableWriteAndReadSuccess() throws IOException {
        TransceiveResult original = new TransceiveResult(mResult, mData);
        Parcel parcel = Parcel.obtain();
        original.writeToParcel(parcel, 0);
        parcel.setDataPosition(0);

        TransceiveResult createdFromParcel = TransceiveResult.CREATOR.createFromParcel(parcel);
        assertArrayEquals(mData, createdFromParcel.getResponseOrThrow());
        parcel.recycle();
    }

    @Test(expected = IOException.class)
    public void testParcelableWriteAndReadFailure() throws IOException {
        int result = TransceiveResult.RESULT_FAILURE;
        TransceiveResult original = new TransceiveResult(result, null);
        Parcel parcel = Parcel.obtain();
        original.writeToParcel(parcel, 0);
        parcel.setDataPosition(0);

        TransceiveResult createdFromParcel = TransceiveResult.CREATOR.createFromParcel(parcel);
        createdFromParcel.getResponseOrThrow();
        parcel.recycle();
    }

    @Test
    public void testDescribeContents() {
        TransceiveResult transceiveResult = new TransceiveResult(TransceiveResult.RESULT_SUCCESS,
                new byte[]{0x01});
        assertEquals(0, transceiveResult.describeContents());
    }

    @Test
    public void testParcelableCreatorArray() {
        TransceiveResult[] array = TransceiveResult.CREATOR.newArray(5);
        assertNotNull(array);
        assertEquals(5, array.length);
    }
}
