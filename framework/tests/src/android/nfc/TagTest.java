/*
 * Copyright (C) 2010 The Android Open Source Project
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

import static android.nfc.tech.TagTechnology.ISO_DEP;
import static android.nfc.tech.TagTechnology.MIFARE_CLASSIC;
import static android.nfc.tech.TagTechnology.MIFARE_ULTRALIGHT;
import static android.nfc.tech.TagTechnology.NDEF;
import static android.nfc.tech.TagTechnology.NDEF_FORMATABLE;
import static android.nfc.tech.TagTechnology.NFC_A;
import static android.nfc.tech.TagTechnology.NFC_B;
import static android.nfc.tech.TagTechnology.NFC_BARCODE;
import static android.nfc.tech.TagTechnology.NFC_F;
import static android.nfc.tech.TagTechnology.NFC_V;

import static org.junit.Assert.assertArrayEquals;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertNotNull;
import static org.junit.Assert.assertNull;
import static org.junit.Assert.assertThrows;
import static org.junit.Assert.assertTrue;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.when;

import android.nfc.tech.IsoDep;
import android.nfc.tech.MifareClassic;
import android.nfc.tech.MifareUltralight;
import android.nfc.tech.Ndef;
import android.nfc.tech.NdefFormatable;
import android.nfc.tech.NfcA;
import android.nfc.tech.NfcB;
import android.nfc.tech.NfcBarcode;
import android.nfc.tech.NfcF;
import android.nfc.tech.NfcV;
import android.os.Bundle;
import android.os.RemoteException;

import org.junit.Before;
import org.junit.Test;
import org.mockito.Mock;
import org.mockito.MockitoAnnotations;

import java.io.IOException;

public class TagTest {
    private final byte[] id = "id".getBytes();
    @Mock
    private INfcTag mTagService;
    @Mock
    private Bundle mBundle;
    private Tag mTag;

    @Before
    public void setUp() {
        MockitoAnnotations.initMocks(this);
        mTag = createInstance(NFC_A, mTagService, null, null);
    }

    private Tag createInstance(int tech, INfcTag tagService, int[] techList,
            Bundle[] techListExtras) {
        techList = techList == null ? new int[]{tech} : techList;
        techListExtras = techListExtras == null ? new Bundle[]{mBundle} : techListExtras;
        int serviceHandle = 1;
        long cookie = (long) 1;
        return new Tag(id, techList, techListExtras, serviceHandle, cookie, tagService);
    }

    @Test
    public void testTagInstance() {
        assertTrue(mTag.hasTech(NFC_A));
        assertEquals(1, mTag.getServiceHandle());
        assertArrayEquals(new int[]{NFC_A}, mTag.getTechCodeList());
        assertEquals(id, mTag.getId());
        assertArrayEquals(new String[]{NfcA.class.getName()}, mTag.getTechList());
    }

    @Test
    public void testTagInstanceWithTechNfcB() {
        mTag = createInstance(NFC_B, mTagService, null, null);

        assertArrayEquals(new int[]{NFC_B}, mTag.getTechCodeList());
        assertEquals(id, mTag.getId());
        assertArrayEquals(new String[]{NfcB.class.getName()}, mTag.getTechList());
    }

    @Test
    public void testTagInstanceWithTechNfcF() {
        mTag = createInstance(NFC_F, mTagService, null, null);

        assertArrayEquals(new int[]{NFC_F}, mTag.getTechCodeList());
        assertEquals(id, mTag.getId());
        assertArrayEquals(new String[]{NfcF.class.getName()}, mTag.getTechList());
    }

    @Test
    public void testTagInstanceWithTechNfcV() {
        mTag = createInstance(NFC_V, mTagService, null, null);

        assertArrayEquals(new int[]{NFC_V}, mTag.getTechCodeList());
        assertEquals(id, mTag.getId());
        assertArrayEquals(new String[]{NfcV.class.getName()}, mTag.getTechList());
    }

    @Test
    public void testTagInstanceWithTechNfcBarcode() {
        mTag = createInstance(NFC_BARCODE, mTagService, null, null);

        assertArrayEquals(new int[]{NFC_BARCODE}, mTag.getTechCodeList());
        assertEquals(id, mTag.getId());
        assertArrayEquals(new String[]{NfcBarcode.class.getName()}, mTag.getTechList());
    }

    @Test
    public void testTagInstanceWithTechIsoDep() {
        mTag = createInstance(ISO_DEP, mTagService, null, null);

        assertArrayEquals(new int[]{ISO_DEP}, mTag.getTechCodeList());
        assertEquals(id, mTag.getId());
        assertArrayEquals(new String[]{IsoDep.class.getName()}, mTag.getTechList());
    }

    @Test
    public void testTagInstanceWithTechMifareClassic() {
        mTag = createInstance(MIFARE_CLASSIC, mTagService, null, null);

        assertArrayEquals(new int[]{MIFARE_CLASSIC}, mTag.getTechCodeList());
        assertEquals(id, mTag.getId());
        assertArrayEquals(new String[]{MifareClassic.class.getName()}, mTag.getTechList());
    }

    @Test
    public void testTagInstanceWithTechMifareUltralight() {
        mTag = createInstance(MIFARE_ULTRALIGHT, mTagService, null, null);

        assertArrayEquals(new int[]{MIFARE_ULTRALIGHT}, mTag.getTechCodeList());
        assertEquals(id, mTag.getId());
        assertArrayEquals(new String[]{MifareUltralight.class.getName()}, mTag.getTechList());
    }

    @Test
    public void testTagInstanceWithTechNdef() {
        mTag = createInstance(NDEF, mTagService, null, null);

        assertArrayEquals(new int[]{NDEF}, mTag.getTechCodeList());
        assertEquals(id, mTag.getId());
        assertArrayEquals(new String[]{Ndef.class.getName()}, mTag.getTechList());
    }

    @Test
    public void testTagInstanceWithTechNdefFormatable() {
        mTag = createInstance(NDEF_FORMATABLE, mTagService, null, null);

        assertArrayEquals(new int[]{NDEF_FORMATABLE}, mTag.getTechCodeList());
        assertEquals(id, mTag.getId());
        assertArrayEquals(new String[]{NdefFormatable.class.getName()}, mTag.getTechList());
    }

    @Test(expected = IllegalArgumentException.class)
    public void testTagInstanceWithNoTech() {
        mTag = createInstance(-1, mTagService, null, null);
    }

    @Test(expected = IllegalStateException.class)
    public void testRediscoverWithNoConnectedTechnology() throws IOException {
        mTag.setConnectedTechnology(NFC_A);
        mTag.rediscover();
    }

    @Test(expected = IOException.class)
    public void testRediscoverWithNoTagServices() throws IOException {
        mTag = createInstance(NFC_A, null, null, null);
        mTag.rediscover();
    }

    @Test
    public void testRediscover() throws RemoteException, IOException {
        Tag mockTag = mock(Tag.class);
        when(mTagService.rediscover(1)).thenReturn(mockTag);

        assertEquals(mockTag, mTag.rediscover());
    }

    @Test(expected = IOException.class)
    public void testNullRediscoverWhenINfcTagRediscoveryIsNull()
            throws RemoteException, IOException {
        when(mTagService.rediscover(1)).thenReturn(null);
        mTag.rediscover();
    }

    @Test
    public void testHasTechNegativeScenario() {
        assertFalse(mTag.hasTech(NFC_B));
    }

    @Test
    public void testGetTechExtrasReturnsCorrectBundle() {
        int[] techList = new int[]{1, 2, 3, 4};
        Bundle[] techExtras = new Bundle[techList.length];
        for (int i = 0; i < techExtras.length; i++) {
            techExtras[i] = new Bundle();
            techExtras[i].putString("key" + i, "value" + i);
        }
        mTag = createInstance(NFC_A, mTagService, techList, techExtras);

        Bundle result = mTag.getTechExtras(2);
        assertNotNull(result);
        assertEquals("value1", result.getString("key1"));
    }

    @Test
    public void testGetTechExtrasReturnsNullWhenTechNotFound() {
        Bundle result = mTag.getTechExtras(99);
        assertNull(result);
    }

    @Test
    public void testGetTagServiceReturnsServiceWhenTagIsUpToDate() throws Exception {
        when(mTagService.isTagUpToDate(1)).thenReturn(true);

        INfcTag result = mTag.getTagService();
        assertNotNull(result);
        assertEquals(mTagService, result);
    }

    @Test(expected = SecurityException.class)
    public void testGetTagServiceThrowsSecurityExceptionWhenTagIsOutOfDate() throws Exception {
        when(mTagService.isTagUpToDate(1)).thenReturn(false);

        mTag.getTagService();
    }

    @Test
    public void testGetTagServiceReturnsNullWhenTagServiceIsNull() {
        mTag = createInstance(NFC_A, null, null, null);
        INfcTag result = mTag.getTagService();
        assertNull(result);
    }

    @Test
    public void testGetTagServiceThrowsRuntimeExceptionWhenRemoteExceptionOccurs()
            throws Exception {
        when(mTagService.isTagUpToDate(1)).thenThrow(new RemoteException());

        RuntimeException exception = assertThrows(RuntimeException.class, () -> {
            mTag.getTagService();
        });
        assertNotNull(exception);
    }

    @Test
    public void testToStringReturnsExpectedFormat() {
        String expectedOutput = "TAG: Tech [" + NfcA.class.getName() + "]";

        String result = mTag.toString();
        assertEquals(expectedOutput, result);
    }

    @Test
    public void testSetConnectedTechnologySuccess() {
        boolean result = mTag.setConnectedTechnology(5);
        assertTrue(result);
        assertEquals(5, mTag.getConnectedTechnology());
    }

    @Test
    public void testSetConnectedTechnology_FailureWhenAlreadySet() {
        mTag.setConnectedTechnology(5);

        boolean result = mTag.setConnectedTechnology(10);
        assertFalse(result);
        assertEquals(5, mTag.getConnectedTechnology());
    }

    @Test
    public void testGetConnectedTechnologyDefaultValue() {
        int connectedTechnology = mTag.getConnectedTechnology();
        assertEquals(-1, connectedTechnology);
    }

    @Test
    public void testSetTechnologyDisconnected() {
        mTag.setConnectedTechnology(5);

        mTag.setTechnologyDisconnected();
        assertEquals(-1, mTag.getConnectedTechnology());
    }
}
