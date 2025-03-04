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

package com.android.nfc.emulatorapp

import android.app.Instrumentation
import android.content.Intent
import androidx.test.platform.app.InstrumentationRegistry
import com.android.nfc.utils.NfcSnippet
import com.google.android.mobly.snippet.rpc.Rpc;

class NfcApduDeviceSnippet : NfcSnippet() {
  private lateinit var mActivity: MainActivity

  @Rpc(description = "Start Main Activity")
  fun startMainActivity(json: String) {
    val instrumentation: Instrumentation = InstrumentationRegistry.getInstrumentation()
    val intent = Intent(Intent.ACTION_MAIN)
    intent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK)
    intent.setClassName(
            instrumentation.getTargetContext(), MainActivity::class.java.getName())
    intent.putExtra(MainActivity.SNOOP_DATA_FLAG, json)

    mActivity = instrumentation.startActivitySync(intent) as MainActivity
  }

  @Rpc(description = "Close activity")
  fun closeActivity() {
    mActivity.finish()
  }
}