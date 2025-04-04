#  Copyright (C) 2024 The Android Open Source Project
#
#  Licensed under the Apache License, Version 2.0 (the "License");
#  you may not use this file except in compliance with the License.
#  You may obtain a copy of the License at
#
#       http://www.apache.org/licenses/LICENSE-2.0
#
#  Unless required by applicable law or agreed to in writing, software
#  distributed under the License is distributed on an "AS IS" BASIS,
#  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#  See the License for the specific language governing permissions and
#  limitations under the License.
#
#  Licensed under the Apache License, Version 2.0 (the "License");
#  you may not use this file except in compliance with the License.
#  You may obtain a copy of the License at
#
#       http://www.apache.org/licenses/LICENSE-2.0
#
#  Unless required by applicable law or agreed to in writing, software
#  distributed under the License is distributed on an "AS IS" BASIS,
#  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#  See the License for the specific language governing permissions and
#  limitations under the License.

# Lint as: python3
"""CTS Tests that verify NFC HCE features.

These tests require one phone and one PN532 board. The phone acts as a card emulator, and the PN532
acts as an NFC reader. The devices should be placed back to back.
"""

from http.client import HTTPSConnection
import json
import logging
import ssl
import sys
import time

from android.platform.test.annotations import CddTest
from android.platform.test.annotations import ApiTest
from mobly import asserts
from mobly import base_test
from mobly import test_runner
from mobly import utils
from mobly.controllers import android_device


_LOG = logging.getLogger(__name__)
logging.basicConfig(level=logging.INFO)
try:
    import pn532
    from pn532.nfcutils import (
        parse_protocol_params,
        create_select_apdu,
        poll_and_transact,
        poll_and_observe_frames,
        get_apdus,
        POLLING_FRAME_ALL_TEST_CASES,
        POLLING_FRAMES_TYPE_A_SPECIAL,
        POLLING_FRAMES_TYPE_B_SPECIAL,
        POLLING_FRAMES_TYPE_F_SPECIAL,
        POLLING_FRAMES_TYPE_A_LONG,
        POLLING_FRAMES_TYPE_B_LONG,
        POLLING_FRAME_ON,
        POLLING_FRAME_OFF,
        get_frame_test_stats,
        TimedWrapper,
        ns_to_ms,
        ns_to_us,
        us_to_ms,
    )
except ImportError as e:
    _LOG.warning(f"Cannot import PN532 library due to {e}")

# Timeout to give the NFC service time to perform async actions such as
# discover tags.
_NFC_TIMEOUT_SEC = 10
_NFC_TECH_A_POLLING_ON = (0x1 #NfcAdapter.FLAG_READER_NFC_A
                          | 0x10 #NfcAdapter.FLAG_READER_NFC_BARCODE
                          | 0x80 #NfcAdapter.FLAG_READER_SKIP_NDEF_CHECK
                          )
_NFC_TECH_A_POLLING_OFF = (0x10 #NfcAdapter.FLAG_READER_NFC_BARCODE
                           | 0x80 #NfcAdapter.FLAG_READER_SKIP_NDEF_CHECK
                           )
_NFC_TECH_A_LISTEN_ON = 0x1 #NfcAdapter.FLAG_LISTEN_NFC_PASSIVE_A
_NFC_TECH_F_LISTEN_ON = 0x4 #NfcAdapter.FLAG_LISTEN_NFC_PASSIVE_F
_NFC_LISTEN_OFF = 0x0 #NfcAdapter.FLAG_LISTEN_DISABLE
_SERVICE_PACKAGE = "com.android.nfc.service"
_ACCESS_SERVICE = _SERVICE_PACKAGE + ".AccessService"
_OFFHOST_SERVICE = _SERVICE_PACKAGE + ".OffHostService"
_LARGE_NUM_AIDS_SERVICE = _SERVICE_PACKAGE + ".LargeNumAidsService"
_PAYMENT_SERVICE_1 = _SERVICE_PACKAGE + ".PaymentService1"
_PAYMENT_SERVICE_2 = _SERVICE_PACKAGE + ".PaymentService2"
_PAYMENT_SERVICE_DYNAMIC_AIDS = _SERVICE_PACKAGE + ".PaymentServiceDynamicAids"
_PREFIX_ACCESS_SERVICE = _SERVICE_PACKAGE + ".PrefixAccessService"
_PREFIX_PAYMENT_SERVICE_1 = _SERVICE_PACKAGE + ".PrefixPaymentService1"
_PREFIX_TRANSPORT_SERVICE_2 = _SERVICE_PACKAGE + ".PrefixTransportService2"
_SCREEN_OFF_PAYMENT_SERVICE = _SERVICE_PACKAGE + ".ScreenOffPaymentService"
_SCREEN_ON_ONLY_OFF_HOST_SERVICE = _SERVICE_PACKAGE + ".ScreenOnOnlyOffHostService"
_THROUGHPUT_SERVICE = _SERVICE_PACKAGE + ".ThroughputService"
_TRANSPORT_SERVICE_1 = _SERVICE_PACKAGE + ".TransportService1"
_TRANSPORT_SERVICE_2 = _SERVICE_PACKAGE + ".TransportService2"
_POLLING_LOOP_SERVICE_1 = _SERVICE_PACKAGE + ".PollingLoopService"
_POLLING_LOOP_SERVICE_2 = _SERVICE_PACKAGE + ".PollingLoopService2"

_NUM_POLLING_LOOPS = 50
_FAILED_TAG_MSG =  "Reader did not detect tag, transaction not attempted."
_FAILED_TRANSACTION_MSG = "Transaction failed, check device logs for more information."

_FRAME_EVENT_TIMEOUT_SEC = 1
_POLLING_FRAME_TIMESTAMP_TOLERANCE_MS = 5
_POLLING_FRAME_TIMESTAMP_EXCEED_COUNT_TOLERANCE_ = 3
_FAILED_MISSING_POLLING_FRAMES_MSG = "Device did not receive all polling frames"
_FAILED_TIMESTAMP_TOLERANCE_EXCEEDED_MSG = "Polling frame timestamp tolerance exceeded"
_FAILED_VENDOR_GAIN_VALUE_DROPPED_ON_POWER_INCREASE = """
Polling frame vendor specific gain value dropped on power increase
"""
_FAILED_FRAME_TYPE_INVALID = "Polling frame type is invalid"
_FAILED_FRAME_DATA_INVALID = "Polling frame data is invalid"



