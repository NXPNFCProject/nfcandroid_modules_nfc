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

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertTrue;

import org.junit.Test;

public class ErrorCodesTest {
    @Test
    public void testIsErrorForValidCode() {
        assertFalse(ErrorCodes.isError(ErrorCodes.SUCCESS));
    }

    @Test
    public void testIsErrorForErrorCode() {
        assertTrue(ErrorCodes.isError(ErrorCodes.ERROR_CANCELLED));
        assertTrue(ErrorCodes.isError(ErrorCodes.ERROR_TIMEOUT));
    }

    @Test
    public void testAsStringValidCode() {
        assertEquals("SUCCESS", ErrorCodes.asString(ErrorCodes.SUCCESS));
        assertEquals("IO", ErrorCodes.asString(ErrorCodes.ERROR_IO));
        assertEquals("CANCELLED", ErrorCodes.asString(ErrorCodes.ERROR_CANCELLED));
        assertEquals("TIMEOUT", ErrorCodes.asString(ErrorCodes.ERROR_TIMEOUT));
        assertEquals("BUSY", ErrorCodes.asString(ErrorCodes.ERROR_BUSY));
        assertEquals("CONNECT/DISCONNECT", ErrorCodes.asString(ErrorCodes.ERROR_CONNECT));
        assertEquals("READ", ErrorCodes.asString(ErrorCodes.ERROR_READ));
        assertEquals("WRITE", ErrorCodes.asString(ErrorCodes.ERROR_WRITE));
        assertEquals("INVALID_PARAM", ErrorCodes.asString(ErrorCodes.ERROR_INVALID_PARAM));
        assertEquals("INSUFFICIENT_RESOURCES",
                ErrorCodes.asString(ErrorCodes.ERROR_INSUFFICIENT_RESOURCES));
        assertEquals("SOCKET_CREATION", ErrorCodes.asString(ErrorCodes.ERROR_SOCKET_CREATION));
        assertEquals("SOCKET_NOT_CONNECTED",
                ErrorCodes.asString(ErrorCodes.ERROR_SOCKET_NOT_CONNECTED));
        assertEquals("BUFFER_TO_SMALL", ErrorCodes.asString(ErrorCodes.ERROR_BUFFER_TO_SMALL));
        assertEquals("SAP_USED", ErrorCodes.asString(ErrorCodes.ERROR_SAP_USED));
        assertEquals("SERVICE_NAME_USED", ErrorCodes.asString(ErrorCodes.ERROR_SERVICE_NAME_USED));
        assertEquals("SOCKET_OPTIONS", ErrorCodes.asString(ErrorCodes.ERROR_SOCKET_OPTIONS));
        assertEquals("NFC_ON", ErrorCodes.asString(ErrorCodes.ERROR_NFC_ON));
        assertEquals("NOT_INITIALIZED", ErrorCodes.asString(ErrorCodes.ERROR_NOT_INITIALIZED));
        assertEquals("SE_ALREADY_SELECTED",
                ErrorCodes.asString(ErrorCodes.ERROR_SE_ALREADY_SELECTED));
        assertEquals("SE_CONNECTED", ErrorCodes.asString(ErrorCodes.ERROR_SE_CONNECTED));
        assertEquals("NO_SE_CONNECTED", ErrorCodes.asString(ErrorCodes.ERROR_NO_SE_CONNECTED));
        assertEquals("NOT_SUPPORTED", ErrorCodes.asString(ErrorCodes.ERROR_NOT_SUPPORTED));
    }

    @Test
    public void testAsStringUnknownCode() {
        assertEquals("UNKNOWN ERROR", ErrorCodes.asString(999));
        assertEquals("UNKNOWN ERROR", ErrorCodes.asString(-999));
    }
}
