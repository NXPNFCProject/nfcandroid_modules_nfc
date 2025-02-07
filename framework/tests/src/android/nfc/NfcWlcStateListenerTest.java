/*
 * Copyright 2023 The Android Open Source Project
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

import static org.mockito.Mockito.doThrow;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.never;
import static org.mockito.Mockito.times;
import static org.mockito.Mockito.verify;
import static org.mockito.Mockito.verifyNoMoreInteractions;

import android.os.RemoteException;

import org.junit.Before;
import org.junit.Test;
import org.mockito.Mock;
import org.mockito.MockitoAnnotations;

import java.util.concurrent.Executor;

public class NfcWlcStateListenerTest {
    @Mock
    private INfcAdapter mMockAdapter;
    @Mock
    private Executor mMockExecutor;
    @Mock
    private NfcAdapter.WlcStateListener mMockListener;
    private NfcWlcStateListener mNfcWlcStateListener;

    @Before
    public void setUp() {
        MockitoAnnotations.initMocks(this);
        mNfcWlcStateListener = new NfcWlcStateListener(mMockAdapter);
    }

    @Test
    public void testRegisterFirstRegistration() throws Exception {
        mNfcWlcStateListener.register(mMockExecutor, mMockListener);
        verify(mMockAdapter, times(1)).registerWlcStateListener(mNfcWlcStateListener);
    }

    @Test
    public void testRegisterListenerAlreadyRegistered() throws Exception {
        mNfcWlcStateListener.register(mMockExecutor, mMockListener);
        mNfcWlcStateListener.register(mMockExecutor, mMockListener);
        verify(mMockAdapter, times(1)).registerWlcStateListener(mNfcWlcStateListener);
    }

    @Test
    public void testRegisterExceptionDuringAdapterRegistration() throws Exception {
        doThrow(new RemoteException()).when(mMockAdapter).registerWlcStateListener(
                mNfcWlcStateListener);

        mNfcWlcStateListener.register(mMockExecutor, mMockListener);
        verify(mMockAdapter, times(1)).registerWlcStateListener(mNfcWlcStateListener);
        verifyNoMoreInteractions(mMockAdapter);
    }

    @Test
    public void testRegisterMultipleListeners() throws Exception {
        NfcAdapter.WlcStateListener anotherListener = mock(NfcAdapter.WlcStateListener.class);
        Executor anotherExecutor = mock(Executor.class);
        mNfcWlcStateListener.register(mMockExecutor, mMockListener);
        mNfcWlcStateListener.register(anotherExecutor, anotherListener);

        verify(mMockAdapter, times(1)).registerWlcStateListener(mNfcWlcStateListener);
    }

    @Test
    public void testUnregisterListenerNotRegistered() {
        mNfcWlcStateListener.unregister(mMockListener);
        verifyNoMoreInteractions(mMockAdapter);
    }

    @Test
    public void testUnregisterListenerRegistered() throws Exception {
        mNfcWlcStateListener.register(mMockExecutor, mMockListener);

        mNfcWlcStateListener.unregister(mMockListener);
        verify(mMockAdapter, times(1)).unregisterWlcStateListener(mNfcWlcStateListener);
    }

    @Test
    public void testUnregisterExceptionDuringAdapterUnregistration() throws Exception {
        mNfcWlcStateListener.register(mMockExecutor, mMockListener);
        doThrow(new RemoteException()).when(mMockAdapter).unregisterWlcStateListener(
                mNfcWlcStateListener);

        mNfcWlcStateListener.unregister(mMockListener);
        verify(mMockAdapter, times(1)).unregisterWlcStateListener(mNfcWlcStateListener);
    }

    @Test
    public void testUnregisterMultipleListeners() throws Exception {
        NfcAdapter.WlcStateListener anotherListener = mock(NfcAdapter.WlcStateListener.class);
        Executor anotherExecutor = mock(Executor.class);
        mNfcWlcStateListener.register(mMockExecutor, mMockListener);
        mNfcWlcStateListener.register(anotherExecutor, anotherListener);

        mNfcWlcStateListener.unregister(mMockListener);
        verify(mMockAdapter, never()).unregisterWlcStateListener(mNfcWlcStateListener);
        mNfcWlcStateListener.unregister(anotherListener);
        verify(mMockAdapter, times(1)).unregisterWlcStateListener(mNfcWlcStateListener);
    }
}
