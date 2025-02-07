/*
 * Copyright (C) 2015 The Android Open Source Project
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
import static org.junit.Assert.assertNotEquals;

import android.os.Parcel;

import org.junit.Before;
import org.junit.Test;

public class AvailableNfcAntennaTest {
    private final int mLocationX = 10;
    private final int mLocationY = 20;
    private AvailableNfcAntenna mAntenna;

    @Before
    public void setUp() {
        mAntenna = new AvailableNfcAntenna(mLocationX, mLocationY);
    }

    @Test
    public void testConstructorAndGetters() {
        assertEquals(mLocationX, mAntenna.getLocationX());
        assertEquals(mLocationY, mAntenna.getLocationY());
    }

    @Test
    public void testParcelWriteAndRead() {
        int locationX = 15;
        int locationY = 25;
        AvailableNfcAntenna originalAntenna = new AvailableNfcAntenna(locationX, locationY);
        Parcel parcel = Parcel.obtain();
        originalAntenna.writeToParcel(parcel, 0);
        parcel.setDataPosition(0);
        AvailableNfcAntenna createdFromParcel = AvailableNfcAntenna.CREATOR.createFromParcel(
                parcel);

        assertEquals(originalAntenna.getLocationX(), createdFromParcel.getLocationX());
        assertEquals(originalAntenna.getLocationY(), createdFromParcel.getLocationY());
        parcel.recycle();
    }

    @Test
    public void testEqualsAndHashCode() {
        AvailableNfcAntenna antenna = new AvailableNfcAntenna(mLocationX, mLocationY);
        AvailableNfcAntenna nfcAntenna = new AvailableNfcAntenna(15, 25);

        assertEquals(mAntenna, antenna);
        assertNotEquals(mAntenna, nfcAntenna);
        assertEquals(mAntenna.hashCode(), antenna.hashCode());
        assertNotEquals(mAntenna.hashCode(), nfcAntenna.hashCode());
    }

    @Test
    public void testToString() {
        String result = mAntenna.toString();
        assertEquals("AvailableNfcAntenna x: 10 y: 20", result);
    }

    @Test
    public void testDescribeContents() {
        assertEquals(0, mAntenna.describeContents());
    }
}
