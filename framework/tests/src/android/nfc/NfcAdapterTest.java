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

import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertNotNull;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.times;
import static org.mockito.Mockito.verify;
import static org.mockito.Mockito.when;

import android.app.Activity;
import android.content.Context;
import android.content.pm.PackageManager;
import android.os.IBinder;
import android.os.RemoteException;

import com.android.dx.mockito.inline.extended.ExtendedMockito;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;
import org.mockito.Mock;
import org.mockito.MockitoAnnotations;
import org.mockito.MockitoSession;
import org.mockito.internal.util.reflection.FieldSetter;
import org.mockito.quality.Strictness;

public class NfcAdapterTest {
    @Mock
    private IT4tNdefNfcee mockService;
    @Mock
    private Context mMockContext;
    @Mock
    private PackageManager mPackageManager;
    @Mock
    private NfcServiceManager mNfcServiceManager;
    @Mock
    private NfcServiceManager.ServiceRegisterer mServiceRegisterer;
    @Mock
    private IBinder mIBinder;
    @Mock
    private INfcAdapter mINfcAdapterServices;
    @Mock
    private INfcTag mTag;
    @Mock
    private INfcFCardEmulation mNfcFCardEmulation;
    @Mock
    private INfcCardEmulation mINfcCardEmulation;
    private MockitoSession mMockitoSession;

    @Before
    public void setUp() {
        mMockitoSession = ExtendedMockito.mockitoSession()
                .mockStatic(NfcFrameworkInitializer.class)
                .strictness(Strictness.LENIENT)
                .startMocking();
        MockitoAnnotations.initMocks(this);
    }

    @After
    public void tearDown() {
        mMockitoSession.finishMocking();
    }

    private NfcAdapter createNfcInstance() throws RemoteException {
        when(mMockContext.getPackageManager()).thenReturn(mPackageManager);
        when(mPackageManager.hasSystemFeature(PackageManager.FEATURE_NFC)).thenReturn(true);
        when(mPackageManager.hasSystemFeature(
                PackageManager.FEATURE_NFC_HOST_CARD_EMULATION)).thenReturn(true);
        when(mPackageManager.hasSystemFeature(PackageManager.FEATURE_NFC_CHARGING)).thenReturn(
                true);
        when(NfcFrameworkInitializer.getNfcServiceManager()).thenReturn(mNfcServiceManager);
        when(mNfcServiceManager.getNfcManagerServiceRegisterer()).thenReturn(mServiceRegisterer);
        when(mServiceRegisterer.get()).thenReturn(mIBinder);
        when(INfcAdapter.Stub.asInterface(mIBinder)).thenReturn(mINfcAdapterServices);
        when(mINfcAdapterServices.getNfcTagInterface()).thenReturn(mTag);
        when(mINfcAdapterServices.getNfcFCardEmulationInterface()).thenReturn(mNfcFCardEmulation);
        when(mINfcAdapterServices.getNfcCardEmulationInterface()).thenReturn(mINfcCardEmulation);
        when(mINfcAdapterServices.getT4tNdefNfceeInterface()).thenReturn(mockService);

        return NfcAdapter.getNfcAdapter(mMockContext);
    }

    @Test
    public void testGetNfcAdapterInstance() throws RemoteException {
        NfcAdapter nfcAdapter = createNfcInstance();
        assertNotNull(nfcAdapter);
    }

    @Test(expected = UnsupportedOperationException.class)
    public void testDisableForegroundNdefPush() throws RemoteException {
        NfcAdapter adapter = createNfcInstance();
        try {
            FieldSetter.setField(adapter, adapter.getClass().getDeclaredField("sHasNfcFeature"),
                    false);
        } catch (NoSuchFieldException nsfe) {
            throw new RuntimeException(nsfe);
        }

        adapter.disableForegroundDispatch(mock(Activity.class));
    }

    @Test
    public void testDisableNdefPush() throws RemoteException {
        assertFalse(createNfcInstance().disableNdefPush());
    }

    @Test(expected = NullPointerException.class)
    public void testDispatchWithNullTag() throws RemoteException {
        createNfcInstance().dispatch(null);
    }

    @Test
    public void testDispatch() throws RemoteException {
        NfcAdapter adapter = createNfcInstance();
        try {
            FieldSetter.setField(adapter, adapter.getClass().getDeclaredField("sService"),
                    mINfcAdapterServices);
        } catch (NoSuchFieldException nsfe) {
            throw new RuntimeException(nsfe);
        }
        Tag tag = mock(Tag.class);

        adapter.dispatch(tag);
        verify(mINfcAdapterServices).dispatch(tag);
    }

