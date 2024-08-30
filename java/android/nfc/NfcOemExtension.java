/*
 * Copyright (C) 2024 The Android Open Source Project
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

import android.annotation.CallbackExecutor;
import android.annotation.FlaggedApi;
import android.annotation.IntDef;
import android.annotation.NonNull;
import android.annotation.RequiresPermission;
import android.annotation.SystemApi;
import android.content.Context;
import android.os.Binder;
import android.os.RemoteException;
import android.util.Log;

import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.concurrent.Executor;

/**
 * Used for OEM extension APIs.
 * This class holds all the APIs and callbacks defined for OEMs/vendors to extend the NFC stack
 * for their proprietary features.
 *
 * @hide
 */
@FlaggedApi(Flags.FLAG_NFC_OEM_EXTENSION)
@SystemApi
public final class NfcOemExtension {
    private static final String TAG = "NfcOemExtension";
    private static final int OEM_EXTENSION_RESPONSE_THRESHOLD_MS = 2000;
    private final NfcAdapter mAdapter;
    private final NfcOemExtensionCallback mOemNfcExtensionCallback;
    private boolean mIsRegistered = false;
    private final Map<Callback, Executor> mCallbackMap = new HashMap<>();
    private final Context mContext;
    private final Object mLock = new Object();
    private boolean mCardEmulationActivated = false;
    private boolean mRfFieldActivated = false;
    private boolean mRfDiscoveryStarted = false;

    /**
     * Mode Type for {@link #setControllerAlwaysOn(int)}.
     * works same as {@link NfcAdapter#setControllerAlwaysOn(boolean)}.
     * @hide
     */
    @SystemApi
    @FlaggedApi(Flags.FLAG_NFC_OEM_EXTENSION)
    public static final int ENABLE_DEFAULT = NfcAdapter.CONTROLLER_ALWAYS_ON_MODE_DEFAULT;

    /**
     * Mode Type for {@link #setControllerAlwaysOn(int)}.
     * Opens transport to communicate with controller
     * @hide
     */
    @SystemApi
    @FlaggedApi(Flags.FLAG_NFC_OEM_EXTENSION)
    public static final int ENABLE_TRANSPARENT = 2;

    /**
     * Mode Type for {@link #setControllerAlwaysOn(int)}.
     * Opens transport to communicate with controller
     * and initializes and enables the EE subsystem
     * @hide
     */
    @SystemApi
    @FlaggedApi(Flags.FLAG_NFC_OEM_EXTENSION)
    public static final int ENABLE_EE = 3;

    /**
     * Mode Type for {@link #setControllerAlwaysOn(int)}.
     * Disable the Controller Always On Mode
     * @hide
     */
    @SystemApi
    @FlaggedApi(Flags.FLAG_NFC_OEM_EXTENSION)
    public static final int DISABLE = NfcAdapter.CONTROLLER_ALWAYS_ON_DISABLE;

    /**
     * Possible controller modes for {@link #setControllerAlwaysOn(int)}.
     *
     * @hide
     */
    @IntDef(prefix = { "" }, value = {
        ENABLE_DEFAULT,
        ENABLE_TRANSPARENT,
        ENABLE_EE,
        DISABLE,
    })
    @Retention(RetentionPolicy.SOURCE)
    public @interface ControllerMode{}

    /**
     * Interface for Oem extensions for NFC.
     */
    public interface Callback {
        /**
         * Notify Oem to tag is connected or not
         * ex - if tag is connected  notify cover and Nfctest app if app is in testing mode
         *
         * @param connected status of the tag true if tag is connected otherwise false
         * @param tag Tag details
         */
        void onTagConnected(boolean connected, @NonNull Tag tag);

        /**
        * Notifies NFC is activated in listen mode.
        *
        * @param isActivated true, if listen mode activated, else de-activated.
        */
        void onCardEmulationActivated(boolean isActivated);

        /**
        * Notifies the NFC RF Field status
        *
        * @param isActivated true, if RF Field is ON, else RF Field is OFF.
        */
        void onRfFieldActivated(boolean isActivated);

        /**
        * Notifies the NFC RF discovery status
        *
        * @param isDiscoveryStarted true, if RF discovery started, else RF state is Idle.
        */
        void onRfDiscoveryStarted(boolean isDiscoveryStarted);
    }


    /**
     * Constructor to be used only by {@link NfcAdapter}.
     * @hide
     */
    public NfcOemExtension(@NonNull Context context, @NonNull NfcAdapter adapter) {
        mContext = context;
        mAdapter = adapter;
        mOemNfcExtensionCallback = new NfcOemExtensionCallback();
    }

