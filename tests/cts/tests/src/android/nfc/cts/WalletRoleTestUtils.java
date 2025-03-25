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

package android.nfc.cts;

import static android.Manifest.permission.INTERACT_ACROSS_USERS_FULL;
import static android.Manifest.permission.MANAGE_DEFAULT_APPLICATIONS;
import static android.Manifest.permission.MANAGE_ROLE_HOLDERS;
import static android.Manifest.permission.OBSERVE_ROLE_HOLDERS;

import static org.junit.Assert.assertTrue;
import static org.junit.Assume.assumeFalse;

import android.app.role.OnRoleHoldersChangedListener;
import android.app.role.RoleManager;
import android.content.ComponentName;
import android.content.Context;
import android.content.res.Resources;
import android.os.UserManager;
import android.text.TextUtils;

import com.google.common.util.concurrent.MoreExecutors;

import org.junit.Assert;

import java.util.Arrays;
import java.util.List;
import java.util.concurrent.CountDownLatch;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.atomic.AtomicReference;

public final class WalletRoleTestUtils {

    private WalletRoleTestUtils() {}

    static final String CTS_PACKAGE_NAME = "android.nfc.cts";
    static final String WALLET_HOLDER_PACKAGE_NAME = "com.android.test.walletroleholder";
    static final String WALLET_HOLDER_SERVICE_DESC = "Wallet Role CTS Nfc Test Service";
    static final String NFC_FOREGROUND_PACKAGE_NAME = "com.android.test.foregroundnfc";
    static final String NON_PAYMENT_NFC_PACKAGE_NAME = "com.android.test.nonpaymentnfc";
    static final String PAYMENT_AID_1 = "A000000004101012";
    static final String PAYMENT_AID_2 = "A000000004101018";
    static final String PAYMENT_AID_3 = "A000000005101011";
    static final String NON_PAYMENT_AID_1 = "F053414950454D";

    private static final String WALLET_OVERLAY_CONFIG = "config_defaultWallet";
    private static final String CERTIFICATE_SEPARATOR = ":";

    static final List<String> WALLET_HOLDER_AIDS = Arrays.asList("A000000004101011",
            "A000000004101012",
            "A000000004101013",
            "A000000004101018");

    static ComponentName getWalletRoleHolderService() {
        return new ComponentName(WALLET_HOLDER_PACKAGE_NAME,
                "com.android.test.walletroleholder.WalletRoleHolderApduService");
    }

    static ComponentName getWalletRoleHolderXService() {
        return new ComponentName(WALLET_HOLDER_PACKAGE_NAME,
                "com.android.test.walletroleholder.XWalletRoleHolderApduService");
    }

    static ComponentName getForegroundService() {
        return new ComponentName(NFC_FOREGROUND_PACKAGE_NAME,
                "com.android.test.foregroundnfc.ForegroundApduService");
    }

    static ComponentName getAssociatedService() {
        return new ComponentName(NFC_FOREGROUND_PACKAGE_NAME,
                "com.android.test.foregroundnfc.AssociatedApduService");
    }

    static ComponentName getNonPaymentService() {
        return new ComponentName(NON_PAYMENT_NFC_PACKAGE_NAME,
                "com.android.test.nonpaymentnfc.NonPaymentApduService");
    }

    static ComponentName getWalletRoleHolderActivity() {
        return new ComponentName(WALLET_HOLDER_PACKAGE_NAME,
                "com.android.test.walletroleholder.WalletRoleHolderForegroundActivity");
    }

    static boolean setDefaultWalletRoleHolder(Context context, String packageName) {
        RoleManager roleManager = context.getSystemService(RoleManager.class);
        CountDownLatch countDownLatch = new CountDownLatch(1);
        AtomicReference<Boolean> result = new AtomicReference<>(false);
        try {
            roleManager.setDefaultApplication(RoleManager.ROLE_WALLET,
                    packageName, 0,
                    MoreExecutors.directExecutor(), aBoolean -> {
                        result.set(aBoolean);
                        countDownLatch.countDown();
                    });
            countDownLatch.await(3000, TimeUnit.MILLISECONDS);
        } catch (InterruptedException e) {
            return false;
        }
        return result.get();
    }

