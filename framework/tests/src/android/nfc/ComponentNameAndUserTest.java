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

import static org.junit.Assert.*;

import android.content.ComponentName;
import android.os.Parcel;

import org.junit.Before;
import org.junit.Test;

public class ComponentNameAndUserTest {
    private final int mUserId = 1001;
    private ComponentName mComponentName;
    private ComponentNameAndUser mComponentNameAndUser;

    @Before
    public void setUp() {
        mComponentName = new ComponentName("com.example.nfc", "com.example.nfc.MyService");
        mComponentNameAndUser = new ComponentNameAndUser(mUserId, mComponentName);
    }

    @Test
    public void testConstructorAndGetters() {
        assertEquals(mUserId, mComponentNameAndUser.getUserId());
        assertEquals(mComponentName, mComponentNameAndUser.getComponentName());
    }

    @Test
    public void testParcelWriteAndRead() {
        Parcel parcel = Parcel.obtain();
        mComponentNameAndUser.writeToParcel(parcel, 0);
        parcel.setDataPosition(0);
        ComponentNameAndUser createdFromParcel = ComponentNameAndUser.CREATOR.createFromParcel(parcel);

        assertEquals(mComponentNameAndUser.getUserId(), createdFromParcel.getUserId());
        assertEquals(mComponentNameAndUser.getComponentName(), createdFromParcel.getComponentName());
        parcel.recycle();
    }

    @Test
    public void testEqualsAndHashCode() {
        int otherUserId = 1002;
        ComponentName otherComponentName = new ComponentName("com.example.nfc", "com.example.nfc.MyOtherService");
        ComponentNameAndUser otherComponentNameAndUser = new ComponentNameAndUser(otherUserId, otherComponentName);
        ComponentNameAndUser nameAndUser = new ComponentNameAndUser(mUserId, mComponentName);

        assertEquals(mComponentNameAndUser, nameAndUser);
        assertNotEquals(mComponentNameAndUser, otherComponentNameAndUser);
        assertEquals(mComponentNameAndUser.hashCode(), nameAndUser.hashCode());
        assertNotEquals(mComponentNameAndUser.hashCode(), otherComponentNameAndUser.hashCode());
    }

    @Test
    public void testToString() {
        String result = mComponentNameAndUser.toString();
        assertEquals("ComponentInfo{com.example.nfc/com.example.nfc.MyService} for user id: 1001", result);
    }

    @Test
    public void testDescribeContents() {
        assertEquals(0, mComponentNameAndUser.describeContents());
    }
}