    /**
     * Get NdefNfcee Object to write or read data from Ndef Nfcee
     *
     * @return NdefNfcee
     * @hide
     */
    @SystemApi
    @NonNull
    @FlaggedApi(Flags.FLAG_NFC_OEM_EXTENSION)
    public NdefNfcee getNdefNfcee() {
      return NdefNfcee.getInstance(mAdapter);
    }

    /**
     * Register an {@link Callback} to listen for NFC oem extension callbacks
     * Multiple clients can register and callbacks will be invoked asynchronously.
     *
     * <p>The provided callback will be invoked by the given {@link Executor}.
     * As part of {@link #registerCallback(Executor, Callback)} the
     * {@link Callback} will be invoked with current NFC state
     * before the {@link #registerCallback(Executor, Callback)} function completes.
     *
     * @param executor an {@link Executor} to execute given callback
     * @param callback oem implementation of {@link Callback}
     */
    @FlaggedApi(Flags.FLAG_NFC_OEM_EXTENSION)
    @RequiresPermission(android.Manifest.permission.WRITE_SECURE_SETTINGS)
    public void registerCallback(@NonNull @CallbackExecutor Executor executor,
            @NonNull Callback callback) {
        synchronized (mLock) {
            if (executor == null || callback == null) {
                Log.e(TAG, "Executor and Callback must not be null!");
                throw new IllegalArgumentException();
            }

            if (mCallbackMap.containsKey(callback)) {
                Log.e(TAG, "Callback already registered. Unregister existing callback before"
                        + "registering");
                throw new IllegalArgumentException();
            }
            mCallbackMap.put(callback, executor);
            if (!mIsRegistered) {
                try {
                    NfcAdapter.sService.registerOemExtensionCallback(mOemNfcExtensionCallback);
                    mIsRegistered = true;
                } catch (RemoteException e) {
                    Log.w(TAG, "Failed to register oem callback");
                    mCallbackMap.remove(callback);
                    throw e.rethrowFromSystemServer();
                }
            } else {
                updateNfCState(callback, executor);
            }
        }
    }

    private void updateNfCState(Callback callback, Executor executor) {
        if (callback != null) {
            Log.i(TAG, "updateNfCState");
            executor.execute(() -> {
                callback.onCardEmulationActivated(mCardEmulationActivated);
                callback.onRfFieldActivated(mRfFieldActivated);
                callback.onRfDiscoveryStarted(mRfDiscoveryStarted);
            });
        }
    }

    /**
     * Unregister the specified {@link Callback}
     *
     * <p>The same {@link Callback} object used when calling
     * {@link #registerCallback(Executor, Callback)} must be used.
     *
     * <p>Callbacks are automatically unregistered when an application process goes away
     *
     * @param callback oem implementation of {@link Callback}
     */
    @FlaggedApi(Flags.FLAG_NFC_OEM_EXTENSION)
    @RequiresPermission(android.Manifest.permission.WRITE_SECURE_SETTINGS)
    public void unregisterCallback(@NonNull Callback callback) {
        synchronized (mLock) {
            if (!mCallbackMap.containsKey(callback) || !mIsRegistered) {
                Log.e(TAG, "Callback not registered");
                throw new IllegalArgumentException();
            }
            if (mCallbackMap.size() == 1) {
                try {
                    NfcAdapter.sService.unregisterOemExtensionCallback(mOemNfcExtensionCallback);
                    mIsRegistered = false;
                    mCallbackMap.remove(callback);
                } catch (RemoteException e) {
                    Log.w(TAG, "Failed to unregister Oem Callback with service");
                    throw e.rethrowFromSystemServer();
                }
            } else {
                mCallbackMap.remove(callback);
            }
        }
    }

    /**
     * Clear NfcService preference, interface method to clear NFC preference values on OEM specific
     * events. For ex: on soft reset, Nfc default values needs to be overridden by OEM defaults.
     */
    @FlaggedApi(Flags.FLAG_NFC_OEM_EXTENSION)
    @RequiresPermission(android.Manifest.permission.WRITE_SECURE_SETTINGS)
    public void clearPreference() {
        try {
            NfcAdapter.sService.clearPreference();
        } catch (RemoteException e) {
            mAdapter.attemptDeadServiceRecovery(e);
        }
    }

    /**
     * Get the Active NFCEE List
     *
     * @return List of activated secure element list on success
     *         which can contain "eSE" and "UICC",
     *         otherwise empty list.
     */
    @NonNull
    @FlaggedApi(Flags.FLAG_NFC_OEM_EXTENSION)
    public List<String> getActiveNfceeList() {
      List<String> nfceeList = new ArrayList<String>();
      try {
        nfceeList = NfcAdapter.sService.fetchActiveNfceeList();
      } catch (RemoteException e) {
        Log.e(TAG, "getActiveNfceeList: failed", e);
      }
      return nfceeList;
    }