    static boolean removeRoleHolder(Context context, String currentHolder) {
        RoleManager roleManager = context.getSystemService(RoleManager.class);
        CountDownLatch countDownLatch = new CountDownLatch(1);
        AtomicReference<Boolean> result = new AtomicReference<>(false);
        try {
            roleManager.removeRoleHolderAsUser(RoleManager.ROLE_WALLET, currentHolder, 0,
                    context.getUser(), MoreExecutors.directExecutor(), aBoolean -> {
                        result.set(aBoolean);
                        countDownLatch.countDown();
                    });
            countDownLatch.await(3000, TimeUnit.MILLISECONDS);
        } catch (InterruptedException e) {
            return false;
        }
        return result.get();
    }

    static boolean setDefaultWalletRoleHolder(Context context) {
        return setDefaultWalletRoleHolder(context, "android.nfc.cts");
    }

    static String getDefaultWalletRoleHolder(Context context) {
        try {
            androidx.test.platform.app.InstrumentationRegistry.getInstrumentation()
                    .getUiAutomation().adoptShellPermissionIdentity(MANAGE_DEFAULT_APPLICATIONS);
            RoleManager roleManager = context.getSystemService(RoleManager.class);
            return roleManager.getDefaultApplication(RoleManager.ROLE_WALLET);
        } finally {
            androidx.test.platform.app.InstrumentationRegistry.getInstrumentation()
                    .getUiAutomation().dropShellPermissionIdentity();
        }
    }

    static void runWithRole(Context context, String roleHolder, Runnable runnable) {
        final UserManager userManager = context.getSystemService(UserManager.class);
        assumeFalse("Device must not be headless", userManager.isHeadlessSystemUserMode());
        try {
            runWithRoleNone(context, () -> {}); //Remove the role holder first to trigger callbacks
            RoleManager roleManager = context.getSystemService(RoleManager.class);
            CountDownLatch countDownLatch = new CountDownLatch(1);
            OnRoleHoldersChangedListener onRoleHoldersChangedListener = (roleName, user) -> {
                if (roleName.equals(RoleManager.ROLE_WALLET)) {
                    try {
                        // Wait a second to make sure all other callbacks are also fired on
                        // their respective executors.
                        Thread.sleep(1000);
                    } catch (InterruptedException e) {
                        throw new RuntimeException(e);
                    }
                    countDownLatch.countDown();
                }
            };
            androidx.test.platform.app.InstrumentationRegistry.getInstrumentation()
                    .getUiAutomation().adoptShellPermissionIdentity(OBSERVE_ROLE_HOLDERS);
            roleManager.addOnRoleHoldersChangedListenerAsUser(context.getMainExecutor(),
                    onRoleHoldersChangedListener, context.getUser());
            androidx.test.platform.app.InstrumentationRegistry.getInstrumentation()
                    .getUiAutomation()
                    .adoptShellPermissionIdentity(
                            MANAGE_DEFAULT_APPLICATIONS, INTERACT_ACROSS_USERS_FULL);
            assertTrue(setDefaultWalletRoleHolder(context, roleHolder));
            countDownLatch.await(4000, TimeUnit.MILLISECONDS);
            androidx.test.platform.app.InstrumentationRegistry.getInstrumentation()
                    .getUiAutomation().adoptShellPermissionIdentity(OBSERVE_ROLE_HOLDERS);
            roleManager.removeOnRoleHoldersChangedListenerAsUser(onRoleHoldersChangedListener,
                    context.getUser());
            runnable.run();
        } catch (InterruptedException e) {
            throw new RuntimeException(e);
        } finally {
            runWithRoleNone(context, () -> {}); //Remove the role holder first to trigger callbacks
            androidx.test.platform.app.InstrumentationRegistry.getInstrumentation()
                    .getUiAutomation().dropShellPermissionIdentity();
        }
    }

    static boolean canAssignRoleToPackage(Context context, String packageName) {
        String previousHolder = getDefaultWalletRoleHolder(context);
        try {
            androidx.test.platform.app.InstrumentationRegistry.getInstrumentation()
                    .getUiAutomation().adoptShellPermissionIdentity(
                            MANAGE_DEFAULT_APPLICATIONS, INTERACT_ACROSS_USERS_FULL);
            boolean canSet = setDefaultWalletRoleHolder(context, packageName);
            if (canSet && previousHolder != null) {
                setDefaultWalletRoleHolder(context, previousHolder);
            }
            return canSet;
        } finally {
            androidx.test.platform.app.InstrumentationRegistry.getInstrumentation()
                    .getUiAutomation().dropShellPermissionIdentity();
        }
    }

