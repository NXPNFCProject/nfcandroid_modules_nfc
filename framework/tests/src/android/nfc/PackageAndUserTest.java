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
import static org.junit.Assert.assertTrue;

import org.junit.Before;
import org.junit.Test;

public class PackageAndUserTest {

    private PackageAndUser mPackageAndUser;
    private final int mUserId = 123454321;
    private final String mPkg = "com.example.nfc";

    @Before
    public void setUp() {
        mPackageAndUser = new PackageAndUser(mUserId, mPkg);
    }

    @Test
    public void testGetUserId() {
        assertEquals(mUserId, mPackageAndUser.getUserId());
    }

    @Test
    public void testGetPackage() {
        assertEquals(mPkg, mPackageAndUser.getPackage());
    }

    @Test
    public void testDescribeContents() {
        assertEquals(0, mPackageAndUser.describeContents());
    }

    @Test
    public void testToString() {
        assertEquals(mPkg + " for user id: " + mUserId, mPackageAndUser.toString());
    }

    @Test
    public void testEquals() {
        PackageAndUser anotherPkgAndUserId = new PackageAndUser(mUserId, mPkg);
        assertTrue(mPackageAndUser.equals(anotherPkgAndUserId));
    }

    @Test
    public void testEqualsWithNullObject() {
        assertFalse(mPackageAndUser.equals(null));
    }

    @Test
    public void testEqualsWithDifferentValues() {
        PackageAndUser diffPkgAndUserId = new PackageAndUser(12121212, "diff.example.nfc");
        assertFalse(mPackageAndUser.equals(diffPkgAndUserId));
    }

    @Test
    public void testHashCode() {
        assertEquals(mPkg.hashCode() + mUserId, mPackageAndUser.hashCode());
    }

    @Test
    public void testHashCodeWithNoPackage() {
        mPackageAndUser = new PackageAndUser(mUserId, null);
        assertEquals(mUserId, mPackageAndUser.hashCode());
    }
}
