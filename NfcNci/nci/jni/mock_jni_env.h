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

#ifndef AOSP_NFC_MOCK_JNI_ENV_H
#define AOSP_NFC_MOCK_JNI_ENV_H

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#if defined(__ANDROID__) && __NDK_MAJOR__ < 18
using InputJvalueArray = jvalue*;
#else
using InputJvalueArray = const jvalue*;
#endif

class MockableJNIEnv : public JNIEnv {
 public:
  MockableJNIEnv() {
    functions = &functions_;
    functions_.AllocObject = &CallAllocObject;
    functions_.CallBooleanMethodV = &CallCallBooleanMethodV;
    functions_.CallByteMethodV = &CallCallByteMethodV;
    functions_.CallCharMethodV = &CallCallCharMethodV;
    functions_.CallDoubleMethodV = &CallCallDoubleMethodV;
    functions_.CallFloatMethodV = &CallCallFloatMethodV;
    functions_.CallIntMethodV = &CallCallIntMethodV;
    functions_.CallLongMethodV = &CallCallLongMethodV;
    functions_.CallNonvirtualBooleanMethodV = &CallCallNonvirtualBooleanMethodV;
    functions_.CallNonvirtualByteMethodV = &CallCallNonvirtualByteMethodV;
    functions_.CallNonvirtualCharMethodV = &CallCallNonvirtualCharMethodV;
    functions_.CallNonvirtualDoubleMethodV = &CallCallNonvirtualDoubleMethodV;
    functions_.CallNonvirtualFloatMethodV = &CallCallNonvirtualFloatMethodV;
    functions_.CallNonvirtualIntMethodV = &CallCallNonvirtualIntMethodV;
    functions_.CallNonvirtualLongMethodV = &CallCallNonvirtualLongMethodV;
    functions_.CallNonvirtualObjectMethodV = &CallCallNonvirtualObjectMethodV;
    functions_.CallNonvirtualShortMethodV = &CallCallNonvirtualShortMethodV;
    functions_.CallNonvirtualVoidMethodV = &CallCallNonvirtualVoidMethodV;
    functions_.CallObjectMethodV = &CallCallObjectMethodV;
    functions_.CallShortMethodV = &CallCallShortMethodV;
    functions_.CallStaticBooleanMethodV = &CallCallStaticBooleanMethodV;
    functions_.CallStaticByteMethodV = &CallCallStaticByteMethodV;
    functions_.CallStaticCharMethodV = &CallCallStaticCharMethodV;
    functions_.CallStaticDoubleMethodV = &CallCallStaticDoubleMethodV;
    functions_.CallStaticFloatMethodV = &CallCallStaticFloatMethodV;
    functions_.CallStaticIntMethodV = &CallCallStaticIntMethodV;
    functions_.CallStaticLongMethodV = &CallCallStaticLongMethodV;
    functions_.CallStaticObjectMethodV = &CallCallStaticObjectMethodV;
    functions_.CallStaticShortMethodV = &CallCallStaticShortMethodV;
    functions_.CallStaticVoidMethodV = &CallCallStaticVoidMethodV;
    functions_.CallVoidMethodV = &CallCallVoidMethodV;
    functions_.DefineClass = &CallDefineClass;
    functions_.DeleteGlobalRef = &CallDeleteGlobalRef;
    functions_.DeleteLocalRef = &CallDeleteLocalRef;
    functions_.DeleteWeakGlobalRef = &CallDeleteWeakGlobalRef;
    functions_.EnsureLocalCapacity = &CallEnsureLocalCapacity;
    functions_.ExceptionCheck = &CallExceptionCheck;
    functions_.ExceptionClear = &CallExceptionClear;
    functions_.ExceptionDescribe = &CallExceptionDescribe;
    functions_.ExceptionOccurred = &CallExceptionOccurred;
    functions_.FatalError = &CallFatalError;
    functions_.FindClass = &CallFindClass;
    functions_.FromReflectedField = &CallFromReflectedField;
    functions_.FromReflectedMethod = &CallFromReflectedMethod;
    functions_.GetArrayLength = &CallGetArrayLength;
    functions_.GetBooleanArrayElements = &CallGetBooleanArrayElements;
    functions_.GetBooleanArrayRegion = &CallGetBooleanArrayRegion;
    functions_.GetBooleanField = &CallGetBooleanField;
    functions_.GetByteArrayElements = &CallGetByteArrayElements;
    functions_.GetByteArrayRegion = &CallGetByteArrayRegion;
    functions_.GetByteField = &CallGetByteField;
    functions_.GetCharArrayElements = &CallGetCharArrayElements;
    functions_.GetCharArrayRegion = &CallGetCharArrayRegion;
    functions_.GetCharField = &CallGetCharField;
    functions_.GetDirectBufferAddress = &CallGetDirectBufferAddress;
    functions_.GetDirectBufferCapacity = &CallGetDirectBufferCapacity;
    functions_.GetDoubleArrayElements = &CallGetDoubleArrayElements;
    functions_.GetDoubleArrayRegion = &CallGetDoubleArrayRegion;
    functions_.GetDoubleField = &CallGetDoubleField;
    functions_.GetFieldID = &CallGetFieldID;
    functions_.GetFloatArrayElements = &CallGetFloatArrayElements;
    functions_.GetFloatArrayRegion = &CallGetFloatArrayRegion;
    functions_.GetFloatField = &CallGetFloatField;
    functions_.GetIntArrayElements = &CallGetIntArrayElements;
    functions_.GetIntArrayRegion = &CallGetIntArrayRegion;
    functions_.GetIntField = &CallGetIntField;
    functions_.GetJavaVM = &CallGetJavaVM;
    functions_.GetLongArrayElements = &CallGetLongArrayElements;
    functions_.GetLongArrayRegion = &CallGetLongArrayRegion;
    functions_.GetLongField = &CallGetLongField;
    functions_.GetMethodID = &CallGetMethodID;
    functions_.GetObjectArrayElement = &CallGetObjectArrayElement;
    functions_.GetObjectClass = &CallGetObjectClass;
    functions_.GetObjectField = &CallGetObjectField;
    functions_.GetObjectRefType = &CallGetObjectRefType;
    functions_.GetPrimitiveArrayCritical = &CallGetPrimitiveArrayCritical;
    functions_.GetShortArrayElements = &CallGetShortArrayElements;
    functions_.GetShortArrayRegion = &CallGetShortArrayRegion;
    functions_.GetShortField = &CallGetShortField;
    functions_.GetStaticBooleanField = &CallGetStaticBooleanField;
    functions_.GetStaticByteField = &CallGetStaticByteField;
    functions_.GetStaticCharField = &CallGetStaticCharField;
    functions_.GetStaticDoubleField = &CallGetStaticDoubleField;
    functions_.GetStaticFieldID = &CallGetStaticFieldID;
    functions_.GetStaticFloatField = &CallGetStaticFloatField;
    functions_.GetStaticIntField = &CallGetStaticIntField;
    functions_.GetStaticLongField = &CallGetStaticLongField;
    functions_.GetStaticMethodID = &CallGetStaticMethodID;
    functions_.GetStaticObjectField = &CallGetStaticObjectField;
    functions_.GetStaticShortField = &CallGetStaticShortField;
    functions_.GetStringChars = &CallGetStringChars;
    functions_.GetStringCritical = &CallGetStringCritical;
    functions_.GetStringLength = &CallGetStringLength;
    functions_.GetStringRegion = &CallGetStringRegion;
    functions_.GetStringUTFChars = &CallGetStringUTFChars;
    functions_.GetStringUTFLength = &CallGetStringUTFLength;
    functions_.GetStringUTFRegion = &CallGetStringUTFRegion;
    functions_.GetSuperclass = &CallGetSuperclass;
    functions_.GetVersion = &CallGetVersion;
    functions_.IsAssignableFrom = &CallIsAssignableFrom;
    functions_.IsInstanceOf = &CallIsInstanceOf;
    functions_.IsSameObject = &CallIsSameObject;
    functions_.MonitorEnter = &CallMonitorEnter;
    functions_.MonitorExit = &CallMonitorExit;
    functions_.NewBooleanArray = &CallNewBooleanArray;
    functions_.NewByteArray = &CallNewByteArray;
    functions_.NewCharArray = &CallNewCharArray;
    functions_.NewDirectByteBuffer = &CallNewDirectByteBuffer;
    functions_.NewDoubleArray = &CallNewDoubleArray;
    functions_.NewFloatArray = &CallNewFloatArray;
    functions_.NewGlobalRef = &CallNewGlobalRef;
    functions_.NewIntArray = &CallNewIntArray;
    functions_.NewLocalRef = &CallNewLocalRef;
    functions_.NewLongArray = &CallNewLongArray;
    functions_.NewObjectArray = &CallNewObjectArray;
    functions_.NewObjectV = &CallNewObjectV;
    functions_.NewShortArray = &CallNewShortArray;
    functions_.NewString = &CallNewString;
    functions_.NewStringUTF = &CallNewStringUTF;
    functions_.NewWeakGlobalRef = &CallNewWeakGlobalRef;
    functions_.PopLocalFrame = &CallPopLocalFrame;
    functions_.PushLocalFrame = &CallPushLocalFrame;
    functions_.RegisterNatives = &CallRegisterNatives;
    functions_.ReleaseBooleanArrayElements = &CallReleaseBooleanArrayElements;
    functions_.ReleaseByteArrayElements = &CallReleaseByteArrayElements;
    functions_.ReleaseCharArrayElements = &CallReleaseCharArrayElements;
    functions_.ReleaseDoubleArrayElements = &CallReleaseDoubleArrayElements;
    functions_.ReleaseFloatArrayElements = &CallReleaseFloatArrayElements;
    functions_.ReleaseIntArrayElements = &CallReleaseIntArrayElements;
    functions_.ReleaseLongArrayElements = &CallReleaseLongArrayElements;
    functions_.ReleasePrimitiveArrayCritical =
        &CallReleasePrimitiveArrayCritical;
    functions_.ReleaseShortArrayElements = &CallReleaseShortArrayElements;
    functions_.ReleaseStringChars = &CallReleaseStringChars;
    functions_.ReleaseStringCritical = &CallReleaseStringCritical;
    functions_.ReleaseStringUTFChars = &CallReleaseStringUTFChars;
    functions_.SetBooleanArrayRegion = &CallSetBooleanArrayRegion;
    functions_.SetBooleanField = &CallSetBooleanField;
    functions_.SetByteArrayRegion = &CallSetByteArrayRegion;
    functions_.SetByteField = &CallSetByteField;
    functions_.SetCharArrayRegion = &CallSetCharArrayRegion;
    functions_.SetCharField = &CallSetCharField;
    functions_.SetDoubleArrayRegion = &CallSetDoubleArrayRegion;
    functions_.SetDoubleField = &CallSetDoubleField;
    functions_.SetFloatArrayRegion = &CallSetFloatArrayRegion;
    functions_.SetFloatField = &CallSetFloatField;
    functions_.SetIntArrayRegion = &CallSetIntArrayRegion;
    functions_.SetIntField = &CallSetIntField;
    functions_.SetLongArrayRegion = &CallSetLongArrayRegion;
    functions_.SetLongField = &CallSetLongField;
    functions_.SetObjectArrayElement = &CallSetObjectArrayElement;
    functions_.SetObjectField = &CallSetObjectField;
    functions_.SetShortArrayRegion = &CallSetShortArrayRegion;
    functions_.SetShortField = &CallSetShortField;
    functions_.SetStaticBooleanField = &CallSetStaticBooleanField;
    functions_.SetStaticByteField = &CallSetStaticByteField;
    functions_.SetStaticCharField = &CallSetStaticCharField;
    functions_.SetStaticDoubleField = &CallSetStaticDoubleField;
    functions_.SetStaticFloatField = &CallSetStaticFloatField;
    functions_.SetStaticIntField = &CallSetStaticIntField;
    functions_.SetStaticLongField = &CallSetStaticLongField;
    functions_.SetStaticObjectField = &CallSetStaticObjectField;
    functions_.SetStaticShortField = &CallSetStaticShortField;
    functions_.Throw = &CallThrow;
    functions_.ThrowNew = &CallThrowNew;
    functions_.ToReflectedField = &CallToReflectedField;
    functions_.ToReflectedMethod = &CallToReflectedMethod;
    functions_.UnregisterNatives = &CallUnregisterNatives;
  }
  virtual ~MockableJNIEnv() {}