    static void runWithRoleNone(Context context, Runnable runnable) {
        try {
            String currentHolder = getDefaultWalletRoleHolder(context);
            RoleManager roleManager = context.getSystemService(RoleManager.class);
            CountDownLatch countDownLatch = new CountDownLatch(1);
            OnRoleHoldersChangedListener onRoleHoldersChangedListener = (roleName, user) -> {
                if (roleName.equals(RoleManager.ROLE_WALLET)) {
                    try {
                        // Wait a second to make sure all other callbacks are also fired on
                        // their respective executors.
                        Thread.sleep(1000);
                    } catch (InterruptedException e) {
                        throw new RuntimeException(e);
                    }
                    countDownLatch.countDown();
                }
            };
            androidx.test.platform.app.InstrumentationRegistry.getInstrumentation()
                    .getUiAutomation().adoptShellPermissionIdentity(OBSERVE_ROLE_HOLDERS);
            roleManager.addOnRoleHoldersChangedListenerAsUser(context.getMainExecutor(),
                    onRoleHoldersChangedListener, context.getUser());
            androidx.test.platform.app.InstrumentationRegistry.getInstrumentation()
                    .getUiAutomation().adoptShellPermissionIdentity(MANAGE_ROLE_HOLDERS);
            if (currentHolder != null) {
                roleManager.setRoleFallbackEnabled(RoleManager.ROLE_WALLET, false);
                assertTrue(removeRoleHolder(context, currentHolder));
                countDownLatch.await(4000, TimeUnit.MILLISECONDS);
                roleManager.setRoleFallbackEnabled(RoleManager.ROLE_WALLET, true);
            }
            androidx.test.platform.app.InstrumentationRegistry.getInstrumentation()
                    .getUiAutomation().adoptShellPermissionIdentity(OBSERVE_ROLE_HOLDERS);
            roleManager.removeOnRoleHoldersChangedListenerAsUser(onRoleHoldersChangedListener,
                    context.getUser());
            runnable.run();
        } catch (InterruptedException e) {
            throw new RuntimeException(e);
        } finally {
            androidx.test.platform.app.InstrumentationRegistry.getInstrumentation()
                    .getUiAutomation().dropShellPermissionIdentity();
        }
    }

    static void clearRoleHolders(Context context) {
        try {
            androidx.test.platform.app.InstrumentationRegistry.getInstrumentation()
                    .getUiAutomation().adoptShellPermissionIdentity(MANAGE_ROLE_HOLDERS);
            RoleManager roleManager = context.getSystemService(RoleManager.class);
            CountDownLatch countDownLatch = new CountDownLatch(1);
            AtomicReference<Boolean> result = new AtomicReference<>(false);
            roleManager.clearRoleHoldersAsUser(RoleManager.ROLE_WALLET, 0,
                    context.getUser(), MoreExecutors.directExecutor(), aBoolean -> {
                        try {
                            // Wait a second to make sure all other callbacks are also fired on
                            // their respective executors.
                            Thread.sleep(1000);
                        } catch (InterruptedException e) {
                            throw new RuntimeException(e);
                        }
                        result.set(aBoolean);
                        countDownLatch.countDown();
                    });
            countDownLatch.await(4000, TimeUnit.MILLISECONDS);
        } catch (Exception e) {
            throw new RuntimeException(e);
        } finally {
            androidx.test.platform.app.InstrumentationRegistry.getInstrumentation()
                    .getUiAutomation().dropShellPermissionIdentity();
        }
    }

    static String getOverLayDefaultHolder(Context context) {
        Resources resources = context.getResources();
        int resourceId = resources.getIdentifier(WALLET_OVERLAY_CONFIG,
                "string", "android");
        if (resourceId == 0) {
            return null;
        }
        String defaultHolders;
        try {
            defaultHolders = resources.getString(resourceId);
        } catch (Resources.NotFoundException e) {
            return null;
        }
        if (TextUtils.isEmpty(defaultHolders)) {
            return null;
        }
        int certificateSeparatorIndex = defaultHolders.indexOf(CERTIFICATE_SEPARATOR);
        if (certificateSeparatorIndex != -1) {
            return defaultHolders.substring(0, certificateSeparatorIndex);
        } else {
            return defaultHolders;
        }
    }
}
