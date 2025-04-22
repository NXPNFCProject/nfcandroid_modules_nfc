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
import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertNotNull;
import static org.junit.Assert.assertTrue;

import android.os.Parcel;

import org.junit.Before;
import org.junit.Test;

public class T4tNdefNfceeCcFileInfoTest {

    private final int mCcLength = 0x10;
    private final int mVersion = T4tNdefNfceeCcFileInfo.VERSION_2_0;
    private final int mFileId = 0x1234;
    private final int mMaxSize = 0x100;
    private final boolean mIsReadAllowed = true;
    private final boolean mIsWriteAllowed = false;

    private T4tNdefNfceeCcFileInfo mFileInfo;

    @Before
    public void setUp() {
        mFileInfo = new T4tNdefNfceeCcFileInfo(
                mCcLength, mVersion, mFileId, mMaxSize, mIsReadAllowed, mIsWriteAllowed);
    }

    @Test
    public void testConstructorAndGetters() {
        assertEquals(mCcLength, mFileInfo.getCcFileLength());
        assertEquals(mVersion, mFileInfo.getVersion());
        assertEquals(mFileId, mFileInfo.getFileId());
        assertEquals(mMaxSize, mFileInfo.getMaxSize());
        assertTrue(mFileInfo.isReadAllowed());
        assertFalse(mFileInfo.isWriteAllowed());
    }

    @Test
    public void testParcelableWriteAndRead() {
        mFileInfo = new T4tNdefNfceeCcFileInfo(
                mCcLength, T4tNdefNfceeCcFileInfo.VERSION_3_0, 0x4321, 0x200, mIsReadAllowed,
                mIsWriteAllowed);
        Parcel parcel = Parcel.obtain();
        mFileInfo.writeToParcel(parcel, 0);
        parcel.setDataPosition(0);

        T4tNdefNfceeCcFileInfo createdFromParcel = T4tNdefNfceeCcFileInfo.CREATOR.createFromParcel(
                parcel);
        assertNotNull(createdFromParcel);
        assertEquals(mFileInfo.getCcFileLength(), createdFromParcel.getCcFileLength());
        assertEquals(mFileInfo.getVersion(), createdFromParcel.getVersion());
        assertEquals(mFileInfo.getFileId(), createdFromParcel.getFileId());
        assertEquals(mFileInfo.getMaxSize(), createdFromParcel.getMaxSize());
        assertEquals(mFileInfo.isReadAllowed(), createdFromParcel.isReadAllowed());
        assertEquals(mFileInfo.isWriteAllowed(), createdFromParcel.isWriteAllowed());
        parcel.recycle();
    }

    @Test
    public void testParcelableWithDefaultValues() {
        mFileInfo = new T4tNdefNfceeCcFileInfo(
                0, 0, -1, 0, false, false);
        Parcel parcel = Parcel.obtain();
        mFileInfo.writeToParcel(parcel, 0);
        parcel.setDataPosition(0);

        T4tNdefNfceeCcFileInfo createdFromParcel = T4tNdefNfceeCcFileInfo.CREATOR.createFromParcel(
                parcel);
        assertNotNull(createdFromParcel);
        assertEquals(mFileInfo.getCcFileLength(), createdFromParcel.getCcFileLength());
        assertEquals(mFileInfo.getVersion(), createdFromParcel.getVersion());
        assertEquals(mFileInfo.getFileId(), createdFromParcel.getFileId());
        assertEquals(mFileInfo.getMaxSize(), createdFromParcel.getMaxSize());
        assertEquals(mFileInfo.isReadAllowed(), createdFromParcel.isReadAllowed());
        assertEquals(mFileInfo.isWriteAllowed(), createdFromParcel.isWriteAllowed());
        parcel.recycle();
    }

    @Test
    public void testDescribeContents() {
        int contents = mFileInfo.describeContents();
        assertEquals(0, contents);
    }

    @Test
    public void testParcelableCreatorArray() {
        T4tNdefNfceeCcFileInfo[] array = T4tNdefNfceeCcFileInfo.CREATOR.newArray(5);
        assertNotNull(array);
        assertEquals(5, array.length);
    }
}