  virtual jobject AllocObject(jclass clazz) = 0;

  virtual jboolean CallBooleanMethodV(jobject obj, jmethodID methodID,
                                      va_list args) = 0;
  virtual jbyte CallByteMethodV(jobject obj, jmethodID methodID,
                                va_list args) = 0;
  virtual jchar CallCharMethodV(jobject obj, jmethodID methodID,
                                va_list args) = 0;
  virtual jdouble CallDoubleMethodV(jobject obj, jmethodID methodID,
                                    va_list args) = 0;
  virtual jfloat CallFloatMethodV(jobject obj, jmethodID methodID,
                                  va_list args) = 0;
  virtual jint CallIntMethodV(jobject obj, jmethodID methodID,
                              va_list args) = 0;
  virtual jlong CallLongMethodV(jobject obj, jmethodID methodID,
                                va_list args) = 0;
  virtual jboolean CallNonvirtualBooleanMethodV(jobject obj, jclass clazz,
                                                jmethodID methodID,
                                                va_list args) = 0;
  virtual jbyte CallNonvirtualByteMethodV(jobject obj, jclass clazz,
                                          jmethodID methodID, va_list args) = 0;
  virtual jchar CallNonvirtualCharMethodV(jobject obj, jclass clazz,
                                          jmethodID methodID, va_list args) = 0;
  virtual jdouble CallNonvirtualDoubleMethodV(jobject obj, jclass clazz,
                                              jmethodID methodID,
                                              va_list args) = 0;
  virtual jfloat CallNonvirtualFloatMethodV(jobject obj, jclass clazz,
                                            jmethodID methodID,
                                            va_list args) = 0;
  virtual jint CallNonvirtualIntMethodV(jobject obj, jclass clazz,
                                        jmethodID methodID, va_list args) = 0;
  virtual jlong CallNonvirtualLongMethodV(jobject obj, jclass clazz,
                                          jmethodID methodID, va_list args) = 0;
  virtual jobject CallNonvirtualObjectMethodV(jobject obj, jclass clazz,
                                              jmethodID methodID,
                                              va_list args) = 0;
  virtual jshort CallNonvirtualShortMethodV(jobject obj, jclass clazz,
                                            jmethodID methodID,
                                            va_list args) = 0;
  virtual void CallNonvirtualVoidMethodV(jobject obj, jclass clazz,
                                         jmethodID methodID, va_list args) = 0;
  virtual jobject CallObjectMethodV(jobject obj, jmethodID methodID,
                                    va_list args) = 0;
  virtual jshort CallShortMethodV(jobject obj, jmethodID methodID,
                                  va_list args) = 0;
  virtual jboolean CallStaticBooleanMethodV(jclass clazz, jmethodID methodID,
                                            va_list args) = 0;
  virtual jbyte CallStaticByteMethodV(jclass clazz, jmethodID methodID,
                                      va_list args) = 0;
  virtual jchar CallStaticCharMethodV(jclass clazz, jmethodID methodID,
                                      va_list args) = 0;
  jdouble CallStaticDoubleMethod(jclass clazz, jmethodID methodID, ...) {
    va_list args;
    va_start(args, methodID);
    jdouble result = CallStaticDoubleMethodV(clazz, methodID, args);
    va_end(args);
    return result;
  }
  virtual jdouble CallStaticDoubleMethodV(jclass clazz, jmethodID methodID,
                                          va_list args) = 0;
  virtual jfloat CallStaticFloatMethodV(jclass clazz, jmethodID methodID,
                                        va_list args) = 0;
  virtual jint CallStaticIntMethodV(jclass clazz, jmethodID methodID,
                                    va_list args) = 0;
  jlong CallStaticLongMethod(jclass clazz, jmethodID methodID, ...) {
    va_list args;
    va_start(args, methodID);
    jlong result = CallStaticLongMethodV(clazz, methodID, args);
    va_end(args);
    return result;
  }
  virtual jlong CallStaticLongMethodV(jclass clazz, jmethodID methodID,
                                      va_list args) = 0;
  virtual jobject CallStaticObjectMethodV(jclass clazz, jmethodID methodID,
                                          va_list args) = 0;
  virtual jshort CallStaticShortMethodV(jclass clazz, jmethodID methodID,
                                        va_list args) = 0;
  virtual void CallStaticVoidMethodV(jclass cls, jmethodID methodID,
                                     va_list args) = 0;
  virtual void CallVoidMethodV(jobject obj, jmethodID methodID,
                               va_list args) = 0;
  virtual jclass DefineClass(const char* name, jobject loader, const jbyte* buf,
                             jsize len) = 0;
  virtual void DeleteGlobalRef(jobject gref) = 0;
  virtual void DeleteLocalRef(jobject obj) = 0;
  virtual void DeleteWeakGlobalRef(jweak ref) = 0;
  virtual jint EnsureLocalCapacity(jint capacity) = 0;
  virtual jboolean ExceptionCheck() = 0;
  virtual void ExceptionClear() = 0;
  virtual void ExceptionDescribe() = 0;
  virtual jthrowable ExceptionOccurred() = 0;
  virtual void FatalError(const char* msg) = 0;
  virtual jclass FindClass(const char* name) = 0;
  virtual jfieldID FromReflectedField(jobject field) = 0;
  virtual jmethodID FromReflectedMethod(jobject method) = 0;
  virtual jsize GetArrayLength(jarray array) = 0;
  virtual jboolean* GetBooleanArrayElements(jbooleanArray array,
                                            jboolean* isCopy) = 0;
  virtual void GetBooleanArrayRegion(jbooleanArray array, jsize start,
                                     jsize len, jboolean* buf) = 0;
  virtual jboolean GetBooleanField(jobject obj, jfieldID fieldID) = 0;
  virtual jbyte* GetByteArrayElements(jbyteArray array, jboolean* isCopy) = 0;
  virtual void GetByteArrayRegion(jbyteArray array, jsize start, jsize len,
                                  jbyte* buf) = 0;
  virtual jbyte GetByteField(jobject obj, jfieldID fieldID) = 0;
  virtual jchar* GetCharArrayElements(jcharArray array, jboolean* isCopy) = 0;
  virtual void GetCharArrayRegion(jcharArray array, jsize start, jsize len,
                                  jchar* buf) = 0;
  virtual jchar GetCharField(jobject obj, jfieldID fieldID) = 0;
  virtual void* GetDirectBufferAddress(jobject buf) = 0;
  virtual jlong GetDirectBufferCapacity(jobject buf) = 0;
  virtual jdouble* GetDoubleArrayElements(jdoubleArray array,
                                          jboolean* isCopy) = 0;
  virtual void GetDoubleArrayRegion(jdoubleArray array, jsize start, jsize len,
                                    jdouble* buf) = 0;
  virtual jdouble GetDoubleField(jobject obj, jfieldID fieldID) = 0;
  virtual jfieldID GetFieldID(jclass clazz, const char* name,
                              const char* sig) = 0;
  virtual jfloat* GetFloatArrayElements(jfloatArray array,
                                        jboolean* isCopy) = 0;
  virtual void GetFloatArrayRegion(jfloatArray array, jsize start, jsize len,
                                   jfloat* buf) = 0;
  virtual jfloat GetFloatField(jobject obj, jfieldID fieldID) = 0;
  virtual jint* GetIntArrayElements(jintArray array, jboolean* isCopy) = 0;
  virtual void GetIntArrayRegion(jintArray array, jsize start, jsize len,
                                 jint* buf) = 0;
  virtual jint GetIntField(jobject obj, jfieldID fieldID) = 0;
  virtual jint GetJavaVM(JavaVM** vm) = 0;
  virtual jlong* GetLongArrayElements(jlongArray array, jboolean* isCopy) = 0;
  virtual void GetLongArrayRegion(jlongArray array, jsize start, jsize len,
                                  jlong* buf) = 0;
  virtual jlong GetLongField(jobject obj, jfieldID fieldID) = 0;
  virtual jmethodID GetMethodID(jclass clazz, const char* name,
                                const char* sig) = 0;
  virtual jobject GetObjectArrayElement(jobjectArray array, jsize index) = 0;
  virtual jclass GetObjectClass(jobject obj) = 0;
  virtual jobject GetObjectField(jobject obj, jfieldID fieldID) = 0;
  virtual jobjectRefType GetObjectRefType(jobject obj) = 0;
  virtual void* GetPrimitiveArrayCritical(jarray array, jboolean* isCopy) = 0;
  virtual jshort* GetShortArrayElements(jshortArray array,
                                        jboolean* isCopy) = 0;
  virtual void GetShortArrayRegion(jshortArray array, jsize start, jsize len,
                                   jshort* buf) = 0;
  virtual jshort GetShortField(jobject obj, jfieldID fieldID) = 0;
  virtual jboolean GetStaticBooleanField(jclass clazz, jfieldID fieldID) = 0;
  virtual jbyte GetStaticByteField(jclass clazz, jfieldID fieldID) = 0;
  virtual jchar GetStaticCharField(jclass clazz, jfieldID fieldID) = 0;
  virtual jdouble GetStaticDoubleField(jclass clazz, jfieldID fieldID) = 0;
  virtual jfieldID GetStaticFieldID(jclass clazz, const char* name,
                                    const char* sig) = 0;
  virtual jfloat GetStaticFloatField(jclass clazz, jfieldID fieldID) = 0;
  virtual jint GetStaticIntField(jclass clazz, jfieldID fieldID) = 0;
  virtual jlong GetStaticLongField(jclass clazz, jfieldID fieldID) = 0;
  virtual jmethodID GetStaticMethodID(jclass clazz, const char* name,
                                      const char* sig) = 0;
  virtual jobject GetStaticObjectField(jclass clazz, jfieldID fieldID) = 0;
  virtual jshort GetStaticShortField(jclass clazz, jfieldID fieldID) = 0;
  virtual jsize GetStringLength(jstring str) = 0;
  virtual void GetStringRegion(jstring str, jsize start, jsize len,
                               jchar* buf) = 0;
  virtual const char* GetStringUTFChars(jstring str, jboolean* isCopy) = 0;
  virtual jsize GetStringUTFLength(jstring str) = 0;
  virtual void GetStringUTFRegion(jstring str, jsize start, jsize len,
                                  char* buf) = 0;
  virtual jclass GetSuperclass(jclass sub) = 0;
  virtual jint GetVersion() = 0;
  virtual jboolean IsAssignableFrom(jclass sub, jclass sup) = 0;
  virtual jboolean IsInstanceOf(jobject obj, jclass clazz) = 0;
  virtual jboolean IsSameObject(jobject obj1, jobject obj2) = 0;
  virtual const jchar* GetStringChars(jstring str, jboolean* isCopy) = 0;
  virtual const jchar* GetStringCritical(jstring str, jboolean* isCopy) = 0;
  virtual jint MonitorEnter(jobject obj) = 0;
  virtual jint MonitorExit(jobject obj) = 0;
  virtual jbooleanArray NewBooleanArray(jsize len) = 0;
  virtual jbyteArray NewByteArray(jsize len) = 0;
  virtual jcharArray NewCharArray(jsize len) = 0;
  virtual jobject NewDirectByteBuffer(void* address, jlong capacity) = 0;
  virtual jdoubleArray NewDoubleArray(jsize len) = 0;
  virtual jfloatArray NewFloatArray(jsize len) = 0;
  virtual jobject NewGlobalRef(jobject lobj) = 0;
  virtual jintArray NewIntArray(jsize len) = 0;
  virtual jobject NewLocalRef(jobject ref) = 0;
  virtual jlongArray NewLongArray(jsize len) = 0;
  virtual jobject NewObjectA(jclass clazz, jmethodID methodID,
                             InputJvalueArray args) = 0;
  virtual jobjectArray NewObjectArray(jsize len, jclass clazz,
                                      jobject init) = 0;
  virtual jobject NewObjectV(jclass clazz, jmethodID methodID,
                             va_list args) = 0;
  virtual jshortArray NewShortArray(jsize len) = 0;
  virtual jstring NewString(const jchar* unicode, jsize len) = 0;
  virtual jstring NewStringUTF(const char* utf) = 0;
  virtual jweak NewWeakGlobalRef(jobject obj) = 0;
  virtual jobject PopLocalFrame(jobject result) = 0;
  virtual jint PushLocalFrame(jint capacity) = 0;
  virtual jint RegisterNatives(jclass clazz, const JNINativeMethod* methods,
                               jint nMethods) = 0;
  virtual void ReleaseBooleanArrayElements(jbooleanArray array, jboolean* elems,
                                           jint mode) = 0;
  virtual void ReleaseByteArrayElements(jbyteArray array, jbyte* elems,
                                        jint mode) = 0;
  virtual void ReleaseCharArrayElements(jcharArray array, jchar* elems,
                                        jint mode) = 0;
  virtual void ReleaseDoubleArrayElements(jdoubleArray array, jdouble* elems,
                                          jint mode) = 0;
  virtual void ReleaseFloatArrayElements(jfloatArray array, jfloat* elems,
                                         jint mode) = 0;
  virtual void ReleaseIntArrayElements(jintArray array, jint* elems,
                                       jint mode) = 0;
  virtual void ReleaseLongArrayElements(jlongArray array, jlong* elems,
                                        jint mode) = 0;
  virtual void ReleasePrimitiveArrayCritical(jarray array, void* carray,
                                             jint mode) = 0;
  virtual void ReleaseShortArrayElements(jshortArray array, jshort* elems,
                                         jint mode) = 0;
  virtual void ReleaseStringChars(jstring str, const jchar* chars) = 0;
  virtual void ReleaseStringCritical(jstring str, const jchar* cstring) = 0;
  virtual void ReleaseStringUTFChars(jstring str, const char* chars) = 0;
  virtual void SetBooleanArrayRegion(jbooleanArray array, jsize start,
                                     jsize len, const jboolean* buf) = 0;
  virtual void SetBooleanField(jobject obj, jfieldID fieldID, jboolean val) = 0;
  virtual void SetByteArrayRegion(jbyteArray array, jsize start, jsize len,
                                  const jbyte* buf) = 0;
  virtual void SetByteField(jobject obj, jfieldID fieldID, jbyte val) = 0;
  virtual void SetCharArrayRegion(jcharArray array, jsize start, jsize len,
                                  const jchar* buf) = 0;
  virtual void SetCharField(jobject obj, jfieldID fieldID, jchar val) = 0;
  virtual void SetDoubleArrayRegion(jdoubleArray array, jsize start, jsize len,
                                    const jdouble* buf) = 0;
  virtual void SetDoubleField(jobject obj, jfieldID fieldID, jdouble val) = 0;
  virtual void SetFloatArrayRegion(jfloatArray array, jsize start, jsize len,
                                   const jfloat* buf) = 0;
  virtual void SetFloatField(jobject obj, jfieldID fieldID, jfloat val) = 0;
  virtual void SetIntArrayRegion(jintArray array, jsize start, jsize len,
                                 const jint* buf) = 0;
  virtual void SetIntField(jobject obj, jfieldID fieldID, jint val) = 0;
  virtual void SetLongArrayRegion(jlongArray array, jsize start, jsize len,
                                  const jlong* buf) = 0;
  virtual void SetLongField(jobject obj, jfieldID fieldID, jlong val) = 0;
  virtual void SetObjectArrayElement(jobjectArray array, jsize index,
                                     jobject val) = 0;
  virtual void SetObjectField(jobject obj, jfieldID fieldID, jobject val) = 0;
  virtual void SetShortArrayRegion(jshortArray array, jsize start, jsize len,
                                   const jshort* buf) = 0;
  virtual void SetShortField(jobject obj, jfieldID fieldID, jshort val) = 0;
  virtual void SetStaticBooleanField(jclass clazz, jfieldID fieldID,
                                     jboolean value) = 0;
  virtual void SetStaticByteField(jclass clazz, jfieldID fieldID,
                                  jbyte value) = 0;
  virtual void SetStaticCharField(jclass clazz, jfieldID fieldID,
                                  jchar value) = 0;
  virtual void SetStaticDoubleField(jclass clazz, jfieldID fieldID,
                                    jdouble value) = 0;
  virtual void SetStaticFloatField(jclass clazz, jfieldID fieldID,
                                   jfloat value) = 0;
  virtual void SetStaticIntField(jclass clazz, jfieldID fieldID,
                                 jint value) = 0;
  virtual void SetStaticLongField(jclass clazz, jfieldID fieldID,
                                  jlong value) = 0;
  virtual void SetStaticObjectField(jclass clazz, jfieldID fieldID,
                                    jobject value) = 0;
  virtual void SetStaticShortField(jclass clazz, jfieldID fieldID,
                                   jshort value) = 0;
  virtual jint Throw(jthrowable obj) = 0;
  virtual jint ThrowNew(jclass clazz, const char* msg) = 0;
  virtual jobject ToReflectedField(jclass cls, jfieldID fieldID,
                                   jboolean isStatic) = 0;
  virtual jobject ToReflectedMethod(jclass cls, jmethodID methodID,
                                    jboolean isStatic) = 0;
  virtual jint UnregisterNatives(jclass clazz) = 0;