    /**
     * Sets NFC controller always on feature.
     * <p>This API is for the NFCC internal state management. It allows to discriminate
     * the controller function from the NFC function by keeping the NFC controller on without
     * any NFC RF enabled if necessary.
     * <p>This call is asynchronous.
     * Register a listener {@link NfcAdapter.ControllerAlwaysOnListener}
     * by {@link NfcAdapter#registerControllerAlwaysOnListener} to find out when the operation is
     * complete.
     * <p>If this returns true, then either NFCC always on state has been set based on the value,
     * or a {@link NfcAdapter.ControllerAlwaysOnListener#onControllerAlwaysOnChanged(boolean)}
     * will be invoked to indicate the state change.
     * If this returns false, then there is some problem that prevents an attempt to turn NFCC
     * always on.
     * @params mode ref {@link ControllerMode}
     * @throws UnsupportedOperationException if FEATURE_NFC,
     * FEATURE_NFC_HOST_CARD_EMULATION, FEATURE_NFC_HOST_CARD_EMULATION_NFCF,
     * FEATURE_NFC_OFF_HOST_CARD_EMULATION_UICC and FEATURE_NFC_OFF_HOST_CARD_EMULATION_ESE
     * are unavailable
     * @return void
     * @hide
     */
    @SystemApi
    @FlaggedApi(Flags.FLAG_NFC_OEM_EXTENSION)
    @RequiresPermission(android.Manifest.permission.NFC_SET_CONTROLLER_ALWAYS_ON)
    public boolean setControllerAlwaysOn(@ControllerMode int mode) {
        if (!NfcAdapter.sHasNfcFeature && !NfcAdapter.sHasCeFeature) {
            throw new UnsupportedOperationException();
        }
        try {
            return NfcAdapter.sService.setControllerAlwaysOn(mode);
        } catch (RemoteException e) {
            mAdapter.attemptDeadServiceRecovery(e);
            // Try one more time
            if (NfcAdapter.sService == null) {
                Log.e(TAG, "Failed to recover NFC Service.");
                return false;
            }
            try {
                return NfcAdapter.sService.setControllerAlwaysOn(mode);
            } catch (RemoteException ee) {
                Log.e(TAG, "Failed to recover NFC Service.");
            }
            return false;
        }
    }

    private final class NfcOemExtensionCallback extends INfcOemExtensionCallback.Stub {
        @Override
        public void onTagConnected(boolean connected, Tag tag) throws RemoteException {
            synchronized (mLock) {
                final long identity = Binder.clearCallingIdentity();
                try {
                    for (Callback callback : mCallbackMap.keySet()) {
                        Executor executor = mCallbackMap.get(callback);
                        executor.execute(() -> callback.onTagConnected(connected, tag));
                    }
                } catch (RuntimeException ex) {
                    throw ex;
                } finally {
                    Binder.restoreCallingIdentity(identity);
                }
            }
        }

        @Override
        public void onCardEmulationActivated(boolean isActivated) throws RemoteException {
            mCardEmulationActivated = isActivated;
            synchronized (mLock) {
                final long identity = Binder.clearCallingIdentity();
                try {
                    for (Callback callback : mCallbackMap.keySet()) {
                        Executor executor = mCallbackMap.get(callback);
                        executor.execute(() -> callback.onCardEmulationActivated(isActivated));
                    }
                } catch (RuntimeException ex) {
                    throw ex;
                } finally {
                    Binder.restoreCallingIdentity(identity);
                }
            }
        }

        @Override
        public void onRfFieldActivated(boolean isActivated) throws RemoteException {
            mRfFieldActivated = isActivated;
            synchronized (mLock) {
                final long identity = Binder.clearCallingIdentity();
                try {
                    for (Callback callback : mCallbackMap.keySet()) {
                        Executor executor = mCallbackMap.get(callback);
                        executor.execute(() -> callback.onRfFieldActivated(isActivated));
                    }
                } catch (RuntimeException ex) {
                    throw ex;
                } finally {
                    Binder.restoreCallingIdentity(identity);
                }
            }
        }

        @Override
        public void onRfDiscoveryStarted(boolean isDiscoveryStarted) throws RemoteException {
            mRfDiscoveryStarted = isDiscoveryStarted;
            synchronized (mLock) {
                final long identity = Binder.clearCallingIdentity();
                try {
                    for (Callback callback : mCallbackMap.keySet()) {
                        Executor executor = mCallbackMap.get(callback);
                        executor.execute(() -> callback.onRfDiscoveryStarted(isDiscoveryStarted));
                    }
                } catch (RuntimeException ex) {
                    throw ex;
                } finally {
                    Binder.restoreCallingIdentity(identity);
                }
            }
        }
    }
}