class CtsNfcHceMultiDeviceTestCases(base_test.BaseTestClass):

    def _set_up_emulator(self, *args, start_emulator_fun=None, service_list=[],
                 expected_service=None, is_payment=False, preferred_service=None,
                 payment_default_service=None, should_disable_services_on_destroy=True):
        """
        Sets up emulator device for multidevice tests.
        :param args: arguments for start_emulator_fun, if any
        :param start_emulator_fun: fun
            Custom function to start the emulator activity. If not present,
            startSimpleEmulatorActivity will be used.
        :param service_list: list
            List of services to set up. Only used if a custom function is not called.
        :param expected_service: String
            Class name of the service expected to handle the APDUs.
        :param is_payment: bool
            Whether test is setting up payment services. If so, this function will register
            this app as the default wallet.
        :param preferred_service: String
            Service to set as preferred service, if any.
        :param payment_default_service: String
            For payment tests only: the default payment service that is expected to handle APDUs.
        :param should_disable_services_on_destroy: bool
            Whether to disable services on destroy (set to False for reboot tests).

        :return:
        """
        role_held_handler = self.emulator.nfc_emulator.asyncWaitForRoleHeld(
            'RoleHeld')
        if start_emulator_fun is not None:
            start_emulator_fun(*args)
        else:
            if preferred_service is None:
                self.emulator.nfc_emulator.startSimpleEmulatorActivity(
                        service_list, expected_service, is_payment,
                        should_disable_services_on_destroy)
            else:
                self.emulator.nfc_emulator.startSimpleEmulatorActivityWithPreferredService(
                        service_list, expected_service, preferred_service, is_payment)

        if is_payment:
            role_held_handler.waitAndGet('RoleHeld', _NFC_TIMEOUT_SEC)
            if payment_default_service is None:
                raise Exception("Must define payment_default_service for payment tests.")
            self.emulator.nfc_emulator.waitForService(payment_default_service)

    def _set_up_reader_and_assert_transaction(self, expected_service=None):
        """
        Sets up reader, and asserts successful APDU transaction
        :param expected_service: string
                Class name of the service expected to handle the APDUs on the emulator device.
        :return:
        """
        if expected_service is None:
            raise Exception('expected_service must be defined.')
        command_apdus, response_apdus = get_apdus(self.emulator.nfc_emulator, expected_service)
        tag_detected, transacted = poll_and_transact(self.pn532, command_apdus, response_apdus)
        asserts.assert_true(tag_detected, _FAILED_TAG_MSG)
        asserts.assert_true(transacted, _FAILED_TRANSACTION_MSG)

    def _is_cuttlefish_device(self, ad: android_device.AndroidDevice) -> bool:
        product_name = ad.adb.getprop("ro.product.name")
        return "cf_x86" in product_name

    def _reboot(self, ad: android_device.AndroidDevice):
        ad.reboot()
        ad.nfc_emulator.turnScreenOn()
        ad.nfc_emulator.pressMenu()

    def _get_casimir_id_for_device(self):
        host = "localhost"
        conn = HTTPSConnection(host, 1443, context=ssl._create_unverified_context())
        path = '/devices'
        headers = {'Content-type': 'application/json'}
        conn.request("GET", path, {}, headers)
        response = conn.getresponse()
        json_obj = json.loads(response.read())
        first_device = json_obj[0]
        return first_device["device_id"]

    def setup_class(self):
        """
        Sets up class by registering an emulator device, enabling NFC, and loading snippets.

        If a PN532 serial path is found, it uses this to configure the device. Otherwise, set up a
        second phone as a reader device.
        """
        self.pn532 = None

        # This tracks the error message for a setup failure.
        # It is set to None only if the entire setup_class runs successfully.
        self._setup_failure_reason = 'Failed to find Android device(s).'

        # Indicates if the setup failure should block (FAIL) or not block (SKIP) test cases.
        # Blocking failures indicate that something unexpectedly went wrong during test setup,
        # and the user should have it fixed.
        # Non-blocking failures indicate that the device(s) did not meet the test requirements,
        # and the test does not need to be run.
        self._setup_failure_should_block_tests = True

        try:
            devices = self.register_controller(android_device)[:1]
            self.emulator = devices[0]

            self._setup_failure_reason = (
                'Cannot load emulator snippet. Is NfcEmulatorTestApp.apk '
                'installed on the emulator?'
            )
            self.emulator.load_snippet(
                'nfc_emulator', 'com.android.nfc.emulator'
            )
            self.emulator.adb.shell(['svc', 'nfc', 'enable'])
            self.emulator.debug_tag = 'emulator'
            if not self.emulator.nfc_emulator.isNfcHceSupported():
                self._setup_failure_reason = f'NFC HCE is not supported on {self.emulator}'
                self._setup_failure_should_block_tests = False
                return

            if (
                hasattr(self.emulator, 'dimensions')
                and 'pn532_serial_path' in self.emulator.dimensions
            ):
                pn532_serial_path = self.emulator.dimensions["pn532_serial_path"]
            else:
                pn532_serial_path = self.user_params.get("pn532_serial_path", "")

            casimir_id = None
            if self._is_cuttlefish_device(self.emulator):
                self._setup_failure_reason = ('Failed to set up casimir connection for Cuttlefish '
                                              'device')
                casimir_id = self._get_casimir_id_for_device()

            if casimir_id is not None and len(casimir_id) > 0:
                self._setup_failure_reason = 'Failed to connect to casimir'
                _LOG.info("casimir_id = " + casimir_id)
                self.pn532 = pn532.Casimir(casimir_id)
            else:
                self._setup_failure_reason = 'Failed to connect to PN532 board.'
                self.pn532 = pn532.PN532(pn532_serial_path)
                self.pn532.mute()

        except Exception as e:
            _LOG.warning('setup_class failed with error %s', e)
            return
        self._setup_failure_reason = None

    def setup_test(self):
        """
        Turns emulator screen on and unlocks between tests as some tests will turn the screen off.
        """
        if self._setup_failure_should_block_tests:
            asserts.assert_true(
                self._setup_failure_reason is None, self._setup_failure_reason
            )
        else:
            asserts.skip_if(
                self._setup_failure_reason is not None, self._setup_failure_reason
            )

        self.emulator.nfc_emulator.logInfo("*** TEST START: " + self.current_test_info.name +
                                           " ***")
        self.emulator.nfc_emulator.turnScreenOn()
        self.emulator.nfc_emulator.pressMenu()

    def on_fail(self, record):
        if self.user_params.get('take_bug_report_on_fail', False):
            test_name = record.test_name
            if hasattr(self, 'emulator') and hasattr(self.emulator, 'nfc_emulator'):
                self.emulator.take_bug_report(
                    test_name=self.emulator.debug_tag + "_" + test_name,
                    destination=self.current_test_info.output_path,
                )

    @CddTest(requirements = ["7.4.4/C-2-2", "7.4.4/C-1-2"])
    def test_single_non_payment_service(self):
        """Tests successful APDU exchange between non-payment service and
        reader.

        Test Steps:
        1. Start emulator activity and set up non-payment HCE Service.
        2. Set callback handler on emulator for when a TestPass event is
        received.
        3. Set up PN532, which should trigger APDU exchange.

        Verifies:
        1. Verifies a successful APDU exchange.
        """
        self._set_up_emulator(
            service_list=[_TRANSPORT_SERVICE_1],
            expected_service=_TRANSPORT_SERVICE_1
        )

        self._set_up_reader_and_assert_transaction(expected_service=_TRANSPORT_SERVICE_1)

    @CddTest(requirements = ["7.4.4/C-2-2", "7.4.4/C-1-2", "9.1/C-0-1"])
    def test_single_payment_service(self):
        """Tests successful APDU exchange between payment service and
        reader.

        Test Steps:
        1. Set callback handler on emulator for when the instrumentation app is
        set to default wallet app.
        2. Start emulator activity and wait for the app to hold the wallet role.
        3. Start PN532 reader, which should trigger APDU exchange between
        reader and emulator.

        Verifies:
        1. Verifies emulator device sets the instrumentation emulator app to the
        default wallet app.
        2. Verifies a successful APDU exchange.
        """
        self._set_up_emulator(
            service_list=[_PAYMENT_SERVICE_1],
            expected_service=_PAYMENT_SERVICE_1,
            is_payment=True,
            payment_default_service=_PAYMENT_SERVICE_1
        )

        self._set_up_reader_and_assert_transaction(expected_service=_PAYMENT_SERVICE_1)

    @CddTest(requirements = ["7.4.4/C-2-2", "7.4.4/C-1-2", "9.1/C-0-1"])
    def test_single_payment_service_after_reboot(self):
        """Tests successful APDU exchange between payment service and
        reader after a reboot.

        Test Steps:
        1. Set callback handler on emulator for when the instrumentation app is
        set to default wallet app.
        2. Reboot the emulator device.
        3. Start emulator activity and wait for the app to hold the wallet role.
        4. Start PN532 reader, which should trigger APDU exchange between
        reader and emulator.

        Verifies:
        1. Verifies emulator device sets the instrumentation emulator app to the
        default wallet app.
        2. Verifies a successful APDU exchange after reboot.
        """
        # Set the role before rebooting and ensure it remains enabled after
        # reboot to ensure that the NFC stack binds to it at bootup.
        self._set_up_emulator(
            service_list=[_PAYMENT_SERVICE_1],
            expected_service=_PAYMENT_SERVICE_1,
            is_payment=True, # Set the role holder before reboot.
            payment_default_service=_PAYMENT_SERVICE_1,
            should_disable_services_on_destroy=False # Don't disable services on shutdown.
        )
        self._reboot(self.emulator)
        # Setup the payment service activity to handle the transaction after
        # reboot.
        self._set_up_emulator(
            service_list=[_PAYMENT_SERVICE_1],
            expected_service=_PAYMENT_SERVICE_1,
            is_payment=False, # Don't set the role to ensure that state is persisted across reboot.
            payment_default_service=_PAYMENT_SERVICE_1
        )
        self._set_up_reader_and_assert_transaction(expected_service=_PAYMENT_SERVICE_1)


    @CddTest(requirements = ["7.4.4/C-2-2", "7.4.4/C-1-2", "9.1/C-0-1"])
    def test_single_payment_service_with_background_app(self):
        """Tests successful APDU exchange between payment service and
        reader.

        Test Steps:
        1. Start emulator activity and wait for the app to hold the wallet role.
        2. Set callback handler on emulator for when a TestPass event is
        received.
        3. Move emulator activity to the background by sending a Home key event.
        4. Start PN532, which should trigger APDU exchange with the emulator.

        Verifies:
        1. Verifies emulator device sets the instrumentation emulator app to the
        default wallet app.
        2. Verifies a successful APDU exchange.
        """
        self._set_up_emulator(
            service_list=[_PAYMENT_SERVICE_1],
            expected_service=_PAYMENT_SERVICE_1,
            is_payment=True,
            payment_default_service=_PAYMENT_SERVICE_1
        )

        self.emulator.nfc_emulator.pressHome()

        self._set_up_reader_and_assert_transaction(expected_service=_PAYMENT_SERVICE_1)

    def test_single_payment_service_crashes(self):
        """Tests successful APDU exchange between payment service and
        reader.

        Test Steps:
        1. Set callback handler on emulator for when the instrumentation app is
        set to default wallet app.
        2. Start emulator activity and wait for the role to be set.
        2. Set callback handler on emulator for when a TestPass event is
        received.
        3. Start PN532, which should trigger APDU exchange with the emulator.

        Verifies:
        1. Verifies emulator device sets the instrumentation emulator app to the
        default wallet app.
        2. Verifies a successful APDU exchange.
        """
        self._set_up_emulator(
            service_list=[_PAYMENT_SERVICE_1],
            expected_service=_PAYMENT_SERVICE_1,
            is_payment=True,
            payment_default_service=_PAYMENT_SERVICE_1
        )

        ps = (self.emulator.adb.shell(["ps", "|", "grep", "com.android.nfc.emulator.payment"])
              .decode("utf-8"))
        pid = ps.split()[1]
        try:
            self.emulator.adb.shell(["kill", "-9", pid])
        except adb.AdbError:
            _LOG.info(f"Could not kill pid {pid} through adb.")
            self.emulator.nfc_emulator.killProcess(pid)

        self._set_up_reader_and_assert_transaction(expected_service=_PAYMENT_SERVICE_1)

    @CddTest(requirements = ["7.4.4/C-2-2", "7.4.4/C-1-2", "9.1/C-0-1"])
    def test_dual_payment_service(self):
        """Tests successful APDU exchange between a payment service and
        reader when two payment services are set up on the emulator.

        Test Steps:
        1. Start emulator activity and wait for the role to be set.
        2. Set callback handler on emulator for when a TestPass event is
        received.
        3. Start reader activity, which should trigger APDU exchange between
        reader and emulator.

        Verifies:
        1. Verifies a successful APDU exchange.
        """
        self._set_up_emulator(
            service_list=[_PAYMENT_SERVICE_1,_PAYMENT_SERVICE_2],
            expected_service=_PAYMENT_SERVICE_1,
            is_payment=True,
            payment_default_service=_PAYMENT_SERVICE_1
        )

        self._set_up_reader_and_assert_transaction(expected_service=_PAYMENT_SERVICE_1)

    @CddTest(requirements = ["7.4.4/C-2-2", "7.4.4/C-1-2", "9.1/C-0-1"])
    def test_foreground_payment_emulator(self):
        """Tests successful APDU exchange between non-default payment service and
        reader when the foreground app sets a preference for the non-default
        service.

        Test Steps:
        1. Start emulator activity and wait for the role to be set.
        2. Set callback handler on emulator for when a TestPass event is
        received.
        3. Start reader activity, which should trigger APDU exchange between
        reader and emulator.

        Verifies:
        1. Verifies a successful APDU exchange.
        """
        self._set_up_emulator(
            service_list=[_PAYMENT_SERVICE_1, _PAYMENT_SERVICE_2],
            preferred_service=_PAYMENT_SERVICE_2,
            expected_service=_PAYMENT_SERVICE_2,
            is_payment=True,
            payment_default_service=_PAYMENT_SERVICE_2
        )

        self._set_up_reader_and_assert_transaction(expected_service=_PAYMENT_SERVICE_2)

    @CddTest(requirements = ["7.4.4/C-2-2", "7.4.4/C-1-2"])
    def test_dynamic_aid_emulator(self):
        """Tests successful APDU exchange between payment service and reader
        when the payment service has registered dynamic AIDs.

        Test Steps:
        1. Start emulator activity and wait for the role to be set.
        2. Set callback handler on emulator for when a TestPass event is
        received.
        3. Start reader activity, which should trigger APDU exchange between
        reader and emulator.

        Verifies:
        1. Verifies a successful APDU exchange.
        """
        self._set_up_emulator(
            start_emulator_fun=self.emulator.nfc_emulator.startDynamicAidEmulatorActivity,
            payment_default_service=_PAYMENT_SERVICE_DYNAMIC_AIDS
        )

        self._set_up_reader_and_assert_transaction(expected_service=_PAYMENT_SERVICE_DYNAMIC_AIDS)

    @CddTest(requirements = ["7.4.4/C-2-2", "7.4.4/C-1-2", "9.1/C-0-1"])
    def test_payment_prefix_emulator(self):
        """Tests successful APDU exchange between payment service and reader
        when the payment service has statically registered prefix AIDs.

        Test Steps:
        1. Start emulator activity and wait for the role to be set.
        2. Set callback handler on emulator for when a TestPass event is
        received.
        3. Start reader activity, which should trigger APDU exchange between
        reader and emulator.

        Verifies:
        1. Verifies a successful APDU exchange.
        """
        asserts.skip_if(not self.emulator.nfc_emulator.isAidPrefixRegistrationSupported(),
            "Prefix registration is not supported on device")
        self._set_up_emulator(
            start_emulator_fun=self.emulator.nfc_emulator.startPrefixPaymentEmulatorActivity,
            payment_default_service=_PREFIX_PAYMENT_SERVICE_1,
            is_payment=True
        )

        self._set_up_reader_and_assert_transaction(expected_service=_PREFIX_PAYMENT_SERVICE_1)

    @CddTest(requirements = ["7.4.4/C-2-2", "7.4.4/C-1-2", "9.1/C-0-1"])
    def test_prefix_payment_emulator_2(self):
        """Tests successful APDU exchange between payment service and reader
        when the payment service has statically registered prefix AIDs.
        Identical to the test above, except PrefixPaymentService2 is set up
        first in the emulator activity.

        Test Steps:
        1. Start emulator activity and wait for the role to be set.
        2. Set callback handler on emulator for when a TestPass event is
        received.
        3. Start reader activity, which should trigger APDU exchange between
        reader and emulator.

        Verifies:
        1. Verifies a successful APDU exchange.
        """
        asserts.skip_if(not self.emulator.nfc_emulator.isAidPrefixRegistrationSupported(),
            "Prefix registration is not supported on device")
        self._set_up_emulator(
            start_emulator_fun=self.emulator.nfc_emulator.startPrefixPaymentEmulator2Activity,
            payment_default_service=_PREFIX_PAYMENT_SERVICE_1,
            is_payment=True
        )

        self._set_up_reader_and_assert_transaction(expected_service=_PREFIX_PAYMENT_SERVICE_1)

    @CddTest(requirements = ["7.4.4/C-2-2", "7.4.4/C-1-2"])
    def test_other_prefix(self):
        """Tests successful APDU exchange when the emulator dynamically
        registers prefix AIDs for a non-payment service.

        Test steps:
        1. Start emulator activity.
        2. Set callback handler on emulator for when ApduSuccess event is
        received.
        3. Start reader activity, which should trigger APDU exchange between
        reader and emulator.

        Verifies:
        1. Verifies successful APDU sequence exchange.

        """
        asserts.skip_if(not self.emulator.nfc_emulator.isAidPrefixRegistrationSupported(),
            "Prefix registration is not supported on device")
        self._set_up_emulator(
            start_emulator_fun=self.emulator.nfc_emulator.startDualNonPaymentPrefixEmulatorActivity)

        self._set_up_reader_and_assert_transaction(expected_service=_PREFIX_ACCESS_SERVICE)

    @CddTest(requirements = ["7.4.4/C-2-2", "7.4.4/C-1-2"])
    def test_offhost_service(self):
        """Tests successful APDU exchange between offhost service and reader.

        Test Steps:
        1. Start emulator activity.
        2. Set callback handler for when reader TestPass event is received.
        3. Start reader activity, which should trigger APDU exchange between
        reader and emulator.

        Verifies:
        1. Verifies a successful APDU exchange.
        """
        self._set_up_emulator(
            False, start_emulator_fun=self.emulator.nfc_emulator.startOffHostEmulatorActivity)

        self._set_up_reader_and_assert_transaction(expected_service=_OFFHOST_SERVICE)

    @CddTest(requirements = ["7.4.4/C-2-2", "7.4.4/C-1-2"])
    def test_on_and_offhost_service(self):
        """Tests successful APDU exchange between when reader selects both an on-host and off-host
        service.

        Test Steps:
        1. Start emulator activity.
        2. Set callback handler for when reader TestPass event is received.
        3. Start reader activity, which should trigger APDU exchange between
        reader and emulator.

        Verifies:
        1. Verifies a successful APDU exchange.
        """
        self._set_up_emulator(
            start_emulator_fun=self.emulator.nfc_emulator.startOnAndOffHostEmulatorActivity)

        self._set_up_reader_and_assert_transaction(expected_service=_TRANSPORT_SERVICE_1)

    @CddTest(requirements = ["7.4.4/C-2-2", "7.4.4/C-1-2"])
    def test_dual_non_payment(self):
        """Tests successful APDU exchange between transport service and reader
        when two non-payment services are enabled.

        Test Steps:
        1. Start emulator activity which sets up TransportService2 and
        AccessService.
        2. Start PN532, which should trigger APDU exchange between
        reader and emulator.

        Verifies:
        1. Verifies a successful APDU exchange.
        """
        self._set_up_emulator(
            service_list=[_TRANSPORT_SERVICE_2, _ACCESS_SERVICE],
            expected_service=_TRANSPORT_SERVICE_2,
            is_payment=False
        )

        self._set_up_reader_and_assert_transaction(expected_service = _TRANSPORT_SERVICE_2)

    @CddTest(requirements = ["7.4.4/C-2-2", "7.4.4/C-1-2"])
    def test_foreground_non_payment(self):
        """Tests successful APDU exchange between non-payment service and
          reader when the foreground app sets a preference for the
          non-default service.

          Test Steps:
          1. Start emulator activity which sets up TransportService1 and
          TransportService2
          2. Start PN532, which should trigger APDU exchange between
          reader and non-default service.

          Verifies:
          1. Verifies a successful APDU exchange.
          """
        self._set_up_emulator(
            service_list=[_TRANSPORT_SERVICE_1, _TRANSPORT_SERVICE_2],
            preferred_service=_TRANSPORT_SERVICE_2,
            expected_service=_TRANSPORT_SERVICE_2,
            is_payment=False
        )

        self._set_up_reader_and_assert_transaction(
            expected_service=_TRANSPORT_SERVICE_2)

    @CddTest(requirements = ["7.4.4/C-2-2", "7.4.4/C-1-2"])
    def test_throughput(self):
        """Tests that APDU sequence exchange occurs with under 60ms per APDU.

         Test Steps:
         1. Start emulator activity.
         2. Set callback handler on emulator for when a TestPass event is
         received.
         3. Start reader activity, which should trigger APDU exchange between
         reader and non-default service.

         Verifies:
         1. Verifies a successful APDU exchange between the emulator and the
         transport service with the duration averaging under 60 ms per single
         exchange.
         """
        asserts.skip_if("_cf_x86_" in self.emulator.adb.getprop("ro.product.name"),
                        "Skipping throughput test on Cuttlefish")
        self.emulator.nfc_emulator.startThroughputEmulatorActivity()
        test_pass_handler = self.emulator.nfc_emulator.asyncWaitForTestPass(
            'ApduUnderThreshold')
        command_apdus, response_apdus = get_apdus(self.emulator.nfc_emulator,
                                                  _THROUGHPUT_SERVICE)
        poll_and_transact(self.pn532, command_apdus, response_apdus)

        test_pass_handler.waitAndGet('ApduUnderThreshold', _NFC_TIMEOUT_SEC)

    @CddTest(requirements = ["7.4.4/C-2-2", "7.4.4/C-1-2"])
    def test_tap_50_times(self):
        """Tests that 50 consecutive APDU exchanges are successful.

        Test Steps:
         1. Start emulator activity.
         2. Perform the following sequence 50 times:
            a. Set callback handler on emulator for when a TestPass event is
            received.
            b. Start PN532.
            c. Wait for successful APDU exchange.
            d. Close reader activity.

         Verifies:
         1. Verifies 50 successful APDU exchanges.
         """
        self._set_up_emulator(
            service_list=[_TRANSPORT_SERVICE_1],
            expected_service=_TRANSPORT_SERVICE_1
        )

        command_apdus, response_apdus = get_apdus(self.emulator.nfc_emulator,
                                                  _TRANSPORT_SERVICE_1)
        for i in range(50):
            tag_detected, transacted = poll_and_transact(self.pn532, command_apdus,
                                                         response_apdus)
            asserts.assert_true(
                tag_detected, _FAILED_TAG_MSG
            )
            asserts.assert_true(transacted, _FAILED_TRANSACTION_MSG)

    @CddTest(requirements = ["7.4.4/C-2-2", "7.4.4/C-1-2"])
    def test_large_num_aids(self):
        """Tests that a long APDU sequence (256 commands/responses) is
        successfully exchanged.

        Test Steps:
         1. Start emulator activity.
         2. Set callback handler on emulator for when a TestPass event is
         received.
         3. Start reader activity.
         4. Wait for successful APDU exchange.

         Verifies:
         1. Verifies successful APDU exchange.
         """
        self._set_up_emulator(
            start_emulator_fun=self.emulator.nfc_emulator.startLargeNumAidsEmulatorActivity
        )

        self._set_up_reader_and_assert_transaction(expected_service=_LARGE_NUM_AIDS_SERVICE)

    @CddTest(requirements = ["7.4.4/C-2-2", "7.4.4/C-1-2"])
    def test_screen_off_payment(self):
        """Tests that APDU exchange occurs when device screen is off.

        Test Steps:
        1. Start emulator activity and wait for the role to be set.
        2. Turn emulator screen off.
        3. Start PN532, which should trigger successful APDU exchange.

        Verifies:
        1. Verifies default wallet app is set.
        2. Verifies screen is turned off on the emulator.
        3. Verifies successful APDU exchange with emulator screen off.
        """
        self._set_up_emulator(
            start_emulator_fun=self.emulator.nfc_emulator.startScreenOffPaymentEmulatorActivity,
            payment_default_service=_SCREEN_OFF_PAYMENT_SERVICE,
            is_payment=True
        )

        screen_off_handler = self.emulator.nfc_emulator.asyncWaitForScreenOff(
            'ScreenOff')
        self.emulator.nfc_emulator.turnScreenOff()
        screen_off_handler.waitAndGet('ScreenOff', _NFC_TIMEOUT_SEC)

        self._set_up_reader_and_assert_transaction(expected_service=_SCREEN_OFF_PAYMENT_SERVICE)

    @CddTest(requirements = ["7.4.4/C-2-2", "7.4.4/C-1-2"])
    def test_conflicting_non_payment(self):
        """ This test registers two non-payment services with conflicting AIDs,
        selects a service to use, and ensures the selected service exchanges
        an APDU sequence with the reader.

        Test Steps:
        1. Start emulator.
        2. Start PN532 reader, which should trigger a popup screen of NFC services to select from.
        3. Select a service on the emulator device from the list of services.
        4. Set a callback handler on the emulator for a successful APDU
        exchange.
        6. Set up reader for transaction, which should trigger the APDU
        exchange with the selected service.

        Verifies:
        1. Verifies APDU exchange is successful between the reader and the
        selected service.
        """
        self._set_up_emulator(service_list=[_TRANSPORT_SERVICE_1,_TRANSPORT_SERVICE_2],
                              expected_service=_TRANSPORT_SERVICE_2, is_payment=False)
        command_apdus, response_apdus = get_apdus(self.emulator.nfc_emulator, _TRANSPORT_SERVICE_2)
        poll_and_transact(self.pn532, command_apdus[:1], response_apdus[:1])

        self.emulator.nfc_emulator.selectItem()

        tag_detected, transacted = poll_and_transact(self.pn532, command_apdus, response_apdus)
        asserts.assert_true(
            tag_detected, _FAILED_TAG_MSG
        )
        asserts.assert_true(transacted, _FAILED_TRANSACTION_MSG)


    @CddTest(requirements = ["7.4.4/C-2-2", "7.4.4/C-1-2"])

    @ApiTest(
            apis = {
                "android.nfc.cardemulation.CardEmulation.NfcEventCallback#onAidConflictOccurred"
            })
    def test_conflicting_non_payment_prefix(self):
        """ This test registers two non-payment services with conflicting
        prefix AIDs, selects a service to use, and ensures the selected
        service exchanges an APDU sequence with the reader.

        Test Steps:
        1. Start emulator.
        2. Start PN532.
        3. Select a service on the emulator device from the list of services.
        4. Disable polling on the reader.
        5. Re-enable polling on the reader, which should trigger the APDU
        exchange with the selected service.

        Verifies:
        1. Verifies APDU exchange is successful between the reader and the
        selected service.
        """
        asserts.skip_if(not self.emulator.nfc_emulator.isAidPrefixRegistrationSupported(),
            "Prefix registration is not supported on device")
        self._set_up_emulator(
            start_emulator_fun=
                self.emulator.nfc_emulator.startConflictingNonPaymentPrefixEmulatorActivity,
            is_payment=False
        )
        command_apdus, response_apdus = get_apdus(self.emulator.nfc_emulator,
                                                  _PREFIX_TRANSPORT_SERVICE_2)
        test_pass_handler = self.emulator.nfc_emulator.asyncWaitForTestPass(
            'ApduSuccess'
        )
        poll_and_transact(self.pn532, command_apdus[:1], response_apdus[:1])

        self.emulator.nfc_emulator.selectItem()
        tag_detected, transacted = poll_and_transact(self.pn532, command_apdus, response_apdus)
        asserts.assert_true(tag_detected, _FAILED_TAG_MSG)
        asserts.assert_true(transacted, _FAILED_TRANSACTION_MSG)

        test_pass_handler.waitAndGet('ApduSuccess', _NFC_TIMEOUT_SEC)

    #@CddTest(requirements = {"TODO"})
    @ApiTest(
            apis = {
                "android.nfc.cardemulation.CardEmulation.NfcEventCallback#onAidNotRouted",
                "android.nfc.cardemulation.CardEmulation.NfcEventCallback#onRemoteFieldChanged"
            })
    def test_event_listener(self):
        """ This test registers an event listener with the emulator and ensures
        that the event listener receives callbacks when the field status changes and
        when an AID goes unrouted.

        Test Steps:
        1. Start the emulator.
        2. Start PN532.
        3. Select an unrouted AID on the emulator.
        4. Select a routed AID on the emulator.

        Verifies:
        1. Verifies that the event listener receives callbacks when the field
        status changes and when an AID goes unrouted.
        """
        self._set_up_emulator(
            start_emulator_fun=self.emulator.nfc_emulator.startEventListenerActivity,
            is_payment=False
        )

        # unrouted AID
        command_apdus, response_apdus = get_apdus(self.emulator.nfc_emulator,
                                                    _ACCESS_SERVICE)
        test_pass_handler = self.emulator.nfc_emulator.asyncWaitForTestPass(
            'EventListenerSuccess'
        )
        tag_detected, transacted = poll_and_transact(self.pn532, command_apdus, response_apdus)
        asserts.assert_true(tag_detected, _FAILED_TAG_MSG)
        asserts.assert_false(transacted, _FAILED_TRANSACTION_MSG)

        # routed AID
        command_apdus, response_apdus = get_apdus(self.emulator.nfc_emulator,
                                                    _TRANSPORT_SERVICE_1)
        tag_detected, transacted = poll_and_transact(self.pn532, command_apdus, response_apdus)
        asserts.assert_true(tag_detected, _FAILED_TAG_MSG)
        asserts.assert_true(transacted, _FAILED_TRANSACTION_MSG)
        test_pass_handler.waitAndGet('EventListenerSuccess', _NFC_TIMEOUT_SEC)


    @CddTest(requirements = ["7.4.4/C-2-2", "7.4.4/C-1-2"])
    def test_protocol_params(self):
        """ Tests that the Nfc-A and ISO-DEP protocol parameters are being
        set correctly.

        Test Steps:
        1. Start emulator.
        2. Start PN532.
        4. Wait for success event to be sent.

        Verifies:
        1. Verifies Nfc-A and ISO-DEP protocol parameters are set correctly.
        """
        success = False
        self._set_up_emulator(
            service_list=[],
            expected_service=""
        )

        for i in range(_NUM_POLLING_LOOPS):
            tag = self.pn532.poll_a()
            msg = None
            if tag is not None:
                success, msg = parse_protocol_params(tag.sel_res, tag.ats)
                self.pn532.mute()
                break
            self.pn532.mute()
        asserts.assert_true(success, msg if msg is not None else _FAILED_TAG_MSG)


    @CddTest(requirements = ["7.4.4/C-2-2", "7.4.4/C-1-2"])
    def test_screen_on_only_off_host_service(self):
        """
        Test Steps:
        1. Start emulator.
        2. Turn screen off.
        3. Start PN532, and ensure expected APDU exchange occurs.
        4. Turn screen off.
        5. Start PN532, and ensure expected APDU exchange occurs.


        Verifies:
        1. Verifies correct APDU response when screen is off.
        2. Verifies correct APDU response between reader and off-host service
        when screen is on.
        """
        #Tests APDU exchange with screen off.
        self._set_up_emulator(
            start_emulator_fun=self.emulator.nfc_emulator.startScreenOnOnlyOffHostEmulatorActivity
        )
        self.emulator.nfc_emulator.turnScreenOff()
        screen_off_handler = self.emulator.nfc_emulator.asyncWaitForScreenOff(
            'ScreenOff')
        screen_off_handler.waitAndGet('ScreenOff', _NFC_TIMEOUT_SEC)

        self._set_up_reader_and_assert_transaction(
            expected_service=_SCREEN_ON_ONLY_OFF_HOST_SERVICE)

        self.pn532.mute()

        #Tests APDU exchange with screen on.
        screen_on_handler = self.emulator.nfc_emulator.asyncWaitForScreenOn(
            'ScreenOn')
        self.emulator.nfc_emulator.pressMenu()
        screen_on_handler.waitAndGet('ScreenOn', _NFC_TIMEOUT_SEC)

        self._set_up_reader_and_assert_transaction(expected_service=
                                                   _SCREEN_ON_ONLY_OFF_HOST_SERVICE)

    def test_single_payment_service_toggle_nfc_off_on(self):
        """Tests successful APDU exchange between payment service and
        reader.

        Test Steps:
        1. Start emulator activity and wait for the role to be set.
        2. Toggle NFC off and back on the emulator.
        3. Set callback handler on emulator for when a TestPass event is
        received.
        4. Start reader activity, which should trigger APDU exchange between
        reader and emulator.

        Verifies:
        1. Verifies emulator device sets the instrumentation emulator app to the
        default wallet app.
        2. Verifies a successful APDU exchange after toggling NFC off and on.
        """
        # Wait for instrumentation app to hold onto wallet role before starting
        # reader
        self._set_up_emulator(
            service_list=[_PAYMENT_SERVICE_1],
            expected_service=_PAYMENT_SERVICE_1,
            is_payment=True,
            payment_default_service=_PAYMENT_SERVICE_1
        )

        self.emulator.nfc_emulator.setNfcState(False)
        self.emulator.nfc_emulator.setNfcState(True)

        self._set_up_reader_and_assert_transaction(expected_service=_PAYMENT_SERVICE_1)

    @CddTest(requirements = ["7.4.4/C-1-13"])
    def test_polling_frame_timestamp(self):
        """Tests that PollingFrame object timestamp values are reported correctly
        and do not deviate from host measurements

        Test Steps:
        1. Toggle NFC reader field OFF
        2. Start emulator activity
        3. Perform a polling loop, wait for field OFF event.
        4. Collect polling frames. Iterate over matching polling loop frame
        and device time measurements. Calculate elapsed time for each and verify
        that the host-device difference does not exceed the delay threshold.

        Verifies:
        1. Verifies that timestamp values are reported properly
        for each tested frame type.
        2. Verifies that the difference between matching host and device
        timestamps does not exceed _POLLING_FRAME_TIMESTAMP_TOLERANCE_MS.
        """
        asserts.skip_if(not self.emulator.nfc_emulator.isObserveModeSupported(),
            "Skipping polling frame timestamp test, observe mode not supported")

        # 1. Mute the field before starting the emulator
        # in order to be able to trigger ON event when the test starts
        self.pn532.mute()

        # 2. Start emulator activity
        self._set_up_emulator(
            start_emulator_fun=self.emulator.nfc_emulator.startPollingFrameEmulatorActivity
        )

        timed_pn532 = TimedWrapper(self.pn532)
        testcases = [
            POLLING_FRAME_ON,
            *POLLING_FRAMES_TYPE_A_SPECIAL,
            *POLLING_FRAMES_TYPE_A_SPECIAL,
            *POLLING_FRAMES_TYPE_A_LONG,
            *POLLING_FRAMES_TYPE_A_LONG,
            *POLLING_FRAMES_TYPE_B_SPECIAL,
            *POLLING_FRAMES_TYPE_B_SPECIAL,
            *POLLING_FRAMES_TYPE_B_LONG,
            *POLLING_FRAMES_TYPE_B_LONG,
            *POLLING_FRAMES_TYPE_F_SPECIAL,
            *POLLING_FRAMES_TYPE_F_SPECIAL,
            POLLING_FRAME_OFF,
        ]
        # 3. Transmit polling frames
        frames = poll_and_observe_frames(
            pn532=timed_pn532,
            emulator=self.emulator.nfc_emulator,
            testcases=testcases,
            restore_original_frame_ordering=True
        )
        timings = timed_pn532.timings

        # Pre-format data for error if one happens
        frame_stats = get_frame_test_stats(
            frames=frames,
            testcases=testcases,
            timestamps=[ns_to_us(timestamp) for (_, timestamp) in timings]
        )

        # Check that there are as many polling loop events as frames sent
        asserts.assert_equal(
            len(testcases), len(frames),
            _FAILED_MISSING_POLLING_FRAMES_MSG,
            frame_stats
        )

        # For each event, calculate the amount of time elapsed since the previous one
        # Subtract the resulting host/device time delta values
        # Verify that the difference does not exceed the threshold
        previous_timestamp_device = None
        first_timestamp_start, first_timestamp_end = timings[0]
        first_timestamp = (first_timestamp_start + first_timestamp_end) / 2
        first_timestamp_error = (first_timestamp_end - first_timestamp_start)/ 2
        first_timestamp_device = frames[0].timestamp

        num_exceeding_threshold = 0
        for idx, (frame, timing, testcase) in enumerate(zip(frames, timings, testcases)):
            timestamp_host_start, timestamp_host_end = timing
            timestamp_host = (timestamp_host_start + timestamp_host_end) / 2
            timestamp_error = (timestamp_host_end - timestamp_host_start) / 2
            timestamp_device = frame.timestamp

            _LOG.debug(
               f"{testcase.data.upper():32}" + \
               f" ({testcase.configuration.type}" + \
               f", {'+' if testcase.configuration.crc else '-'}" + \
               f", {testcase.configuration.bits})" + \
               f" -> {frame.data.hex().upper():32} ({frame.type})"
            )

            pre_previous_timestamp_device = previous_timestamp_device
            previous_timestamp_device = timestamp_device

            # Skip cases when timestamp value wraps
            # as there's no way to establish the delta
            # and re-establish the baselines
            if (timestamp_device - first_timestamp_device) < 0:
                _LOG.warning(
                    "Timestamp value wrapped" + \
                    f" from {pre_previous_timestamp_device}" + \
                    f" to {previous_timestamp_device} for frame {frame};" + \
                    " Skipping comparison..."
                )
                first_timestamp_device = timestamp_device
                first_timestamp = timestamp_host
                first_timestamp_error = timestamp_error
                continue

            device_host_difference = us_to_ms(timestamp_device - first_timestamp_device) - \
                ns_to_ms(timestamp_host - first_timestamp)
            total_error = ns_to_ms(timestamp_error + first_timestamp_error)
            if abs(device_host_difference) > _POLLING_FRAME_TIMESTAMP_TOLERANCE_MS + total_error:
                debug_info = {
                    "index": idx,
                    "frame_sent": testcase.format_for_error(timestamp=ns_to_us(timestamp_host)),
                    "frame_received": frame.to_dict(),
                    "difference": device_host_difference,
                    "time_device": us_to_ms(timestamp_device - first_timestamp_device),
                    "time_host": ns_to_ms(timestamp_host - first_timestamp),
                    "total_error": total_error,
                }
                num_exceeding_threshold = num_exceeding_threshold + 1
                _LOG.warning(f"Polling frame timestamp tolerance exceeded: {debug_info}")
        asserts.assert_less(num_exceeding_threshold,
                                  _POLLING_FRAME_TIMESTAMP_EXCEED_COUNT_TOLERANCE_)

    @CddTest(requirements = ["7.4.4/C-1-13"])
    def test_polling_frame_vendor_specific_gain(self):
        """Tests that PollingFrame object vendorSpecificGain value
        changes when overall NFC reader output power changes

        Test Steps:
        1. Toggle NFC reader field OFF
        2. Start emulator activity
        3. For each power level (0-100% with 20% step), send polling loop
        consisting of normally encountered polling frames
        4. For each result, calculate average vendorSpecificGain per NFC mode
        compare those values against the previous power step, and assert that
        they are equal or bigger than the previous one

        Verifies:
        1. Verifies that vendorSpecificGain value increases or stays the same
        when PN532 output power increases.
        """
        asserts.skip_if(not self.emulator.nfc_emulator.isObserveModeSupported(),
                    "Skipping polling frame gain test, observe mode not supported")

        self.pn532.mute()
        emulator = self.emulator.nfc_emulator

        self._set_up_emulator(
            start_emulator_fun=emulator.startPollingFrameEmulatorActivity
        )

        # Loop two times so that HostEmulationManager releases all frames
        testcases = [
            POLLING_FRAME_ON,
            *POLLING_FRAMES_TYPE_A_SPECIAL,
            *POLLING_FRAMES_TYPE_B_SPECIAL,
            *POLLING_FRAMES_TYPE_F_SPECIAL,
            POLLING_FRAME_OFF
        ] * 2

        # 6 steps; 0%, 20%, 40%, 60%, 80%, 100%
        power_levels = [0, 1, 2, 3, 4, 5]
        polling_frame_types = ("A", "B", "F")

        results_for_power_level = {}

        for power_level in power_levels:
            # Warning for 0% might appear,
            # as it's possible for no events to be produced
            frames = poll_and_observe_frames(
                testcases=testcases,
                pn532=self.pn532,
                emulator=emulator,
                # Scale from 0 to 100%
                power_level = power_level * 20,
                ignore_field_off_event_timeout=power_level == 0
            )

            frames_for_type = {
                type_: [
                    f.vendor_specific_gain for f in frames if f.type == type_
                ] for type_ in polling_frame_types
            }
            results_for_power_level[power_level] = {
                # If no frames for type, assume gain is negative -1
                type_: (sum(frames) / len(frames)) if len(frames) else -1
                for type_, frames in frames_for_type.items()
            }

        _LOG.debug(f"Polling frame gain results {results_for_power_level}")

        issues = []
        for power_level in power_levels:
            # No value to compare to
            if power_level == 0:
                continue

            for type_ in polling_frame_types:
                previous_gain = results_for_power_level[power_level - 1][type_]
                current_gain = results_for_power_level[power_level][type_]
                if current_gain >= previous_gain:
                    continue
                sample = {
                    "type": type_,
                    "power_level": power_level * 20,
                    "previous_gain": previous_gain,
                    "current_gain": current_gain,
                }
                _LOG.warning(
                    f"Reported gain level dropped" + \
                    f" between power steps {sample}"
                )
                issues.append(sample)

        # Allow up to 2 reported gain decreases out of (5 * 3) = 15 test samples
        # Theoretically, this could happen
        # due to automatic power/gain/load management feature of chipsets
        asserts.assert_true(
            len(issues) <= 2,
            _FAILED_VENDOR_GAIN_VALUE_DROPPED_ON_POWER_INCREASE,
        )

    @CddTest(requirements = ["7.4.4/C-1-13"])
    def test_polling_frame_type(self):
        """Tests that PollingFrame object 'type' value is set correctly

        Test Steps:
        1. Toggle NFC reader field OFF
        2. Start emulator activity
        3. Perform a polling loop, wait for field OFF event.
        4. Collect polling frames. Iterate over sent and received frame pairs,
        verify that polling frame type matches.

        Verifies:
        1. Verifies that PollingFrame.type value is set correctly
        """
        asserts.skip_if(not self.emulator.nfc_emulator.isObserveModeSupported(),
                    "Skipping polling frame type test, observe mode not supported")
        self.pn532.mute()
        emulator = self.emulator.nfc_emulator

        self._set_up_emulator(
            start_emulator_fun=emulator.startPollingFrameEmulatorActivity
        )

        testcases = POLLING_FRAME_ALL_TEST_CASES

        # 3. Transmit polling frames
        frames = poll_and_observe_frames(
            pn532=self.pn532,
            emulator=self.emulator.nfc_emulator,
            testcases=testcases,
            restore_original_frame_ordering=True,
        )

        # Check that there are as many polling loop events as frames sent
        asserts.assert_equal(
            len(testcases), len(frames),
            _FAILED_MISSING_POLLING_FRAMES_MSG,
            get_frame_test_stats(frames=frames, testcases=testcases)
        )

        issues = [
            {
                "index": idx,
                "expected": testcase.success_types,
                "received": frame.type,
                "data": frame.data.hex(),
            } for idx, (testcase, frame) in enumerate(zip(testcases, frames))
            if frame.type not in testcase.success_types
        ]

        asserts.assert_equal(len(issues), 0, _FAILED_FRAME_TYPE_INVALID, issues)

    @CddTest(requirements = ["7.4.4/C-1-13"])
    def test_polling_frame_data(self):
        """Tests that PollingFrame object 'data' value is set correctly

        Test Steps:
        1. Toggle NFC reader field OFF
        2. Start emulator activity
        3. Perform a polling loop, wait for field OFF event.
        4. Collect polling frames. Iterate over sent and received frame pairs,
        verify that polling frame type matches.

        Verifies:
        1. Verifies that PollingFrame.data value is set correctly
        """
        asserts.skip_if(not self.emulator.nfc_emulator.isObserveModeSupported(),
                    "Skipping polling frame data test, observe mode not supported")
        self.pn532.mute()
        emulator = self.emulator.nfc_emulator

        self._set_up_emulator(
            start_emulator_fun=emulator.startPollingFrameEmulatorActivity
        )

        testcases = POLLING_FRAME_ALL_TEST_CASES

        # 3. Transmit polling frames
        frames = poll_and_observe_frames(
            pn532=self.pn532,
            emulator=self.emulator.nfc_emulator,
            testcases=testcases,
            restore_original_frame_ordering=True,
        )

        # Check that there are as many polling loop events as frames sent
        asserts.assert_equal(
            len(testcases), len(frames),
            _FAILED_MISSING_POLLING_FRAMES_MSG,
            get_frame_test_stats(frames=frames, testcases=testcases)
        )

        issues = [
            {
                "index": idx,
                "expected": testcase.expected_data,
                "received": frame.data.hex()
            } for idx, (testcase, frame) in enumerate(zip(testcases, frames))
            if frame.data.hex() not in testcase.expected_data
        ]

        for testcase, frame in zip(testcases, frames):
            if frame.data.hex() not in testcase.warning_data:
                continue
            _LOG.warning(
                f"Polling frame data variation '{frame.data.hex()}'" + \
                f" is accepted but not correct {testcase.success_data}"
            )

        asserts.assert_equal(len(issues), 0, _FAILED_FRAME_DATA_INVALID, issues)

    def teardown_test(self):
        if hasattr(self, 'emulator') and hasattr(self.emulator, 'nfc_emulator'):
            self.emulator.nfc_emulator.closeActivity()
            self.emulator.nfc_emulator.logInfo(
                "*** TEST END: " + self.current_test_info.name + " ***")
        if hasattr(self, 'pn532'):
            self.pn532.reset_buffers()
            self.pn532.mute()
        if hasattr(self, 'emulator'):
            param_list = [[self.emulator]]
            utils.concurrent_exec(lambda d: d.services.create_output_excerpts_all(
                self.current_test_info),
                                  param_list=param_list,
                                  raise_on_exception=True)

    #@CddTest(requirements = {"7.4.4/C-2-2", "7.4.4/C-1-2"})
    def test_single_non_payment_service_with_listen_tech_disabled(self):
        """Tests successful APDU exchange between non-payment service and
        reader does not proceed when Type-a listen tech is disabled.

        Test Steps:
        1. Start emulator activity and set up non-payment HCE Service.
        2. Set listen tech to disabled on the emulator.
        3. Start PN532 and verify transaction does not proceed.
        5. Set listen tech to Type-A on the emulator.
        6. Start PN532 and verify APDU exchange between reader and emulator.

        Verifies:
        1. Verifies that no APDU exchange occurs when the listen tech is disabled.
        2. Verifies a successful APDU exchange after re-enabling.
        """
        self._set_up_emulator(service_list=[_TRANSPORT_SERVICE_1],
                              expected_service=_TRANSPORT_SERVICE_1, is_payment=False)
        self.emulator.nfc_emulator.setListenTech(_NFC_LISTEN_OFF)

        test_pass_handler = self.emulator.nfc_emulator.asyncWaitForTestPass(
            'ApduSuccess')

        command_apdus, response_apdus = get_apdus(self.emulator.nfc_emulator, _TRANSPORT_SERVICE_1)
        tag_detected, transacted = poll_and_transact(self.pn532, command_apdus[:1],
                                                     response_apdus[:1])
        asserts.assert_false(tag_detected, "Tag is detected unexpectedly!")
        asserts.assert_false(transacted, "Transaction is completed unexpectedly!")

        # Set listen on
        self.emulator.nfc_emulator.setListenTech(_NFC_TECH_A_LISTEN_ON)
        tag_detected, transacted = poll_and_transact(self.pn532, command_apdus[:1],
                                                     response_apdus[:1])
        asserts.assert_true(tag_detected, _FAILED_TAG_MSG)
        asserts.assert_true(transacted, _FAILED_TRANSACTION_MSG)


    #@CddTest(requirements = {"7.4.4/C-2-2", "7.4.4/C-1-2"})
    def test_single_non_payment_service_with_listen_tech_poll_tech_mismatch(self):
        """Tests successful APDU exchange between non-payment service and
        reader does not proceed when emulator listen tech mismatches reader poll tech.

        Test Steps:
        1. Start emulator activity and set up non-payment HCE Service.
        2. Set listen tech to Type-F on the emulator.
        3. Start PN532 and verify transaction does not proceed.
        4. Set listen tech to Type-A on the emulator.
        6. Start PN532 and verify APDU exchange between reader and emulator.

        Verifies:
        1. Verifies that no APDU exchange occurs when the listen tech mismatches with poll tech.
        2. Verifies a successful APDU exchange when no longer mismatched.
        """
        self._set_up_emulator(service_list=[_TRANSPORT_SERVICE_1],
                              expected_service=_TRANSPORT_SERVICE_1, is_payment=False)
        # Set listen to Type-F
        self.emulator.nfc_emulator.setListenTech(_NFC_TECH_F_LISTEN_ON)

        command_apdus, response_apdus = get_apdus(self.emulator.nfc_emulator, _TRANSPORT_SERVICE_1)
        tag_detected, transacted = poll_and_transact(self.pn532, command_apdus[:1],
                                                     response_apdus[:1])
        asserts.assert_false(tag_detected, "Tag is detected unexpectedly!")
        asserts.assert_false(transacted, "Transaction is completed unexpectedly!")

        # Set listen to Type-A
        self.emulator.nfc_emulator.setListenTech(_NFC_TECH_A_LISTEN_ON)
        tag_detected, transacted = poll_and_transact(self.pn532, command_apdus[:1], response_apdus[:1])
        asserts.assert_true(tag_detected, _FAILED_TAG_MSG)
        asserts.assert_true(transacted, _FAILED_TRANSACTION_MSG)

if __name__ == '__main__':
    # Take test args
    if '--' in sys.argv:
        index = sys.argv.index('--')
        sys.argv = sys.argv[:1] + sys.argv[index + 1:]
    test_runner.main()