 private:
  static jobject JNICALL CallAllocObject(JNIEnv* env, jclass clazz) {
    return static_cast<MockableJNIEnv*>(env)->AllocObject(clazz);
  }
  static jboolean JNICALL CallCallBooleanMethodV(JNIEnv* env, jobject obj,
                                                 jmethodID methodID,
                                                 va_list args) {
    return static_cast<MockableJNIEnv*>(env)->CallBooleanMethodV(obj, methodID,
                                                                 args);
  }
  static jbyte JNICALL CallCallByteMethodV(JNIEnv* env, jobject obj,
                                           jmethodID methodID, va_list args) {
    return static_cast<MockableJNIEnv*>(env)->CallByteMethodV(obj, methodID,
                                                              args);
  }
  static jchar JNICALL CallCallCharMethodV(JNIEnv* env, jobject obj,
                                           jmethodID methodID, va_list args) {
    return static_cast<MockableJNIEnv*>(env)->CallCharMethodV(obj, methodID,
                                                              args);
  }
  static jdouble JNICALL CallCallDoubleMethodV(JNIEnv* env, jobject obj,
                                               jmethodID methodID,
                                               va_list args) {
    return static_cast<MockableJNIEnv*>(env)->CallDoubleMethodV(obj, methodID,
                                                                args);
  }
  static jfloat JNICALL CallCallFloatMethodV(JNIEnv* env, jobject obj,
                                             jmethodID methodID, va_list args) {
    return static_cast<MockableJNIEnv*>(env)->CallFloatMethodV(obj, methodID,
                                                               args);
  }
  static jint JNICALL CallCallIntMethodV(JNIEnv* env, jobject obj,
                                         jmethodID methodID, va_list args) {
    return static_cast<MockableJNIEnv*>(env)->CallIntMethodV(obj, methodID,
                                                             args);
  }
  static jlong JNICALL CallCallLongMethodV(JNIEnv* env, jobject obj,
                                           jmethodID methodID, va_list args) {
    return static_cast<MockableJNIEnv*>(env)->CallLongMethodV(obj, methodID,
                                                              args);
  }
  static jboolean JNICALL CallCallNonvirtualBooleanMethodV(JNIEnv* env,
                                                           jobject obj,
                                                           jclass clazz,
                                                           jmethodID methodID,
                                                           va_list args) {
    return static_cast<MockableJNIEnv*>(env)->CallNonvirtualBooleanMethodV(
        obj, clazz, methodID, args);
  }
  static jbyte JNICALL CallCallNonvirtualByteMethodV(JNIEnv* env, jobject obj,
                                                     jclass clazz,
                                                     jmethodID methodID,
                                                     va_list args) {
    return static_cast<MockableJNIEnv*>(env)->CallNonvirtualByteMethodV(
        obj, clazz, methodID, args);
  }
  static jchar JNICALL CallCallNonvirtualCharMethodV(JNIEnv* env, jobject obj,
                                                     jclass clazz,
                                                     jmethodID methodID,
                                                     va_list args) {
    return static_cast<MockableJNIEnv*>(env)->CallNonvirtualCharMethodV(
        obj, clazz, methodID, args);
  }
  static jdouble JNICALL CallCallNonvirtualDoubleMethodV(JNIEnv* env,
                                                         jobject obj,
                                                         jclass clazz,
                                                         jmethodID methodID,
                                                         va_list args) {
    return static_cast<MockableJNIEnv*>(env)->CallNonvirtualDoubleMethodV(
        obj, clazz, methodID, args);
  }
  static jfloat JNICALL CallCallNonvirtualFloatMethodV(JNIEnv* env, jobject obj,
                                                       jclass clazz,
                                                       jmethodID methodID,
                                                       va_list args) {
    return static_cast<MockableJNIEnv*>(env)->CallNonvirtualFloatMethodV(
        obj, clazz, methodID, args);
  }
  static jint JNICALL CallCallNonvirtualIntMethodV(JNIEnv* env, jobject obj,
                                                   jclass clazz,
                                                   jmethodID methodID,
                                                   va_list args) {
    return static_cast<MockableJNIEnv*>(env)->CallNonvirtualIntMethodV(
        obj, clazz, methodID, args);
  }
  static jlong JNICALL CallCallNonvirtualLongMethodV(JNIEnv* env, jobject obj,
                                                     jclass clazz,
                                                     jmethodID methodID,
                                                     va_list args) {
    return static_cast<MockableJNIEnv*>(env)->CallNonvirtualLongMethodV(
        obj, clazz, methodID, args);
  }
  static jobject JNICALL CallCallNonvirtualObjectMethodV(JNIEnv* env,
                                                         jobject obj,
                                                         jclass clazz,
                                                         jmethodID methodID,
                                                         va_list args) {
    return static_cast<MockableJNIEnv*>(env)->CallNonvirtualObjectMethodV(
        obj, clazz, methodID, args);
  }
  static jshort JNICALL CallCallNonvirtualShortMethodV(JNIEnv* env, jobject obj,
                                                       jclass clazz,
                                                       jmethodID methodID,
                                                       va_list args) {
    return static_cast<MockableJNIEnv*>(env)->CallNonvirtualShortMethodV(
        obj, clazz, methodID, args);
  }
  static void JNICALL CallCallNonvirtualVoidMethodV(JNIEnv* env, jobject obj,
                                                    jclass clazz,
                                                    jmethodID methodID,
                                                    va_list args) {
    static_cast<MockableJNIEnv*>(env)->CallNonvirtualVoidMethodV(
        obj, clazz, methodID, args);
  }
  static jobject JNICALL CallCallObjectMethodV(JNIEnv* env, jobject obj,
                                               jmethodID methodID,
                                               va_list args) {
    return static_cast<MockableJNIEnv*>(env)->CallObjectMethodV(obj, methodID,
                                                                args);
  }
  static jshort JNICALL CallCallShortMethodV(JNIEnv* env, jobject obj,
                                             jmethodID methodID, va_list args) {
    return static_cast<MockableJNIEnv*>(env)->CallShortMethodV(obj, methodID,
                                                               args);
  }
  static jboolean JNICALL CallCallStaticBooleanMethodV(JNIEnv* env,
                                                       jclass clazz,
                                                       jmethodID methodID,
                                                       va_list args) {
    return static_cast<MockableJNIEnv*>(env)->CallStaticBooleanMethodV(
        clazz, methodID, args);
  }
  static jbyte JNICALL CallCallStaticByteMethodV(JNIEnv* env, jclass clazz,
                                                 jmethodID methodID,
                                                 va_list args) {
    return static_cast<MockableJNIEnv*>(env)->CallStaticByteMethodV(
        clazz, methodID, args);
  }
  static jchar JNICALL CallCallStaticCharMethodV(JNIEnv* env, jclass clazz,
                                                 jmethodID methodID,
                                                 va_list args) {
    return static_cast<MockableJNIEnv*>(env)->CallStaticCharMethodV(
        clazz, methodID, args);
  }
  static jdouble JNICALL CallCallStaticDoubleMethodV(JNIEnv* env, jclass clazz,
                                                     jmethodID methodID,
                                                     va_list args) {
    return static_cast<MockableJNIEnv*>(env)->CallStaticDoubleMethodV(
        clazz, methodID, args);
  }
  static jfloat JNICALL CallCallStaticFloatMethodV(JNIEnv* env, jclass clazz,
                                                   jmethodID methodID,
                                                   va_list args) {
    return static_cast<MockableJNIEnv*>(env)->CallStaticFloatMethodV(
        clazz, methodID, args);
  }
  static jint JNICALL CallCallStaticIntMethodV(JNIEnv* env, jclass clazz,
                                               jmethodID methodID,
                                               va_list args) {
    return static_cast<MockableJNIEnv*>(env)->CallStaticIntMethodV(
        clazz, methodID, args);
  }
  static jlong JNICALL CallCallStaticLongMethodV(JNIEnv* env, jclass clazz,
                                                 jmethodID methodID,
                                                 va_list args) {
    return static_cast<MockableJNIEnv*>(env)->CallStaticLongMethodV(
        clazz, methodID, args);
  }
  static jobject JNICALL CallCallStaticObjectMethodV(JNIEnv* env, jclass clazz,
                                                     jmethodID methodID,
                                                     va_list args) {
    return static_cast<MockableJNIEnv*>(env)->CallStaticObjectMethodV(
        clazz, methodID, args);
  }
  static jshort JNICALL CallCallStaticShortMethodV(JNIEnv* env, jclass clazz,
                                                   jmethodID methodID,
                                                   va_list args) {
    return static_cast<MockableJNIEnv*>(env)->CallStaticShortMethodV(
        clazz, methodID, args);
  }
  static void JNICALL CallCallStaticVoidMethodV(JNIEnv* env, jclass cls,
                                                jmethodID methodID,
                                                va_list args) {
    static_cast<MockableJNIEnv*>(env)->CallStaticVoidMethodV(cls, methodID,
                                                             args);
  }
  static void JNICALL CallCallVoidMethodV(JNIEnv* env, jobject obj,
                                          jmethodID methodID, va_list args) {
    static_cast<MockableJNIEnv*>(env)->CallVoidMethodV(obj, methodID, args);
  }
  static jclass JNICALL CallDefineClass(JNIEnv* env, const char* name,
                                        jobject loader, const jbyte* buf,
                                        jsize len) {
    return static_cast<MockableJNIEnv*>(env)->DefineClass(name, loader, buf,
                                                          len);
  }
  static void JNICALL CallDeleteGlobalRef(JNIEnv* env, jobject gref) {
    static_cast<MockableJNIEnv*>(env)->DeleteGlobalRef(gref);
  }
  static void JNICALL CallDeleteLocalRef(JNIEnv* env, jobject obj) {
    static_cast<MockableJNIEnv*>(env)->DeleteLocalRef(obj);
  }
  static void JNICALL CallDeleteWeakGlobalRef(JNIEnv* env, jweak ref) {
    static_cast<MockableJNIEnv*>(env)->DeleteWeakGlobalRef(ref);
  }
  static jint JNICALL CallEnsureLocalCapacity(JNIEnv* env, jint capacity) {
    return static_cast<MockableJNIEnv*>(env)->EnsureLocalCapacity(capacity);
  }
  static jboolean JNICALL CallExceptionCheck(JNIEnv* env) {
    return static_cast<MockableJNIEnv*>(env)->ExceptionCheck();
  }
  static void JNICALL CallExceptionClear(JNIEnv* env) {
    static_cast<MockableJNIEnv*>(env)->ExceptionClear();
  }
  static void JNICALL CallExceptionDescribe(JNIEnv* env) {
    static_cast<MockableJNIEnv*>(env)->ExceptionDescribe();
  }
  static jthrowable JNICALL CallExceptionOccurred(JNIEnv* env) {
    return static_cast<MockableJNIEnv*>(env)->ExceptionOccurred();
  }
  static void JNICALL CallFatalError(JNIEnv* env, const char* msg) {
    static_cast<MockableJNIEnv*>(env)->FatalError(msg);
  }
  static jclass JNICALL CallFindClass(JNIEnv* env, const char* name) {
    return static_cast<MockableJNIEnv*>(env)->FindClass(name);
  }
  static jfieldID JNICALL CallFromReflectedField(JNIEnv* env, jobject field) {
    return static_cast<MockableJNIEnv*>(env)->FromReflectedField(field);
  }
  static jmethodID JNICALL CallFromReflectedMethod(JNIEnv* env,
                                                   jobject method) {
    return static_cast<MockableJNIEnv*>(env)->FromReflectedMethod(method);
  }
  static jsize JNICALL CallGetArrayLength(JNIEnv* env, jarray array) {
    return static_cast<MockableJNIEnv*>(env)->GetArrayLength(array);
  }
  static jboolean* JNICALL CallGetBooleanArrayElements(JNIEnv* env,
                                                       jbooleanArray array,
                                                       jboolean* isCopy) {
    return static_cast<MockableJNIEnv*>(env)->GetBooleanArrayElements(array,
                                                                      isCopy);
  }
  static void JNICALL CallGetBooleanArrayRegion(JNIEnv* env,
                                                jbooleanArray array,
                                                jsize start, jsize len,
                                                jboolean* buf) {
    static_cast<MockableJNIEnv*>(env)->GetBooleanArrayRegion(array, start, len,
                                                             buf);
  }
  static jboolean JNICALL CallGetBooleanField(JNIEnv* env, jobject obj,
                                              jfieldID fieldID) {
    return static_cast<MockableJNIEnv*>(env)->GetBooleanField(obj, fieldID);
  }
  static jbyte* JNICALL CallGetByteArrayElements(JNIEnv* env, jbyteArray array,
                                                 jboolean* isCopy) {
    return static_cast<MockableJNIEnv*>(env)->GetByteArrayElements(array,
                                                                   isCopy);
  }
  static void JNICALL CallGetByteArrayRegion(JNIEnv* env, jbyteArray array,
                                             jsize start, jsize len,
                                             jbyte* buf) {
    static_cast<MockableJNIEnv*>(env)->GetByteArrayRegion(array, start, len,
                                                          buf);
  }
  static jbyte JNICALL CallGetByteField(JNIEnv* env, jobject obj,
                                        jfieldID fieldID) {
    return static_cast<MockableJNIEnv*>(env)->GetByteField(obj, fieldID);
  }
  static jchar* JNICALL CallGetCharArrayElements(JNIEnv* env, jcharArray array,
                                                 jboolean* isCopy) {
    return static_cast<MockableJNIEnv*>(env)->GetCharArrayElements(array,
                                                                   isCopy);
  }
  static void JNICALL CallGetCharArrayRegion(JNIEnv* env, jcharArray array,
                                             jsize start, jsize len,
                                             jchar* buf) {
    static_cast<MockableJNIEnv*>(env)->GetCharArrayRegion(array, start, len,
                                                          buf);
  }
  static jchar JNICALL CallGetCharField(JNIEnv* env, jobject obj,
                                        jfieldID fieldID) {
    return static_cast<MockableJNIEnv*>(env)->GetCharField(obj, fieldID);
  }
  static void* JNICALL CallGetDirectBufferAddress(JNIEnv* env, jobject buf) {
    return static_cast<MockableJNIEnv*>(env)->GetDirectBufferAddress(buf);
  }
  static jlong JNICALL CallGetDirectBufferCapacity(JNIEnv* env, jobject buf) {
    return static_cast<MockableJNIEnv*>(env)->GetDirectBufferCapacity(buf);
  }
  static jdouble* JNICALL CallGetDoubleArrayElements(JNIEnv* env,
                                                     jdoubleArray array,
                                                     jboolean* isCopy) {
    return static_cast<MockableJNIEnv*>(env)->GetDoubleArrayElements(array,
                                                                     isCopy);
  }
  static void JNICALL CallGetDoubleArrayRegion(JNIEnv* env, jdoubleArray array,
                                               jsize start, jsize len,
                                               jdouble* buf) {
    static_cast<MockableJNIEnv*>(env)->GetDoubleArrayRegion(array, start, len,
                                                            buf);
  }
  static jdouble JNICALL CallGetDoubleField(JNIEnv* env, jobject obj,
                                            jfieldID fieldID) {
    return static_cast<MockableJNIEnv*>(env)->GetDoubleField(obj, fieldID);
  }
  static jfieldID JNICALL CallGetFieldID(JNIEnv* env, jclass clazz,
                                         const char* name, const char* sig) {
    return static_cast<MockableJNIEnv*>(env)->GetFieldID(clazz, name, sig);
  }
  static jfloat* JNICALL CallGetFloatArrayElements(JNIEnv* env,
                                                   jfloatArray array,
                                                   jboolean* isCopy) {
    return static_cast<MockableJNIEnv*>(env)->GetFloatArrayElements(array,
                                                                    isCopy);
  }
  static void JNICALL CallGetFloatArrayRegion(JNIEnv* env, jfloatArray array,
                                              jsize start, jsize len,
                                              jfloat* buf) {
    static_cast<MockableJNIEnv*>(env)->GetFloatArrayRegion(array, start, len,
                                                           buf);
  }
  static jfloat JNICALL CallGetFloatField(JNIEnv* env, jobject obj,
                                          jfieldID fieldID) {
    return static_cast<MockableJNIEnv*>(env)->GetFloatField(obj, fieldID);
  }
  static jint* JNICALL CallGetIntArrayElements(JNIEnv* env, jintArray array,
                                               jboolean* isCopy) {
    return static_cast<MockableJNIEnv*>(env)->GetIntArrayElements(array,
                                                                  isCopy);
  }
  static void JNICALL CallGetIntArrayRegion(JNIEnv* env, jintArray array,
                                            jsize start, jsize len, jint* buf) {
    static_cast<MockableJNIEnv*>(env)->GetIntArrayRegion(array, start, len,
                                                         buf);
  }
  static jint JNICALL CallGetIntField(JNIEnv* env, jobject obj,
                                      jfieldID fieldID) {
    return static_cast<MockableJNIEnv*>(env)->GetIntField(obj, fieldID);
  }
  static jint JNICALL CallGetJavaVM(JNIEnv* env, JavaVM** vm) {
    return static_cast<MockableJNIEnv*>(env)->GetJavaVM(vm);
  }
  static jlong* JNICALL CallGetLongArrayElements(JNIEnv* env, jlongArray array,
                                                 jboolean* isCopy) {
    return static_cast<MockableJNIEnv*>(env)->GetLongArrayElements(array,
                                                                   isCopy);
  }
  static void JNICALL CallGetLongArrayRegion(JNIEnv* env, jlongArray array,
                                             jsize start, jsize len,
                                             jlong* buf) {
    static_cast<MockableJNIEnv*>(env)->GetLongArrayRegion(array, start, len,
                                                          buf);
  }
  static jlong JNICALL CallGetLongField(JNIEnv* env, jobject obj,
                                        jfieldID fieldID) {
    return static_cast<MockableJNIEnv*>(env)->GetLongField(obj, fieldID);
  }
  static jmethodID JNICALL CallGetMethodID(JNIEnv* env, jclass clazz,
                                           const char* name, const char* sig) {
    return static_cast<MockableJNIEnv*>(env)->GetMethodID(clazz, name, sig);
  }
  static jobject JNICALL CallGetObjectArrayElement(JNIEnv* env,
                                                   jobjectArray array,
                                                   jsize index) {
    return static_cast<MockableJNIEnv*>(env)->GetObjectArrayElement(array,
                                                                    index);
  }
  static jclass JNICALL CallGetObjectClass(JNIEnv* env, jobject obj) {
    return static_cast<MockableJNIEnv*>(env)->GetObjectClass(obj);
  }
  static jobject JNICALL CallGetObjectField(JNIEnv* env, jobject obj,
                                            jfieldID fieldID) {
    return static_cast<MockableJNIEnv*>(env)->GetObjectField(obj, fieldID);
  }
  static jobjectRefType JNICALL CallGetObjectRefType(JNIEnv* env, jobject obj) {
    return static_cast<MockableJNIEnv*>(env)->GetObjectRefType(obj);
  }
  static void* JNICALL CallGetPrimitiveArrayCritical(JNIEnv* env, jarray array,
                                                     jboolean* isCopy) {
    return static_cast<MockableJNIEnv*>(env)->GetPrimitiveArrayCritical(array,
                                                                        isCopy);
  }
  static jshort* JNICALL CallGetShortArrayElements(JNIEnv* env,
                                                   jshortArray array,
                                                   jboolean* isCopy) {
    return static_cast<MockableJNIEnv*>(env)->GetShortArrayElements(array,
                                                                    isCopy);
  }
  static void JNICALL CallGetShortArrayRegion(JNIEnv* env, jshortArray array,
                                              jsize start, jsize len,
                                              jshort* buf) {
    static_cast<MockableJNIEnv*>(env)->GetShortArrayRegion(array, start, len,
                                                           buf);
  }
  static jshort JNICALL CallGetShortField(JNIEnv* env, jobject obj,
                                          jfieldID fieldID) {
    return static_cast<MockableJNIEnv*>(env)->GetShortField(obj, fieldID);
  }
  static jboolean JNICALL CallGetStaticBooleanField(JNIEnv* env, jclass clazz,
                                                    jfieldID fieldID) {
    return static_cast<MockableJNIEnv*>(env)->GetStaticBooleanField(clazz,
                                                                    fieldID);
  }
  static jbyte JNICALL CallGetStaticByteField(JNIEnv* env, jclass clazz,
                                              jfieldID fieldID) {
    return static_cast<MockableJNIEnv*>(env)->GetStaticByteField(clazz,
                                                                 fieldID);
  }
  static jchar JNICALL CallGetStaticCharField(JNIEnv* env, jclass clazz,
                                              jfieldID fieldID) {
    return static_cast<MockableJNIEnv*>(env)->GetStaticCharField(clazz,
                                                                 fieldID);
  }
  static jdouble JNICALL CallGetStaticDoubleField(JNIEnv* env, jclass clazz,
                                                  jfieldID fieldID) {
    return static_cast<MockableJNIEnv*>(env)->GetStaticDoubleField(clazz,
                                                                   fieldID);
  }
  static jfieldID JNICALL CallGetStaticFieldID(JNIEnv* env, jclass clazz,
                                               const char* name,
                                               const char* sig) {
    return static_cast<MockableJNIEnv*>(env)->GetStaticFieldID(clazz, name,
                                                               sig);
  }
  static jfloat JNICALL CallGetStaticFloatField(JNIEnv* env, jclass clazz,
                                                jfieldID fieldID) {
    return static_cast<MockableJNIEnv*>(env)->GetStaticFloatField(clazz,
                                                                  fieldID);
  }
  static jint JNICALL CallGetStaticIntField(JNIEnv* env, jclass clazz,
                                            jfieldID fieldID) {
    return static_cast<MockableJNIEnv*>(env)->GetStaticIntField(clazz, fieldID);
  }
  static jlong JNICALL CallGetStaticLongField(JNIEnv* env, jclass clazz,
                                              jfieldID fieldID) {
    return static_cast<MockableJNIEnv*>(env)->GetStaticLongField(clazz,
                                                                 fieldID);
  }
  static jmethodID JNICALL CallGetStaticMethodID(JNIEnv* env, jclass clazz,
                                                 const char* name,
                                                 const char* sig) {
    return static_cast<MockableJNIEnv*>(env)->GetStaticMethodID(clazz, name,
                                                                sig);
  }
  static jobject JNICALL CallGetStaticObjectField(JNIEnv* env, jclass clazz,
                                                  jfieldID fieldID) {
    return static_cast<MockableJNIEnv*>(env)->GetStaticObjectField(clazz,
                                                                   fieldID);
  }
  static jshort JNICALL CallGetStaticShortField(JNIEnv* env, jclass clazz,
                                                jfieldID fieldID) {
    return static_cast<MockableJNIEnv*>(env)->GetStaticShortField(clazz,
                                                                  fieldID);
  }
  static jsize JNICALL CallGetStringLength(JNIEnv* env, jstring str) {
    return static_cast<MockableJNIEnv*>(env)->GetStringLength(str);
  }
  static void JNICALL CallGetStringRegion(JNIEnv* env, jstring str, jsize start,
                                          jsize len, jchar* buf) {
    static_cast<MockableJNIEnv*>(env)->GetStringRegion(str, start, len, buf);
  }
  static const char* JNICALL CallGetStringUTFChars(JNIEnv* env, jstring str,
                                                   jboolean* isCopy) {
    return static_cast<MockableJNIEnv*>(env)->GetStringUTFChars(str, isCopy);
  }
  static jsize JNICALL CallGetStringUTFLength(JNIEnv* env, jstring str) {
    return static_cast<MockableJNIEnv*>(env)->GetStringUTFLength(str);
  }
  static void JNICALL CallGetStringUTFRegion(JNIEnv* env, jstring str,
                                             jsize start, jsize len,
                                             char* buf) {
    static_cast<MockableJNIEnv*>(env)->GetStringUTFRegion(str, start, len, buf);
  }
  static jclass JNICALL CallGetSuperclass(JNIEnv* env, jclass sub) {
    return static_cast<MockableJNIEnv*>(env)->GetSuperclass(sub);
  }
  static jint JNICALL CallGetVersion(JNIEnv* env) {
    return static_cast<MockableJNIEnv*>(env)->GetVersion();
  }
  static jboolean JNICALL CallIsAssignableFrom(JNIEnv* env, jclass sub,
                                               jclass sup) {
    return static_cast<MockableJNIEnv*>(env)->IsAssignableFrom(sub, sup);
  }
  static jboolean JNICALL CallIsInstanceOf(JNIEnv* env, jobject obj,
                                           jclass clazz) {
    return static_cast<MockableJNIEnv*>(env)->IsInstanceOf(obj, clazz);
  }
  static jboolean JNICALL CallIsSameObject(JNIEnv* env, jobject obj1,
                                           jobject obj2) {
    return static_cast<MockableJNIEnv*>(env)->IsSameObject(obj1, obj2);
  }
  static const jchar* JNICALL CallGetStringChars(JNIEnv* env, jstring str,
                                                 jboolean* isCopy) {
    return static_cast<MockableJNIEnv*>(env)->GetStringChars(str, isCopy);
  }
  static const jchar* JNICALL CallGetStringCritical(JNIEnv* env, jstring str,
                                                    jboolean* isCopy) {
    return static_cast<MockableJNIEnv*>(env)->GetStringCritical(str, isCopy);
  }
  static jint JNICALL CallMonitorEnter(JNIEnv* env, jobject obj) {
    return static_cast<MockableJNIEnv*>(env)->MonitorEnter(obj);
  }
  static jint JNICALL CallMonitorExit(JNIEnv* env, jobject obj) {
    return static_cast<MockableJNIEnv*>(env)->MonitorExit(obj);
  }
  static jbooleanArray JNICALL CallNewBooleanArray(JNIEnv* env, jsize len) {
    return static_cast<MockableJNIEnv*>(env)->NewBooleanArray(len);
  }
  static jbyteArray JNICALL CallNewByteArray(JNIEnv* env, jsize len) {
    return static_cast<MockableJNIEnv*>(env)->NewByteArray(len);
  }
  static jcharArray JNICALL CallNewCharArray(JNIEnv* env, jsize len) {
    return static_cast<MockableJNIEnv*>(env)->NewCharArray(len);
  }
  static jobject JNICALL CallNewDirectByteBuffer(JNIEnv* env, void* address,
                                                 jlong capacity) {
    return static_cast<MockableJNIEnv*>(env)->NewDirectByteBuffer(address,
                                                                  capacity);
  }
  static jdoubleArray JNICALL CallNewDoubleArray(JNIEnv* env, jsize len) {
    return static_cast<MockableJNIEnv*>(env)->NewDoubleArray(len);
  }
  static jfloatArray JNICALL CallNewFloatArray(JNIEnv* env, jsize len) {
    return static_cast<MockableJNIEnv*>(env)->NewFloatArray(len);
  }
  static jobject JNICALL CallNewGlobalRef(JNIEnv* env, jobject lobj) {
    return static_cast<MockableJNIEnv*>(env)->NewGlobalRef(lobj);
  }
  static jintArray JNICALL CallNewIntArray(JNIEnv* env, jsize len) {
    return static_cast<MockableJNIEnv*>(env)->NewIntArray(len);
  }
  static jobject JNICALL CallNewLocalRef(JNIEnv* env, jobject ref) {
    return static_cast<MockableJNIEnv*>(env)->NewLocalRef(ref);
  }
  static jlongArray JNICALL CallNewLongArray(JNIEnv* env, jsize len) {
    return static_cast<MockableJNIEnv*>(env)->NewLongArray(len);
  }
  static jobject JNICALL CallNewObjectA(JNIEnv* env, jclass clazz,
                                        jmethodID methodID,
                                        InputJvalueArray args) {
    return static_cast<MockableJNIEnv*>(env)->NewObjectA(clazz, methodID, args);
  }
  static jobjectArray JNICALL CallNewObjectArray(JNIEnv* env, jsize len,
                                                 jclass clazz, jobject init) {
    return static_cast<MockableJNIEnv*>(env)->NewObjectArray(len, clazz, init);
  }
  static jobject JNICALL CallNewObjectV(JNIEnv* env, jclass clazz,
                                        jmethodID methodID, va_list args) {
    return static_cast<MockableJNIEnv*>(env)->NewObjectV(clazz, methodID, args);
  }
  static jshortArray JNICALL CallNewShortArray(JNIEnv* env, jsize len) {
    return static_cast<MockableJNIEnv*>(env)->NewShortArray(len);
  }
  static jstring JNICALL CallNewString(JNIEnv* env, const jchar* unicode,
                                       jsize len) {
    return static_cast<MockableJNIEnv*>(env)->NewString(unicode, len);
  }
  static jstring JNICALL CallNewStringUTF(JNIEnv* env, const char* utf) {
    return static_cast<MockableJNIEnv*>(env)->NewStringUTF(utf);
  }
  static jweak JNICALL CallNewWeakGlobalRef(JNIEnv* env, jobject obj) {
    return static_cast<MockableJNIEnv*>(env)->NewWeakGlobalRef(obj);
  }
  static jobject JNICALL CallPopLocalFrame(JNIEnv* env, jobject result) {
    return static_cast<MockableJNIEnv*>(env)->PopLocalFrame(result);
  }
  static jint JNICALL CallPushLocalFrame(JNIEnv* env, jint capacity) {
    return static_cast<MockableJNIEnv*>(env)->PushLocalFrame(capacity);
  }
  static jint JNICALL CallRegisterNatives(JNIEnv* env, jclass clazz,
                                          const JNINativeMethod* methods,
                                          jint nMethods) {
    return static_cast<MockableJNIEnv*>(env)->RegisterNatives(clazz, methods,
                                                              nMethods);
  }
  static void JNICALL CallReleaseBooleanArrayElements(JNIEnv* env,
                                                      jbooleanArray array,
                                                      jboolean* elems,
                                                      jint mode) {
    static_cast<MockableJNIEnv*>(env)->ReleaseBooleanArrayElements(array, elems,
                                                                   mode);
  }
  static void JNICALL CallReleaseByteArrayElements(JNIEnv* env,
                                                   jbyteArray array,
                                                   jbyte* elems, jint mode) {
    static_cast<MockableJNIEnv*>(env)->ReleaseByteArrayElements(array, elems,
                                                                mode);
  }
  static void JNICALL CallReleaseCharArrayElements(JNIEnv* env,
                                                   jcharArray array,
                                                   jchar* elems, jint mode) {
    static_cast<MockableJNIEnv*>(env)->ReleaseCharArrayElements(array, elems,
                                                                mode);
  }
  static void JNICALL CallReleaseDoubleArrayElements(JNIEnv* env,
                                                     jdoubleArray array,
                                                     jdouble* elems,
                                                     jint mode) {
    static_cast<MockableJNIEnv*>(env)->ReleaseDoubleArrayElements(array, elems,
                                                                  mode);
  }
  static void JNICALL CallReleaseFloatArrayElements(JNIEnv* env,
                                                    jfloatArray array,
                                                    jfloat* elems, jint mode) {
    static_cast<MockableJNIEnv*>(env)->ReleaseFloatArrayElements(array, elems,
                                                                 mode);
  }
  static void JNICALL CallReleaseIntArrayElements(JNIEnv* env, jintArray array,
                                                  jint* elems, jint mode) {
    static_cast<MockableJNIEnv*>(env)->ReleaseIntArrayElements(array, elems,
                                                               mode);
  }
  static void JNICALL CallReleaseLongArrayElements(JNIEnv* env,
                                                   jlongArray array,
                                                   jlong* elems, jint mode) {
    static_cast<MockableJNIEnv*>(env)->ReleaseLongArrayElements(array, elems,
                                                                mode);
  }
  static void JNICALL CallReleasePrimitiveArrayCritical(JNIEnv* env,
                                                        jarray array,
                                                        void* carray,
                                                        jint mode) {
    static_cast<MockableJNIEnv*>(env)->ReleasePrimitiveArrayCritical(
        array, carray, mode);
  }
  static void JNICALL CallReleaseShortArrayElements(JNIEnv* env,
                                                    jshortArray array,
                                                    jshort* elems, jint mode) {
    static_cast<MockableJNIEnv*>(env)->ReleaseShortArrayElements(array, elems,
                                                                 mode);
  }
  static void JNICALL CallReleaseStringChars(JNIEnv* env, jstring str,
                                             const jchar* chars) {
    static_cast<MockableJNIEnv*>(env)->ReleaseStringChars(str, chars);
  }
  static void JNICALL CallReleaseStringCritical(JNIEnv* env, jstring str,
                                                const jchar* cstring) {
    static_cast<MockableJNIEnv*>(env)->ReleaseStringCritical(str, cstring);
  }
  static void JNICALL CallReleaseStringUTFChars(JNIEnv* env, jstring str,
                                                const char* chars) {
    static_cast<MockableJNIEnv*>(env)->ReleaseStringUTFChars(str, chars);
  }
  static void JNICALL CallSetBooleanArrayRegion(JNIEnv* env,
                                                jbooleanArray array,
                                                jsize start, jsize len,
                                                const jboolean* buf) {
    static_cast<MockableJNIEnv*>(env)->SetBooleanArrayRegion(array, start, len,
                                                             buf);
  }
  static void JNICALL CallSetBooleanField(JNIEnv* env, jobject obj,
                                          jfieldID fieldID, jboolean val) {
    static_cast<MockableJNIEnv*>(env)->SetBooleanField(obj, fieldID, val);
  }
  static void JNICALL CallSetByteArrayRegion(JNIEnv* env, jbyteArray array,
                                             jsize start, jsize len,
                                             const jbyte* buf) {
    static_cast<MockableJNIEnv*>(env)->SetByteArrayRegion(array, start, len,
                                                          buf);
  }
  static void JNICALL CallSetByteField(JNIEnv* env, jobject obj,
                                       jfieldID fieldID, jbyte val) {
    static_cast<MockableJNIEnv*>(env)->SetByteField(obj, fieldID, val);
  }
  static void JNICALL CallSetCharArrayRegion(JNIEnv* env, jcharArray array,
                                             jsize start, jsize len,
                                             const jchar* buf) {
    static_cast<MockableJNIEnv*>(env)->SetCharArrayRegion(array, start, len,
                                                          buf);
  }
  static void JNICALL CallSetCharField(JNIEnv* env, jobject obj,
                                       jfieldID fieldID, jchar val) {
    static_cast<MockableJNIEnv*>(env)->SetCharField(obj, fieldID, val);
  }
  static void JNICALL CallSetDoubleArrayRegion(JNIEnv* env, jdoubleArray array,
                                               jsize start, jsize len,
                                               const jdouble* buf) {
    static_cast<MockableJNIEnv*>(env)->SetDoubleArrayRegion(array, start, len,
                                                            buf);
  }
  static void JNICALL CallSetDoubleField(JNIEnv* env, jobject obj,
                                         jfieldID fieldID, jdouble val) {
    static_cast<MockableJNIEnv*>(env)->SetDoubleField(obj, fieldID, val);
  }
  static void JNICALL CallSetFloatArrayRegion(JNIEnv* env, jfloatArray array,
                                              jsize start, jsize len,
                                              const jfloat* buf) {
    static_cast<MockableJNIEnv*>(env)->SetFloatArrayRegion(array, start, len,
                                                           buf);
  }
  static void JNICALL CallSetFloatField(JNIEnv* env, jobject obj,
                                        jfieldID fieldID, jfloat val) {
    static_cast<MockableJNIEnv*>(env)->SetFloatField(obj, fieldID, val);
  }
  static void JNICALL CallSetIntArrayRegion(JNIEnv* env, jintArray array,
                                            jsize start, jsize len,
                                            const jint* buf) {
    static_cast<MockableJNIEnv*>(env)->SetIntArrayRegion(array, start, len,
                                                         buf);
  }
  static void JNICALL CallSetIntField(JNIEnv* env, jobject obj,
                                      jfieldID fieldID, jint val) {
    static_cast<MockableJNIEnv*>(env)->SetIntField(obj, fieldID, val);
  }
  static void JNICALL CallSetLongArrayRegion(JNIEnv* env, jlongArray array,
                                             jsize start, jsize len,
                                             const jlong* buf) {
    static_cast<MockableJNIEnv*>(env)->SetLongArrayRegion(array, start, len,
                                                          buf);
  }
  static void JNICALL CallSetLongField(JNIEnv* env, jobject obj,
                                       jfieldID fieldID, jlong val) {
    static_cast<MockableJNIEnv*>(env)->SetLongField(obj, fieldID, val);
  }
  static void JNICALL CallSetObjectArrayElement(JNIEnv* env, jobjectArray array,
                                                jsize index, jobject val) {
    static_cast<MockableJNIEnv*>(env)->SetObjectArrayElement(array, index, val);
  }
  static void JNICALL CallSetObjectField(JNIEnv* env, jobject obj,
                                         jfieldID fieldID, jobject val) {
    static_cast<MockableJNIEnv*>(env)->SetObjectField(obj, fieldID, val);
  }
  static void JNICALL CallSetShortArrayRegion(JNIEnv* env, jshortArray array,
                                              jsize start, jsize len,
                                              const jshort* buf) {
    static_cast<MockableJNIEnv*>(env)->SetShortArrayRegion(array, start, len,
                                                           buf);
  }
  static void JNICALL CallSetShortField(JNIEnv* env, jobject obj,
                                        jfieldID fieldID, jshort val) {
    static_cast<MockableJNIEnv*>(env)->SetShortField(obj, fieldID, val);
  }
  static void JNICALL CallSetStaticBooleanField(JNIEnv* env, jclass clazz,
                                                jfieldID fieldID,
                                                jboolean value) {
    static_cast<MockableJNIEnv*>(env)->SetStaticBooleanField(clazz, fieldID,
                                                             value);
  }
  static void JNICALL CallSetStaticByteField(JNIEnv* env, jclass clazz,
                                             jfieldID fieldID, jbyte value) {
    static_cast<MockableJNIEnv*>(env)->SetStaticByteField(clazz, fieldID,
                                                          value);
  }
  static void JNICALL CallSetStaticCharField(JNIEnv* env, jclass clazz,
                                             jfieldID fieldID, jchar value) {
    static_cast<MockableJNIEnv*>(env)->SetStaticCharField(clazz, fieldID,
                                                          value);
  }
  static void JNICALL CallSetStaticDoubleField(JNIEnv* env, jclass clazz,
                                               jfieldID fieldID,
                                               jdouble value) {
    static_cast<MockableJNIEnv*>(env)->SetStaticDoubleField(clazz, fieldID,
                                                            value);
  }
  static void JNICALL CallSetStaticFloatField(JNIEnv* env, jclass clazz,
                                              jfieldID fieldID, jfloat value) {
    static_cast<MockableJNIEnv*>(env)->SetStaticFloatField(clazz, fieldID,
                                                           value);
  }
  static void JNICALL CallSetStaticIntField(JNIEnv* env, jclass clazz,
                                            jfieldID fieldID, jint value) {
    static_cast<MockableJNIEnv*>(env)->SetStaticIntField(clazz, fieldID, value);
  }
  static void JNICALL CallSetStaticLongField(JNIEnv* env, jclass clazz,
                                             jfieldID fieldID, jlong value) {
    static_cast<MockableJNIEnv*>(env)->SetStaticLongField(clazz, fieldID,
                                                          value);
  }
  static void JNICALL CallSetStaticObjectField(JNIEnv* env, jclass clazz,
                                               jfieldID fieldID,
                                               jobject value) {
    static_cast<MockableJNIEnv*>(env)->SetStaticObjectField(clazz, fieldID,
                                                            value);
  }
  static void JNICALL CallSetStaticShortField(JNIEnv* env, jclass clazz,
                                              jfieldID fieldID, jshort value) {
    static_cast<MockableJNIEnv*>(env)->SetStaticShortField(clazz, fieldID,
                                                           value);
  }
  static jint JNICALL CallThrow(JNIEnv* env, jthrowable obj) {
    return static_cast<MockableJNIEnv*>(env)->Throw(obj);
  }
  static jint JNICALL CallThrowNew(JNIEnv* env, jclass clazz, const char* msg) {
    return static_cast<MockableJNIEnv*>(env)->ThrowNew(clazz, msg);
  }
  static jobject JNICALL CallToReflectedField(JNIEnv* env, jclass cls,
                                              jfieldID fieldID,
                                              jboolean isStatic) {
    return static_cast<MockableJNIEnv*>(env)->ToReflectedField(cls, fieldID,
                                                               isStatic);
  }
  static jobject JNICALL CallToReflectedMethod(JNIEnv* env, jclass cls,
                                               jmethodID methodID,
                                               jboolean isStatic) {
    return static_cast<MockableJNIEnv*>(env)->ToReflectedMethod(cls, methodID,
                                                                isStatic);
  }
  static jint JNICALL CallUnregisterNatives(JNIEnv* env, jclass clazz) {
    return static_cast<MockableJNIEnv*>(env)->UnregisterNatives(clazz);
  }

