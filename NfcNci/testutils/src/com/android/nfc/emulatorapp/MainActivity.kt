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

import android.content.ComponentName
import android.content.pm.PackageManager
import android.os.Bundle
import androidx.appcompat.app.AppCompatActivity;
import androidx.activity.viewModels
import androidx.lifecycle.Observer
import com.google.android.material.button.MaterialButton
import com.google.android.material.textview.MaterialTextView
import java.io.BufferedReader
import java.io.InputStreamReader
import kotlinx.serialization.json.Json
import kotlinx.serialization.json.jsonArray
import kotlinx.serialization.json.jsonObject

class MainActivity : AppCompatActivity() {

  private val viewModel: EmulatorViewModel by viewModels()

  override fun onCreate(savedInstanceState: Bundle?) {
    super.onCreate(savedInstanceState)
    setContentView(R.layout.activity_main)

    val observer =
      Observer<EmulatorUiState> { state ->
        val replayedFileText = "${getString(R.string.replayed_file_text)} ${state.snoopFile}"
        findViewById<MaterialTextView>(R.id.snoop_file_name).text = replayedFileText
        val logText = "${getString(R.string.log_text)}\n${state.transactionLog}"
        findViewById<MaterialTextView>(R.id.transaction_log).text = logText
      }
    viewModel.uiState.observe(this, observer)
    EmulatorHostApduService.viewModel = viewModel

    val snoopData = intent.getStringExtra(SNOOP_DATA_FLAG)
    if (snoopData != null) {
      val apdus = parseJsonString(snoopData)
      updateService(apdus)
    }
    startHostApduService()
  }

  private fun startHostApduService() {
    packageManager.setComponentEnabledSetting(
      ComponentName(this, EmulatorHostApduService::class.java),
      PackageManager.COMPONENT_ENABLED_STATE_ENABLED,
      PackageManager.DONT_KILL_APP,
    )
  }

  /** Extracts all APDU commands and responses from JSON string. */
  private fun parseJsonString(text: String): List<ApduPair> {
    val apduPairs = mutableListOf<ApduPair>()
    val array = Json.parseToJsonElement(text).jsonArray
    for (element in array) {
      val pair = element.jsonObject
      val commands = pair.get("commands")?.jsonArray
      val responses = pair.get("responses")?.jsonArray
      if (commands == null || responses == null || commands.size != responses.size) {
        continue
      }
      for (i in 0..<commands.size) {
        val command = standardize(commands.get(i).toString())
        val response = standardize(responses.get(i).toString())
        apduPairs.add(ApduPair(command, response))
      }
    }
    return apduPairs
  }

  private fun standardize(s: String): String {
    return s.replace("[", "").replace("]", "").replace("\'", "").replace(" ", "").replace("\"", "")
  }

  /** Updates EmulatorHostApduService with the given APDU commands and responses. */
  private fun updateService(apdus: List<ApduPair>) {
    val hashmap: HashMap<String, MutableList<String>> = HashMap()
    for (apduPair in apdus) {
      val existingList = hashmap[apduPair.command]
      if (existingList == null) {
        hashmap[apduPair.command] = mutableListOf(apduPair.response)
      } else {
        existingList.add(apduPair.response)
        hashmap[apduPair.command] = existingList
      }
    }
    EmulatorHostApduService.apdus.postValue(hashmap)
  }

  companion object {
    private const val TAG = "EmulatorHostApduServiceLog"
    const val SNOOP_DATA_FLAG = "snoop_data"
    private const val PARSED_FILES_DIR = "src/com/android/nfc/emulatorapp/parsed_files/"
  }
}