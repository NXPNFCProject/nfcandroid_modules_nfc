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

import android.annotation.FlaggedApi;
import android.annotation.NonNull;
import android.annotation.SystemApi;
import android.os.Parcel;
import android.os.Parcelable;

/**
 * Represents the configuration for NFC RF discovery parameters.
 *
 * <p>This class encapsulates specific settings such as technology mode and frequency
 * used during the NFC discovery process.
 *
 * @hide
 */
@SystemApi
@FlaggedApi(com.android.nfc.module.flags.Flags.FLAG_NFCSTACK_26Q2_UPDATES)
public final class RfDiscoverConfig implements Parcelable {
    private final int mTechnologyMode;
    private final int mFrequency;

    /**
     * Constructs a new {@link RfDiscoverConfig} with the specified parameters.
     *
     * @param techMode The technology mode to be used for discovery.
     * @param frequency The frequency to be used for discovery.
     */
    public RfDiscoverConfig(int techMode, int frequency) {
        mTechnologyMode = techMode;
        mFrequency = frequency;
    }

    /**
     * Gets the technology mode configured for this discovery instance.
     *
     * @return The technology mode integer value.
     */
    public int getTechnologyMode() {
        return mTechnologyMode;
    }

    /**
     * Gets the frequency configured for this discovery instance.
     *
     * @return The frequency integer value.
     */
    public int getFrequency() {
        return mFrequency;
    }

    @Override
    public int describeContents() {
        return 0;
    }

    private RfDiscoverConfig(Parcel in) {
        this.mTechnologyMode = in.readInt();
        this.mFrequency = in.readInt();
    }

    public static final @NonNull Parcelable.Creator<RfDiscoverConfig> CREATOR =
            new Parcelable.Creator<RfDiscoverConfig>() {
                @Override
                public RfDiscoverConfig createFromParcel(Parcel in) {
                    return new RfDiscoverConfig(in);
                }

                @Override
                public RfDiscoverConfig[] newArray(int size) {
                    return new RfDiscoverConfig[size];
                }
            };

    @Override
    public void writeToParcel(@NonNull Parcel dest, int flags) {
        dest.writeInt(mTechnologyMode);
        dest.writeInt(mFrequency);
    }
}