  JNINativeInterface functions_;
};

class MockJNIEnv : public MockableJNIEnv {
 public:
  MOCK_METHOD(jobject, AllocObject, (jclass clazz), (override));
  MOCK_METHOD(jboolean, CallBooleanMethodV,
              (jobject obj, jmethodID methodID, va_list args), (override));
  MOCK_METHOD(jbyte, CallByteMethodV,
              (jobject obj, jmethodID methodID, va_list args), (override));
  MOCK_METHOD(jchar, CallCharMethodV,
              (jobject obj, jmethodID methodID, va_list args), (override));
  MOCK_METHOD(jdouble, CallDoubleMethodV,
              (jobject obj, jmethodID methodID, va_list args), (override));
  MOCK_METHOD(jfloat, CallFloatMethodV,
              (jobject obj, jmethodID methodID, va_list args), (override));
  MOCK_METHOD(jint, CallIntMethodV,
              (jobject obj, jmethodID methodID, va_list args), (override));
  MOCK_METHOD(jlong, CallLongMethodV,
              (jobject obj, jmethodID methodID, va_list args), (override));
  MOCK_METHOD(jboolean, CallNonvirtualBooleanMethodV,
              (jobject obj, jclass clazz, jmethodID methodID, va_list args),
              (override));
  MOCK_METHOD(jbyte, CallNonvirtualByteMethodV,
              (jobject obj, jclass clazz, jmethodID methodID, va_list args),
              (override));
  MOCK_METHOD(jchar, CallNonvirtualCharMethodV,
              (jobject obj, jclass clazz, jmethodID methodID, va_list args),
              (override));
  MOCK_METHOD(jdouble, CallNonvirtualDoubleMethodV,
              (jobject obj, jclass clazz, jmethodID methodID, va_list args),
              (override));
  MOCK_METHOD(jfloat, CallNonvirtualFloatMethodV,
              (jobject obj, jclass clazz, jmethodID methodID, va_list args),
              (override));
  MOCK_METHOD(jint, CallNonvirtualIntMethodV,
              (jobject obj, jclass clazz, jmethodID methodID, va_list args),
              (override));
  MOCK_METHOD(jlong, CallNonvirtualLongMethodV,
              (jobject obj, jclass clazz, jmethodID methodID, va_list args),
              (override));
  MOCK_METHOD(jobject, CallNonvirtualObjectMethodV,
              (jobject obj, jclass clazz, jmethodID methodID, va_list args),
              (override));
  MOCK_METHOD(jshort, CallNonvirtualShortMethodV,
              (jobject obj, jclass clazz, jmethodID methodID, va_list args),
              (override));
  MOCK_METHOD(void, CallNonvirtualVoidMethodV,
              (jobject obj, jclass clazz, jmethodID methodID, va_list args),
              (override));
  MOCK_METHOD(jobject, CallObjectMethodV,
              (jobject obj, jmethodID methodID, va_list args), (override));
  MOCK_METHOD(jshort, CallShortMethodV,
              (jobject obj, jmethodID methodID, va_list args), (override));
  MOCK_METHOD(jboolean, CallStaticBooleanMethodV,
              (jclass clazz, jmethodID methodID, va_list args), (override));
  MOCK_METHOD(jbyte, CallStaticByteMethodV,
              (jclass clazz, jmethodID methodID, va_list args), (override));
  MOCK_METHOD(jchar, CallStaticCharMethodV,
              (jclass clazz, jmethodID methodID, va_list args), (override));
  MOCK_METHOD(jdouble, CallStaticDoubleMethodV,
              (jclass clazz, jmethodID methodID, va_list args), (override));
  MOCK_METHOD(jfloat, CallStaticFloatMethodV,
              (jclass clazz, jmethodID methodID, va_list args), (override));
  MOCK_METHOD(jint, CallStaticIntMethodV,
              (jclass clazz, jmethodID methodID, va_list args), (override));
  MOCK_METHOD(jlong, CallStaticLongMethodV,
              (jclass clazz, jmethodID methodID, va_list args), (override));
  MOCK_METHOD(jobject, CallStaticObjectMethodV,
              (jclass clazz, jmethodID methodID, va_list args), (override));
  MOCK_METHOD(jshort, CallStaticShortMethodV,
              (jclass clazz, jmethodID methodID, va_list args), (override));
  MOCK_METHOD(void, CallStaticVoidMethodV,
              (jclass cls, jmethodID methodID, va_list args), (override));
  MOCK_METHOD(void, CallVoidMethodV,
              (jobject obj, jmethodID methodID, va_list args), (override));
  MOCK_METHOD(jclass, DefineClass,
              (const char* name, jobject loader, const jbyte* buf, jsize len),
              (override));
  MOCK_METHOD(void, DeleteGlobalRef, (jobject gref), (override));
  MOCK_METHOD(void, DeleteLocalRef, (jobject obj), (override));
  MOCK_METHOD(void, DeleteWeakGlobalRef, (jweak ref), (override));
  MOCK_METHOD(jint, EnsureLocalCapacity, (jint capacity), (override));
  MOCK_METHOD(jboolean, ExceptionCheck, (), (override));
  MOCK_METHOD(void, ExceptionClear, (), (override));
  MOCK_METHOD(void, ExceptionDescribe, (), (override));
  MOCK_METHOD(jthrowable, ExceptionOccurred, (), (override));
  MOCK_METHOD(void, FatalError, (const char* msg), (override));
  MOCK_METHOD(jclass, FindClass, (const char* name), (override));
  MOCK_METHOD(jfieldID, FromReflectedField, (jobject field), (override));
  MOCK_METHOD(jmethodID, FromReflectedMethod, (jobject method), (override));
  MOCK_METHOD(jsize, GetArrayLength, (jarray array), (override));
  MOCK_METHOD(jboolean*, GetBooleanArrayElements,
              (jbooleanArray array, jboolean* isCopy), (override));
  MOCK_METHOD(void, GetBooleanArrayRegion,
              (jbooleanArray array, jsize start, jsize len, jboolean* buf),
              (override));
  MOCK_METHOD(jboolean, GetBooleanField, (jobject obj, jfieldID fieldID),
              (override));
  MOCK_METHOD(jbyte*, GetByteArrayElements,
              (jbyteArray array, jboolean* isCopy), (override));
  MOCK_METHOD(void, GetByteArrayRegion,
              (jbyteArray array, jsize start, jsize len, jbyte* buf),
              (override));
  MOCK_METHOD(jbyte, GetByteField, (jobject obj, jfieldID fieldID), (override));
  MOCK_METHOD(jchar*, GetCharArrayElements,
              (jcharArray array, jboolean* isCopy), (override));
  MOCK_METHOD(void, GetCharArrayRegion,
              (jcharArray array, jsize start, jsize len, jchar* buf),
              (override));
  MOCK_METHOD(jchar, GetCharField, (jobject obj, jfieldID fieldID), (override));
  MOCK_METHOD(void*, GetDirectBufferAddress, (jobject buf), (override));
  MOCK_METHOD(jlong, GetDirectBufferCapacity, (jobject buf), (override));
  MOCK_METHOD(jdouble*, GetDoubleArrayElements,
              (jdoubleArray array, jboolean* isCopy), (override));
  MOCK_METHOD(void, GetDoubleArrayRegion,
              (jdoubleArray array, jsize start, jsize len, jdouble* buf),
              (override));
  MOCK_METHOD(jdouble, GetDoubleField, (jobject obj, jfieldID fieldID),
              (override));
  MOCK_METHOD(jfieldID, GetFieldID,
              (jclass clazz, const char* name, const char* sig), (override));
  MOCK_METHOD(jfloat*, GetFloatArrayElements,
              (jfloatArray array, jboolean* isCopy), (override));
  MOCK_METHOD(void, GetFloatArrayRegion,
              (jfloatArray array, jsize start, jsize len, jfloat* buf),
              (override));
  MOCK_METHOD(jfloat, GetFloatField, (jobject obj, jfieldID fieldID),
              (override));
  MOCK_METHOD(jint*, GetIntArrayElements, (jintArray array, jboolean* isCopy),
              (override));
  MOCK_METHOD(void, GetIntArrayRegion,
              (jintArray array, jsize start, jsize len, jint* buf), (override));
  MOCK_METHOD(jint, GetIntField, (jobject obj, jfieldID fieldID), (override));
  MOCK_METHOD(jint, GetJavaVM, (JavaVM * *vm), (override));
  MOCK_METHOD(jlong*, GetLongArrayElements,
              (jlongArray array, jboolean* isCopy), (override));
  MOCK_METHOD(void, GetLongArrayRegion,
              (jlongArray array, jsize start, jsize len, jlong* buf),
              (override));
  MOCK_METHOD(jlong, GetLongField, (jobject obj, jfieldID fieldID), (override));
  MOCK_METHOD(jmethodID, GetMethodID,
              (jclass clazz, const char* name, const char* sig), (override));
  MOCK_METHOD(jobject, GetObjectArrayElement, (jobjectArray array, jsize index),
              (override));
  MOCK_METHOD(jclass, GetObjectClass, (jobject obj), (override));
  MOCK_METHOD(jobject, GetObjectField, (jobject obj, jfieldID fieldID),
              (override));
  MOCK_METHOD(jobjectRefType, GetObjectRefType, (jobject obj), (override));
  MOCK_METHOD(void*, GetPrimitiveArrayCritical,
              (jarray array, jboolean* isCopy), (override));
  MOCK_METHOD(jshort*, GetShortArrayElements,
              (jshortArray array, jboolean* isCopy), (override));
  MOCK_METHOD(void, GetShortArrayRegion,
              (jshortArray array, jsize start, jsize len, jshort* buf),
              (override));
  MOCK_METHOD(jshort, GetShortField, (jobject obj, jfieldID fieldID),
              (override));
  MOCK_METHOD(jboolean, GetStaticBooleanField, (jclass clazz, jfieldID fieldID),
              (override));
  MOCK_METHOD(jbyte, GetStaticByteField, (jclass clazz, jfieldID fieldID),
              (override));
  MOCK_METHOD(jchar, GetStaticCharField, (jclass clazz, jfieldID fieldID),
              (override));
  MOCK_METHOD(jdouble, GetStaticDoubleField, (jclass clazz, jfieldID fieldID),
              (override));
  MOCK_METHOD(jfieldID, GetStaticFieldID,
              (jclass clazz, const char* name, const char* sig), (override));
  MOCK_METHOD(jfloat, GetStaticFloatField, (jclass clazz, jfieldID fieldID),
              (override));
  MOCK_METHOD(jint, GetStaticIntField, (jclass clazz, jfieldID fieldID),
              (override));
  MOCK_METHOD(jlong, GetStaticLongField, (jclass clazz, jfieldID fieldID),
              (override));
  MOCK_METHOD(jmethodID, GetStaticMethodID,
              (jclass clazz, const char* name, const char* sig), (override));
  MOCK_METHOD(jobject, GetStaticObjectField, (jclass clazz, jfieldID fieldID),
              (override));
  MOCK_METHOD(jshort, GetStaticShortField, (jclass clazz, jfieldID fieldID),
              (override));
  MOCK_METHOD(jsize, GetStringLength, (jstring str), (override));
  MOCK_METHOD(void, GetStringRegion,
              (jstring str, jsize start, jsize len, jchar* buf), (override));
  MOCK_METHOD(const char*, GetStringUTFChars, (jstring str, jboolean* isCopy),
              (override));
  MOCK_METHOD(jsize, GetStringUTFLength, (jstring str), (override));
  MOCK_METHOD(void, GetStringUTFRegion,
              (jstring str, jsize start, jsize len, char* buf), (override));
  MOCK_METHOD(jclass, GetSuperclass, (jclass sub), (override));
  MOCK_METHOD(jint, GetVersion, (), (override));
  MOCK_METHOD(jboolean, IsAssignableFrom, (jclass sub, jclass sup), (override));
  MOCK_METHOD(jboolean, IsInstanceOf, (jobject obj, jclass clazz), (override));
  MOCK_METHOD(jboolean, IsSameObject, (jobject obj1, jobject obj2), (override));
  MOCK_METHOD(const jchar*, GetStringChars, (jstring str, jboolean* isCopy),
              (override));
  MOCK_METHOD(const jchar*, GetStringCritical, (jstring str, jboolean* isCopy),
              (override));
  MOCK_METHOD(jint, MonitorEnter, (jobject obj), (override));
  MOCK_METHOD(jint, MonitorExit, (jobject obj), (override));
  MOCK_METHOD(jbooleanArray, NewBooleanArray, (jsize len), (override));
  MOCK_METHOD(jbyteArray, NewByteArray, (jsize len), (override));
  MOCK_METHOD(jcharArray, NewCharArray, (jsize len), (override));
  MOCK_METHOD(jobject, NewDirectByteBuffer, (void* address, jlong capacity),
              (override));
  MOCK_METHOD(jdoubleArray, NewDoubleArray, (jsize len), (override));
  MOCK_METHOD(jfloatArray, NewFloatArray, (jsize len), (override));
  MOCK_METHOD(jobject, NewGlobalRef, (jobject lobj), (override));
  MOCK_METHOD(jintArray, NewIntArray, (jsize len), (override));
  MOCK_METHOD(jobject, NewLocalRef, (jobject ref), (override));
  MOCK_METHOD(jlongArray, NewLongArray, (jsize len), (override));
  MOCK_METHOD(jobject, NewObjectA,
              (jclass clazz, jmethodID methodID, InputJvalueArray args),
              (override));
  MOCK_METHOD(jobjectArray, NewObjectArray,
              (jsize len, jclass clazz, jobject init), (override));
  MOCK_METHOD(jobject, NewObjectV,
              (jclass clazz, jmethodID methodID, va_list args), (override));
  MOCK_METHOD(jshortArray, NewShortArray, (jsize len), (override));
  MOCK_METHOD(jstring, NewString, (const jchar* unicode, jsize len),
              (override));
  MOCK_METHOD(jstring, NewStringUTF, (const char* utf), (override));
  MOCK_METHOD(jweak, NewWeakGlobalRef, (jobject obj), (override));
  MOCK_METHOD(jobject, PopLocalFrame, (jobject result), (override));
  MOCK_METHOD(jint, PushLocalFrame, (jint capacity), (override));
  MOCK_METHOD(jint, RegisterNatives,
              (jclass clazz, const JNINativeMethod* methods, jint nMethods),
              (override));
  MOCK_METHOD(void, ReleaseBooleanArrayElements,
              (jbooleanArray array, jboolean* elems, jint mode), (override));
  MOCK_METHOD(void, ReleaseByteArrayElements,
              (jbyteArray array, jbyte* elems, jint mode), (override));
  MOCK_METHOD(void, ReleaseCharArrayElements,
              (jcharArray array, jchar* elems, jint mode), (override));
  MOCK_METHOD(void, ReleaseDoubleArrayElements,
              (jdoubleArray array, jdouble* elems, jint mode), (override));
  MOCK_METHOD(void, ReleaseFloatArrayElements,
              (jfloatArray array, jfloat* elems, jint mode), (override));
  MOCK_METHOD(void, ReleaseIntArrayElements,
              (jintArray array, jint* elems, jint mode), (override));
  MOCK_METHOD(void, ReleaseLongArrayElements,
              (jlongArray array, jlong* elems, jint mode), (override));
  MOCK_METHOD(void, ReleasePrimitiveArrayCritical,
              (jarray array, void* carray, jint mode), (override));
  MOCK_METHOD(void, ReleaseShortArrayElements,
              (jshortArray array, jshort* elems, jint mode), (override));
  MOCK_METHOD(void, ReleaseStringChars, (jstring str, const jchar* chars),
              (override));
  MOCK_METHOD(void, ReleaseStringCritical, (jstring str, const jchar* cstring),
              (override));
  MOCK_METHOD(void, ReleaseStringUTFChars, (jstring str, const char* chars),
              (override));
  MOCK_METHOD(void, SetBooleanArrayRegion,
              (jbooleanArray array, jsize start, jsize len,
               const jboolean* buf),
              (override));
  MOCK_METHOD(void, SetBooleanField,
              (jobject obj, jfieldID fieldID, jboolean val), (override));
  MOCK_METHOD(void, SetByteArrayRegion,
              (jbyteArray array, jsize start, jsize len, const jbyte* buf),
              (override));
  MOCK_METHOD(void, SetByteField, (jobject obj, jfieldID fieldID, jbyte val),
              (override));
  MOCK_METHOD(void, SetCharArrayRegion,
              (jcharArray array, jsize start, jsize len, const jchar* buf),
              (override));
  MOCK_METHOD(void, SetCharField, (jobject obj, jfieldID fieldID, jchar val),
              (override));
  MOCK_METHOD(void, SetDoubleArrayRegion,
              (jdoubleArray array, jsize start, jsize len, const jdouble* buf),
              (override));
  MOCK_METHOD(void, SetDoubleField,
              (jobject obj, jfieldID fieldID, jdouble val), (override));
  MOCK_METHOD(void, SetFloatArrayRegion,
              (jfloatArray array, jsize start, jsize len, const jfloat* buf),
              (override));
  MOCK_METHOD(void, SetFloatField, (jobject obj, jfieldID fieldID, jfloat val),
              (override));
  MOCK_METHOD(void, SetIntArrayRegion,
              (jintArray array, jsize start, jsize len, const jint* buf),
              (override));
  MOCK_METHOD(void, SetIntField, (jobject obj, jfieldID fieldID, jint val),
              (override));
  MOCK_METHOD(void, SetLongArrayRegion,
              (jlongArray array, jsize start, jsize len, const jlong* buf),
              (override));
  MOCK_METHOD(void, SetLongField, (jobject obj, jfieldID fieldID, jlong val),
              (override));
  MOCK_METHOD(void, SetObjectArrayElement,
              (jobjectArray array, jsize index, jobject val), (override));
  MOCK_METHOD(void, SetObjectField,
              (jobject obj, jfieldID fieldID, jobject val), (override));
  MOCK_METHOD(void, SetShortArrayRegion,
              (jshortArray array, jsize start, jsize len, const jshort* buf),
              (override));
  MOCK_METHOD(void, SetShortField, (jobject obj, jfieldID fieldID, jshort val),
              (override));
  MOCK_METHOD(void, SetStaticBooleanField,
              (jclass clazz, jfieldID fieldID, jboolean value), (override));
  MOCK_METHOD(void, SetStaticByteField,
              (jclass clazz, jfieldID fieldID, jbyte value), (override));
  MOCK_METHOD(void, SetStaticCharField,
              (jclass clazz, jfieldID fieldID, jchar value), (override));
  MOCK_METHOD(void, SetStaticDoubleField,
              (jclass clazz, jfieldID fieldID, jdouble value), (override));
  MOCK_METHOD(void, SetStaticFloatField,
              (jclass clazz, jfieldID fieldID, jfloat value), (override));
  MOCK_METHOD(void, SetStaticIntField,
              (jclass clazz, jfieldID fieldID, jint value), (override));
  MOCK_METHOD(void, SetStaticLongField,
              (jclass clazz, jfieldID fieldID, jlong value), (override));
  MOCK_METHOD(void, SetStaticObjectField,
              (jclass clazz, jfieldID fieldID, jobject value), (override));
  MOCK_METHOD(void, SetStaticShortField,
              (jclass clazz, jfieldID fieldID, jshort value), (override));
  MOCK_METHOD(jint, Throw, (jthrowable obj), (override));
  MOCK_METHOD(jint, ThrowNew, (jclass clazz, const char* msg), (override));
  MOCK_METHOD(jobject, ToReflectedField,
              (jclass cls, jfieldID fieldID, jboolean isStatic), (override));
  MOCK_METHOD(jobject, ToReflectedMethod,
              (jclass cls, jmethodID methodID, jboolean isStatic), (override));
  MOCK_METHOD(jint, UnregisterNatives, (jclass clazz), (override));
};
#endif  // AOSP_NFC_MOCK_JNI_ENV_H
