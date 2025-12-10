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
import static org.junit.Assert.assertNotNull;

import android.os.Parcel;

import org.junit.Before;
import org.junit.Test;

public class WlcListenerDeviceInfoTest {
    private final int mProductId = 12345;
    private final double mTemperature = 35.5;
    private final double mBatteryLevel = 75.0;
    private final int mState = WlcListenerDeviceInfo.STATE_CONNECTED_CHARGING;
    private WlcListenerDeviceInfo mWlcListenerDeviceInfo;

    @Before
    public void setUp() {
        mWlcListenerDeviceInfo = new WlcListenerDeviceInfo(mProductId, mTemperature, mBatteryLevel,
                mState);
    }

    @Test
    public void testConstructorAndGetters() {
        assertEquals(mProductId, mWlcListenerDeviceInfo.getProductId());
        assertEquals(mTemperature, mWlcListenerDeviceInfo.getTemperature(), 0.001);
        assertEquals(mBatteryLevel, mWlcListenerDeviceInfo.getBatteryLevel(), 0.001);
        assertEquals(mState, mWlcListenerDeviceInfo.getState());
    }

    @Test
    public void testParcelableWriteAndRead() {
        Parcel parcel = Parcel.obtain();
        mWlcListenerDeviceInfo.writeToParcel(parcel, 0);
        parcel.setDataPosition(0);

        WlcListenerDeviceInfo createdFromParcel = WlcListenerDeviceInfo.CREATOR.createFromParcel(
                parcel);
        assertEquals(mWlcListenerDeviceInfo.getProductId(), createdFromParcel.getProductId());
        assertEquals(mWlcListenerDeviceInfo.getTemperature(), createdFromParcel.getTemperature(),
                0.001);
        assertEquals(mWlcListenerDeviceInfo.getBatteryLevel(), createdFromParcel.getBatteryLevel(),
                0.001);
        assertEquals(mWlcListenerDeviceInfo.getState(), createdFromParcel.getState());
        parcel.recycle();
    }

    @Test
    public void testDescribeContents() {
        WlcListenerDeviceInfo info = new WlcListenerDeviceInfo(12345, 35.5, 75.0,
                WlcListenerDeviceInfo.STATE_DISCONNECTED);
        assertEquals(0, info.describeContents());
    }

    @Test
    public void testParcelableCreatorArray() {
        WlcListenerDeviceInfo[] array = WlcListenerDeviceInfo.CREATOR.newArray(5);
        assertNotNull(array);
        assertEquals(5, array.length);
    }

    @Test
    public void testStateAnnotations() {
        int validState = WlcListenerDeviceInfo.STATE_CONNECTED_CHARGING;
        WlcListenerDeviceInfo info = new WlcListenerDeviceInfo(12345, 35.5, 75.0, validState);
        assertEquals(validState, info.getState());
    }
}
