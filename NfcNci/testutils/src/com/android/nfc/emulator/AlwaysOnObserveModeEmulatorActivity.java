/*
 * Copyright (C) 2026 The Android Open Source Project
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
package com.android.nfc.emulator;

import android.content.BroadcastReceiver;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.nfc.NfcAdapter.ReaderCallback;
import android.nfc.Tag;
import android.nfc.cardemulation.PollingFrame;
import android.os.Bundle;
import android.provider.Settings;
import android.util.Log;

import com.android.nfc.service.PollingLoopService;

import java.util.ArrayList;
import java.util.HexFormat;

public class AlwaysOnObserveModeEmulatorActivity extends BaseEmulatorActivity implements ReaderCallback {
    private static final String TAG = "AlwaysOnObserveModeEmulatorActivity";

    private final BroadcastReceiver mReceiver =
            new BroadcastReceiver() {
                @Override
                public void onReceive(Context context, Intent intent) {
                    if (PollingLoopService.POLLING_FRAME_ACTION.equals(intent.getAction())) {
                        ArrayList<PollingFrame> frames =
                                intent.getParcelableArrayListExtra(
                                        PollingLoopService.POLLING_FRAME_EXTRA, PollingFrame.class);
                        if (frames != null) {
                            for (PollingFrame frame : frames) {
                                byte[] data = frame.getData();
                                if (data != null && HexFormat.of().formatHex(data).equalsIgnoreCase("DEADBEEF")) {
                                    setTestPassed();
                                    break;
                                }
                            }
                        }
                    }
                }
            };

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        androidx.test.platform.app.InstrumentationRegistry.getInstrumentation()
                .getUiAutomation().adoptShellPermissionIdentity();
        setupServices(PollingLoopService.COMPONENT);
        Settings.Secure.putString(
                getContentResolver(), "nfc.gesture_poll_frame", "DEADBEEF");
        mAdapter.registerGestureExchangeReaderCallback(getMainExecutor(), this);
        mCardEmulation.registerPollingLoopFilterForService(
                PollingLoopService.COMPONENT, "DEADBEEF", false);
        IntentFilter filter = new IntentFilter(PollingLoopService.POLLING_FRAME_ACTION);
        registerReceiver(mReceiver, filter, RECEIVER_EXPORTED);
    }

    @Override
    protected void onResume() {
        super.onResume();
        mCardEmulation.setPreferredService(this, PollingLoopService.COMPONENT);
        waitForPreferredService();
    }

    @Override
    protected void onPause() {
        super.onPause();
        Settings.Secure.putString(
                getContentResolver(), "nfc.gesture_poll_frame", "");
        mAdapter.unregisterGestureExchangeReaderCallback(this);
        androidx.test.platform.app.InstrumentationRegistry.getInstrumentation()
                .getUiAutomation().dropShellPermissionIdentity();
        mCardEmulation.unsetPreferredService(this);
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        unregisterReceiver(mReceiver);
    }

    @Override
    public ComponentName getPreferredServiceComponent() {
        return PollingLoopService.COMPONENT;
    }

    @Override
    public void onTagDiscovered(Tag tag) {
        Log.d(TAG, "onTagDiscovered: " + tag);
    }
}
