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

import static com.android.dx.mockito.inline.extended.ExtendedMockito.doCallRealMethod;
import static com.android.dx.mockito.inline.extended.ExtendedMockito.mock;
import static com.android.dx.mockito.inline.extended.ExtendedMockito.verify;
import static com.android.dx.mockito.inline.extended.ExtendedMockito.when;

import static org.mockito.Mockito.any;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.never;
import static org.mockito.Mockito.times;
import static org.mockito.Mockito.verify;

import android.os.IBinder;
import android.os.RemoteException;

import com.android.dx.mockito.inline.extended.MockedVoidMethod;
import com.android.modules.utils.testing.ExtendedMockitoRule;

import org.junit.Before;
import org.junit.Rule;
import org.junit.Test;
import org.mockito.ArgumentCaptor;
import org.mockito.Mock;
import org.mockito.MockitoSession;
import org.mockito.quality.Strictness;

import java.util.concurrent.Executor;

public class NfcVendorNciCallbackListenerTest {
    @Mock private INfcAdapter mINfcAdapter;
    @Mock private NfcAdapter.NfcVendorNciCallback mCallback;
    @Mock private Executor mExecutor;
    private NfcVendorNciCallbackListener mListener;

    private MockitoSession mStaticMockSession;

    @Rule
    public final ExtendedMockitoRule mExtendedMockitoRule =
            new ExtendedMockitoRule.Builder(this)
                    .mockStatic(NfcAdapter.class)
                    .setStrictness(Strictness.LENIENT)
                    .build();

    @Before
    public void setUp() {
        when(NfcAdapter.getService()).thenReturn(mINfcAdapter);
        when(mINfcAdapter.asBinder()).thenReturn(mock(IBinder.class));
        doCallRealMethod()
                .when(
                        (MockedVoidMethod)
                                () -> NfcAdapter.callService(any(NfcAdapter.ServiceCall.class)));
        mListener = new NfcVendorNciCallbackListener();
    }

    @Test
    public void testRegisterSingleCallback() throws RemoteException {
        mListener.register(mExecutor, mCallback);
        verify(mINfcAdapter).registerVendorExtensionCallback(mListener);
    }

    @Test
    public void testRegisterMultipleCallbacks() throws RemoteException {
        NfcAdapter.NfcVendorNciCallback anotherCallback =
                mock(NfcAdapter.NfcVendorNciCallback.class);
        Executor anotherExecutor = mock(Executor.class);
        mListener.register(mExecutor, mCallback);
        mListener.register(anotherExecutor, anotherCallback);

        verify(mINfcAdapter, times(1)).registerVendorExtensionCallback(mListener);
    }

    @Test
    public void testUnregisterSingleCallback() throws RemoteException {
        mListener.register(mExecutor, mCallback);
        mListener.unregister(mCallback);

        verify(mINfcAdapter).unregisterVendorExtensionCallback(mListener);
    }

    @Test
    public void testUnregisterOneOfMultipleCallbacks() throws RemoteException {
        NfcAdapter.NfcVendorNciCallback anotherCallback =
                mock(NfcAdapter.NfcVendorNciCallback.class);
        Executor anotherExecutor = mock(Executor.class);
        mListener.register(mExecutor, mCallback);
        mListener.register(anotherExecutor, anotherCallback);
        mListener.unregister(mCallback);

        verify(mINfcAdapter, never()).unregisterVendorExtensionCallback(mListener);
    }

    @Test
    public void testOnVendorResponseReceived() throws RemoteException {
        mListener.register(mExecutor, mCallback);
        byte[] payload = new byte[] {0x01, 0x02, 0x03};
        mListener.onVendorResponseReceived(1, 2, payload);

        ArgumentCaptor<Runnable> runnableCaptor = ArgumentCaptor.forClass(Runnable.class);
        verify(mExecutor).execute(runnableCaptor.capture());
        runnableCaptor.getValue().run();
        verify(mCallback).onVendorNciResponse(1, 2, payload);
    }

    @Test
    public void testOnVendorNotificationReceived() throws RemoteException {
        mListener.register(mExecutor, mCallback);
        byte[] payload = new byte[] {0x04, 0x05, 0x06};
        mListener.onVendorNotificationReceived(3, 4, payload);

        ArgumentCaptor<Runnable> runnableCaptor = ArgumentCaptor.forClass(Runnable.class);
        verify(mExecutor).execute(runnableCaptor.capture());
        runnableCaptor.getValue().run();
        verify(mCallback).onVendorNciNotification(3, 4, payload);
    }
}