    @Test(expected = UnsupportedOperationException.class)
    public void testEnableForegroundNdefPush() throws RemoteException {
        NfcAdapter adapter = createNfcInstance();
        try {
            FieldSetter.setField(adapter, adapter.getClass().getDeclaredField("sHasNfcFeature"),
                    false);
        } catch (NoSuchFieldException nsfe) {
            throw new RuntimeException(nsfe);
        }

        adapter.enableForegroundNdefPush(mock(Activity.class), mock(NdefMessage.class));
    }

    @Test
    public void testEnableNdefPush() throws RemoteException {
        assertFalse(createNfcInstance().enableNdefPush());
    }

    @Test(expected = IllegalArgumentException.class)
    public void testGetDefaultAdapterWithNullContext() {
        NfcAdapter.getDefaultAdapter(null);
    }

    @Test(expected = IllegalArgumentException.class)
    public void testGetDefaultAdapterWithNoApplicationContext() {
        when(mMockContext.getApplicationContext()).thenReturn(null);

        NfcAdapter.getDefaultAdapter(mMockContext);
    }

    @Test(expected = UnsupportedOperationException.class)
    public void testGetNfcAdapterExtrasInterfaceWithNullContext() throws RemoteException {
        NfcAdapter nfcAdapter = createNfcInstance();
        try {
            FieldSetter.setField(nfcAdapter, nfcAdapter.getClass().getDeclaredField("mContext"),
                    null);
        } catch (NoSuchFieldException nsfe) {
            throw new RuntimeException(nsfe);
        }

        nfcAdapter.getNfcAdapterExtrasInterface();
    }

    @Test
    public void testGetNfcAdapterExtrasInterface() throws RemoteException {
        NfcAdapter adapter = createNfcInstance();
        try {
            FieldSetter.setField(adapter, adapter.getClass().getDeclaredField("sService"),
                    mINfcAdapterServices);
            FieldSetter.setField(adapter, adapter.getClass().getDeclaredField("mContext"),
                    mMockContext);
        } catch (NoSuchFieldException nsfe) {
            throw new RuntimeException(nsfe);
        }
        when(mMockContext.getPackageName()).thenReturn("android.nfc");

        adapter.getNfcAdapterExtrasInterface();
        verify(mINfcAdapterServices).getNfcAdapterExtrasInterface("android.nfc");
    }

    @Test(expected = UnsupportedOperationException.class)
    public void testGetNfcDtaInterfaceWithNullContext() throws RemoteException {
        NfcAdapter nfcAdapter = createNfcInstance();
        try {
            FieldSetter.setField(nfcAdapter, nfcAdapter.getClass().getDeclaredField("mContext"),
                    null);
        } catch (NoSuchFieldException nsfe) {
            throw new RuntimeException(nsfe);
        }
        nfcAdapter.getNfcDtaInterface();
    }

    @Test
    public void testGetNfcDtaInterface() throws RemoteException {
        NfcAdapter adapter = createNfcInstance();
        try {
            FieldSetter.setField(adapter, adapter.getClass().getDeclaredField("sService"),
                    mINfcAdapterServices);
            FieldSetter.setField(adapter, adapter.getClass().getDeclaredField("mContext"),
                    mMockContext);
        } catch (NoSuchFieldException nsfe) {
            throw new RuntimeException(nsfe);
        }
        when(mMockContext.getPackageName()).thenReturn("android.nfc");

        adapter.getNfcDtaInterface();
        verify(mINfcAdapterServices).getNfcDtaInterface("android.nfc");
    }

    @Test
    public void testGetNfcFCardEmulationService() throws RemoteException {
        NfcAdapter nfcAdapter = createNfcInstance();
        try {
            FieldSetter.setField(nfcAdapter, nfcAdapter.getClass().getDeclaredField("sService"),
                    mINfcAdapterServices);
            FieldSetter.setField(nfcAdapter,
                    nfcAdapter.getClass().getDeclaredField("sHasNfcFeature"),
                    true);
            FieldSetter.setField(nfcAdapter,
                    nfcAdapter.getClass().getDeclaredField("sTagService"),
                    mTag);
            FieldSetter.setField(nfcAdapter,
                    nfcAdapter.getClass().getDeclaredField("sHasCeFeature"),
                    true);
            FieldSetter.setField(nfcAdapter,
                    nfcAdapter.getClass().getDeclaredField("sCardEmulationService"),
                    mINfcCardEmulation);
            FieldSetter.setField(nfcAdapter,
                    nfcAdapter.getClass().getDeclaredField("sNfcFCardEmulationService"),
                    mNfcFCardEmulation);
        } catch (NoSuchFieldException nsfe) {
            throw new RuntimeException(nsfe);
        }
        when(mINfcAdapterServices.getState()).thenReturn(NfcAdapter.STATE_ON);

        nfcAdapter.getNfcFCardEmulationService();
        verify(mINfcAdapterServices, times(4)).getState();
    }
}
