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
import android.annotation.SuppressLint;
import android.annotation.SystemApi;
import android.content.Context;
import android.os.Binder;
import android.os.RemoteException;
import android.os.ResultReceiver;
import android.util.Log;

import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.Executor;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.FutureTask;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.TimeoutException;
import java.util.function.Consumer;
import java.util.function.Function;
import java.util.function.Supplier;

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
    private Executor mExecutor = null;
    private Callback mCallback = null;
    private final Object mLock = new Object();
	private boolean mCardEmulationActivated = false;
    private boolean mRfFieldActivated = false;
    private boolean mRfDiscoveryStarted = false;
    private boolean mSeListenActivated = false;

    /**
     * Event that Host Card Emulation is activated.
     */
    public static final int HCE_ACTIVATE = 1;
    /**
     * Event that some data is transferred in Host Card Emulation.
     */
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
    public static final int HCE_DATA_TRANSFERRED = 2;
    /**
     * Event that Host Card Emulation is deactivated.
     */
    public static final int HCE_DEACTIVATE = 3;
    /**
     * Possible events from {@link Callback#onHceEventReceived}.
     *
     * @hide
     */
    @IntDef(value = {
            HCE_ACTIVATE,
            HCE_DATA_TRANSFERRED,
            HCE_DEACTIVATE
    })
    @Retention(RetentionPolicy.SOURCE)
    public @interface HostCardEmulationAction {}

    /**
     * Status OK
     */
    public static final int STATUS_OK = 0;
    /**
     * Status unknown error
     */
    public static final int STATUS_UNKNOWN_ERROR = 1;

    /**
     * Status codes passed to OEM extension callbacks.
     *
     * @hide
     */
    @IntDef(value = {
            STATUS_OK,
            STATUS_UNKNOWN_ERROR
    })
    @Retention(RetentionPolicy.SOURCE)
    public @interface StatusCode {}


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

        /**
        * Notifies the NFC SE Listen status
        *
        * @param isActivated true, if SE Listen is ON, else SE Listen is OFF.
        */
        void onSeListenActivated(boolean isActivated);

        /**
         * Update the Nfc Adapter State
         * @param state new state that need to be updated
         */
        void onStateUpdated(@NfcAdapter.AdapterState int state);
        /**
         * Check if NfcService apply routing method need to be skipped for
         * some feature.
         * @param isSkipped The {@link Consumer} to be completed. If apply routing can be skipped,
         *                  the {@link Consumer#accept(Object)} should be called with
         *                  {@link Boolean#TRUE}, otherwise call with {@link Boolean#FALSE}.
         */
        void onApplyRouting(@NonNull Consumer<Boolean> isSkipped);
        /**
         * Check if NfcService ndefRead method need to be skipped To skip
         * and start checking for presence of tag
         * @param isSkipped The {@link Consumer} to be completed. If Ndef read can be skipped,
         *                  the {@link Consumer#accept(Object)} should be called with
         *                  {@link Boolean#TRUE}, otherwise call with {@link Boolean#FALSE}.
         */
        void onNdefRead(@NonNull Consumer<Boolean> isSkipped);
        /**
         * Method to check if Nfc is allowed to be enabled by OEMs.
         * @param isAllowed The {@link Consumer} to be completed. If enabling NFC is allowed,
         *                  the {@link Consumer#accept(Object)} should be called with
         *                  {@link Boolean#TRUE}, otherwise call with {@link Boolean#FALSE}.
         * false if NFC cannot be enabled at this time.
         */
        @SuppressLint("MethodNameTense")
        void onEnable(@NonNull Consumer<Boolean> isAllowed);
        /**
         * Method to check if Nfc is allowed to be disabled by OEMs.
         * @param isAllowed The {@link Consumer} to be completed. If disabling NFC is allowed,
         *                  the {@link Consumer#accept(Object)} should be called with
         *                  {@link Boolean#TRUE}, otherwise call with {@link Boolean#FALSE}.
         * false if NFC cannot be disabled at this time.
         */
        void onDisable(@NonNull Consumer<Boolean> isAllowed);

        /**
         * Callback to indicate that Nfc starts to boot.
         */
        void onBootStarted();

        /**
         * Callback to indicate that Nfc starts to enable.
         */
        void onEnableStarted();

        /**
         * Callback to indicate that Nfc starts to enable.
         */
        void onDisableStarted();

        /**
         * Callback to indicate if NFC boots successfully or not.
         * @param status the status code indicating if boot finished successfully
         */
        void onBootFinished(@StatusCode int status);

        /**
         * Callback to indicate if NFC is successfully enabled.
         * @param status the status code indicating if enable finished successfully
         */
        void onEnableFinished(@StatusCode int status);

        /**
         * Callback to indicate if NFC is successfully disabled.
         * @param status the status code indicating if disable finished successfully
         */
        void onDisableFinished(@StatusCode int status);

        /**
         * Check if NfcService tag dispatch need to be skipped.
         * @param isSkipped The {@link Consumer} to be completed. If tag dispatch can be skipped,
         *                  the {@link Consumer#accept(Object)} should be called with
         *                  {@link Boolean#TRUE}, otherwise call with {@link Boolean#FALSE}.
         */
        void onTagDispatch(@NonNull Consumer<Boolean> isSkipped);

        /**
         * Notifies routing configuration is changed.
         */
        void onRoutingChanged();

        /**
         * API to activate start stop cpu boost on hce event.
         *
         * <p>When HCE is activated, transferring data, and deactivated,
         * must call this method to activate, start and stop cpu boost respectively.
         * @param action Flag indicating actions to activate, start and stop cpu boost.
         */
        void onHceEventReceived(@HostCardEmulationAction int action);
    }


    /**
     * Constructor to be used only by {@link NfcAdapter}.
     */
    NfcOemExtension(@NonNull Context context, @NonNull NfcAdapter adapter) {
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
                NfcAdapter.callService(() -> {
                    NfcAdapter.sService.unregisterOemExtensionCallback(mOemNfcExtensionCallback);
                    mIsRegistered = false;
                    mCallbackMap.remove(callback);
                });
            } else {
                mCallbackMap.remove(callback);
            }
        }
    }

    /**
     * Pauses NFC tag reader mode polling for a {@code timeoutInMs} millisecond.
     * In case of {@code timeoutInMs} is zero or invalid
     * polling will be stopped indefinitely use {@link #resumePolling() to resume the polling.
     * @param timeoutInMs the pause polling duration in millisecond
     */
    @FlaggedApi(Flags.FLAG_NFC_OEM_EXTENSION)
    @RequiresPermission(android.Manifest.permission.WRITE_SECURE_SETTINGS)
    public void pausePolling(int timeoutInMs) {
        try {
            NfcAdapter.sService.pausePolling(timeoutInMs);
        } catch (RemoteException e) {
            mAdapter.attemptDeadServiceRecovery(e);
        }
    }

    /**
     * Resumes default NFC tag reader mode polling for the current device state if polling is
     * paused. Calling this while already in polling is a no-op.
     */
    @FlaggedApi(Flags.FLAG_NFC_OEM_EXTENSION)
    @RequiresPermission(android.Manifest.permission.WRITE_SECURE_SETTINGS)
    public void resumePolling () {
        try {
            NfcAdapter.sService.resumePolling();
        } catch (RemoteException e) {
            mAdapter.attemptDeadServiceRecovery(e);
        }
    }

    /**
     * Clear NfcService preference, interface method to clear NFC preference values on OEM specific
     * events. For ex: on soft reset, Nfc default values needs to be overridden by OEM defaults.
     */
    @FlaggedApi(Flags.FLAG_NFC_OEM_EXTENSION)
    @RequiresPermission(android.Manifest.permission.WRITE_SECURE_SETTINGS)
    public void clearPreference() {
        NfcAdapter.callService(() -> NfcAdapter.sService.clearPreference());
    }

    /**
     * Get the screen state from system and set it to current screen state.
     */
    @FlaggedApi(Flags.FLAG_NFC_OEM_EXTENSION)
    @RequiresPermission(android.Manifest.permission.WRITE_SECURE_SETTINGS)
    public void synchronizeScreenState() {
        NfcAdapter.callService(() -> NfcAdapter.sService.setScreenState());
    }

    /**
     * Check if the firmware needs updating.
     *
     * <p>If an update is needed, a firmware will be triggered when NFC is disabled.
     */
    @FlaggedApi(Flags.FLAG_NFC_OEM_EXTENSION)
    @RequiresPermission(android.Manifest.permission.WRITE_SECURE_SETTINGS)
    public void maybeTriggerFirmwareUpdate() {
        NfcAdapter.callService(() -> NfcAdapter.sService.checkFirmware());
    }

    /**
     * Get the Active NFCEE (NFC Execution Environment) List
     *
     * @return List of activated secure elements on success
     *         which can contain "eSE" and "UICC", otherwise empty list.
     */
    @NonNull
    @FlaggedApi(Flags.FLAG_NFC_OEM_EXTENSION)
    public List<String> getActiveNfceeList() {
        return NfcAdapter.callServiceReturn(() ->
            NfcAdapter.sService.fetchActiveNfceeList(), new ArrayList<String>());
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
                if (mCallback == null || mExecutor == null) {
                    return;
                }
                final long identity = Binder.clearCallingIdentity();
                try {
                    mExecutor.execute(() -> mCallback.onTagConnected(connected, tag));
                } finally {
                    Binder.restoreCallingIdentity(identity);
                }
            }
        }
        @Override
        public void onStateUpdated(int state) throws RemoteException {
            handleVoidCallback(state, mCallback::onStateUpdated);
        }
        @Override
        public void onApplyRouting(ResultReceiver isSkipped) throws RemoteException {
            handleVoidCallback(
                    new ReceiverWrapper(isSkipped), mCallback::onApplyRouting);
        }
        @Override
        public void onNdefRead(ResultReceiver isSkipped) throws RemoteException {
            handleVoidCallback(
                    new ReceiverWrapper(isSkipped), mCallback::onNdefRead);
        }
        @Override
        public void onEnable(ResultReceiver isAllowed) throws RemoteException {
            handleVoidCallback(
                    new ReceiverWrapper(isAllowed), mCallback::onEnable);
        }
        @Override
        public void onDisable(ResultReceiver isAllowed) throws RemoteException {
            handleVoidCallback(
                    new ReceiverWrapper(isAllowed), mCallback::onDisable);
        }
        @Override
        public void onBootStarted() throws RemoteException {
            handleVoidCallback(null, (Object input) -> mCallback.onBootStarted());
        }
        @Override
        public void onEnableStarted() throws RemoteException {
            handleVoidCallback(null, (Object input) -> mCallback.onEnableStarted());
        }
        @Override
        public void onDisableStarted() throws RemoteException {
            handleVoidCallback(null, (Object input) -> mCallback.onDisableStarted());
        }
        @Override
        public void onBootFinished(int status) throws RemoteException {
            handleVoidCallback(status, mCallback::onBootFinished);
        }
        @Override
        public void onEnableFinished(int status) throws RemoteException {
            handleVoidCallback(status, mCallback::onEnableFinished);
        }
        @Override
        public void onDisableFinished(int status) throws RemoteException {
            handleVoidCallback(status, mCallback::onDisableFinished);
        }
        @Override
        public void onTagDispatch(ResultReceiver isSkipped) throws RemoteException {
            handleVoidCallback(
                    new ReceiverWrapper(isSkipped), mCallback::onTagDispatch);
        }
        @Override
        public void onRoutingChanged() throws RemoteException {
            handleVoidCallback(null, (Object input) -> mCallback.onRoutingChanged());
        }
        @Override
        public void onHceEventReceived(int action) throws RemoteException {
            handleVoidCallback(action, mCallback::onHceEventReceived);
        }

        private <T> void handleVoidCallback(T input, Consumer<T> callbackMethod) {
            synchronized (mLock) {
                if (mCallback == null || mExecutor == null) {
                    return;
                }
                final long identity = Binder.clearCallingIdentity();
                try {
                    mExecutor.execute(() -> callbackMethod.accept(input));
                } finally {
                    Binder.restoreCallingIdentity(identity);
                }
            }
        }

        private <S, T> S handleNonVoidCallbackWithInput(
                S defaultValue, T input, Function<T, S> callbackMethod) throws RemoteException {
            synchronized (mLock) {
                if (mCallback == null) {
                    return defaultValue;
                }
                final long identity = Binder.clearCallingIdentity();
                S result = defaultValue;
                try {
                    ExecutorService executor = Executors.newSingleThreadExecutor();
                    FutureTask<S> futureTask = new FutureTask<>(
                            () -> callbackMethod.apply(input)
                    );
                    executor.submit(futureTask);
                    try {
                        result = futureTask.get(
                                OEM_EXTENSION_RESPONSE_THRESHOLD_MS, TimeUnit.MILLISECONDS);
                    } catch (ExecutionException | InterruptedException e) {
                        e.printStackTrace();
                    } catch (TimeoutException e) {
                        Log.w(TAG, "Callback timed out: " + callbackMethod);
                        e.printStackTrace();
                    } finally {
                        executor.shutdown();
                    }
                } finally {
                    Binder.restoreCallingIdentity(identity);
                }
                return result;
            }
        }

        private <T> T handleNonVoidCallbackWithoutInput(T defaultValue, Supplier<T> callbackMethod)
                throws RemoteException {
            synchronized (mLock) {
                if (mCallback == null) {
                    return defaultValue;
                }
                final long identity = Binder.clearCallingIdentity();
                T result = defaultValue;
                try {
                    ExecutorService executor = Executors.newSingleThreadExecutor();
                    FutureTask<T> futureTask = new FutureTask<>(
                            callbackMethod::get
                    );
                    executor.submit(futureTask);
                    try {
                        result = futureTask.get(
                                OEM_EXTENSION_RESPONSE_THRESHOLD_MS, TimeUnit.MILLISECONDS);
                    } catch (ExecutionException | InterruptedException e) {
                        e.printStackTrace();
                    } catch (TimeoutException e) {
                        Log.w(TAG, "Callback timed out: " + callbackMethod);
                        e.printStackTrace();
                    } finally {
                        executor.shutdown();
                    }
                } finally {
                    Binder.restoreCallingIdentity(identity);
                }
                return result;
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

        @Override
        public void onSeListenActivated(boolean isActivated) throws RemoteException {
            mSeListenActivated = isActivated;
            synchronized (mLock) {
                final long identity = Binder.clearCallingIdentity();
                try {
                    for (Callback callback : mCallbackMap.keySet()) {
                        Executor executor = mCallbackMap.get(callback);
                        executor.execute(() -> callback.onSeListenActivated(isActivated));
                    }
                } catch (RuntimeException ex) {
                    throw ex;
                } finally {
                    Binder.restoreCallingIdentity(identity);
                }
            }
        }
    }

    private class ReceiverWrapper implements Consumer<Boolean> {
        private final ResultReceiver mResultReceiver;

        ReceiverWrapper(ResultReceiver resultReceiver) {
            mResultReceiver = resultReceiver;
        }

        @Override
        public void accept(Boolean result) {
            mResultReceiver.send(result ? 1 : 0, null);
        }

        @Override
        public Consumer<Boolean> andThen(Consumer<? super Boolean> after) {
            return Consumer.super.andThen(after);
        }
    }
}
