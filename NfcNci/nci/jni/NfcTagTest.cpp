#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <statslog_nfc.h>

#include "NfcTag.h"
#include "mock_jni_env.h"
#include "nfc_api.h"

using AttachCurrentThreadJniEnvStarStarType = JNIEnv**;

class MockNfcStatsUtil : public NfcStatsUtil {
 public:
  MOCK_METHOD(void, writeNfcStatsTagTypeOccurred, (int));
};

class MockableJavaVM : public JavaVM {
 public:
  MockableJavaVM() {
    functions = &functions_;
    functions_.DestroyJavaVM = &CallDestroyJavaVM;
    functions_.AttachCurrentThread = &CallAttachCurrentThread;
    functions_.DetachCurrentThread = &CallDetachCurrentThread;
    functions_.GetEnv = &CallGetEnv;
    functions_.AttachCurrentThreadAsDaemon = &CallAttachCurrentThreadAsDaemon;
  }
  virtual ~MockableJavaVM() {}

  virtual jint DestroyJavaVM() = 0;
  virtual jint AttachCurrentThread(AttachCurrentThreadJniEnvStarStarType penv,
                                   void* args) = 0;
  virtual jint DetachCurrentThread() = 0;
  virtual jint GetEnv(void** penv, jint version) = 0;
  virtual jint AttachCurrentThreadAsDaemon(
      AttachCurrentThreadJniEnvStarStarType penv, void* args) = 0;

 private:
  static jint JNICALL CallDestroyJavaVM(JavaVM* vm) {
    return static_cast<MockableJavaVM*>(vm)->DestroyJavaVM();
  }
  static jint JNICALL CallAttachCurrentThread(
      JavaVM* vm, AttachCurrentThreadJniEnvStarStarType penv, void* args) {
    return static_cast<MockableJavaVM*>(vm)->AttachCurrentThread(penv, args);
  }
  static jint JNICALL CallDetachCurrentThread(JavaVM* vm) {
    return static_cast<MockableJavaVM*>(vm)->DetachCurrentThread();
  }
  static jint JNICALL CallGetEnv(JavaVM* vm, void** penv, jint version) {
    return static_cast<MockableJavaVM*>(vm)->GetEnv(penv, version);
  }
  static jint JNICALL CallAttachCurrentThreadAsDaemon(
      JavaVM* vm, AttachCurrentThreadJniEnvStarStarType penv, void* args) {
    return static_cast<MockableJavaVM*>(vm)->AttachCurrentThreadAsDaemon(penv,
                                                                         args);
  }

  JNIInvokeInterface functions_;
};

class MockJavaVM : public MockableJavaVM {
 public:
  MOCK_METHOD(jint, DestroyJavaVM, (), (override));
  MOCK_METHOD(jint, AttachCurrentThread,
              (AttachCurrentThreadJniEnvStarStarType penv, void* args),
              (override));
  MOCK_METHOD(jint, DetachCurrentThread, (), (override));
  MOCK_METHOD(jint, GetEnv, (void** penv, jint version), (override));
  MOCK_METHOD(jint, AttachCurrentThreadAsDaemon,
              (AttachCurrentThreadJniEnvStarStarType penv, void* args),
              (override));
};

class NfcTagTest : public ::testing::Test {
 protected:
  NfcTag mNfcTag;

 public:
  void setNfcStatsUtil(NfcStatsUtil* nfcStatsUtil) {
    mNfcTag.mNfcStatsUtil = nfcStatsUtil;
  }
  void setNfcNativeData(nfc_jni_native_data* data) {
    mNfcTag.mNativeData = data;
  }
  void setNfcJNIEnv(JNIEnv* jniEnv) { mNfcTag.mJniEnv = jniEnv; }
};

TEST_F(NfcTagTest, NfcTagTypeOccurredType5) {
  MockNfcStatsUtil* mockUtil = new MockNfcStatsUtil();

  EXPECT_CALL(*mockUtil,
              writeNfcStatsTagTypeOccurred(
                  nfc::stats::NFC_TAG_TYPE_OCCURRED__TYPE__TAG_TYPE_5))
      .Times(1);

  setNfcStatsUtil(mockUtil);
  auto* mock_native_data =
      new nfc_jni_native_data(0, 0, new MockJavaVM(), 0, new _jobject(),
                              new _jobject(), 0, 0, 0, new _jobject(), 0,
                              {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                               0},  // Initializer list for the first int[16]
                              {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0});

  setNfcNativeData(mock_native_data);
  MockJNIEnv* mockJniEnv = new MockJNIEnv();
  setNfcJNIEnv(mockJniEnv);

  EXPECT_CALL(*mockJniEnv, NewIntArray(testing::_))
      .WillOnce(testing::Return((jintArray)(new jint[10])))
      .WillOnce(testing::Return((jintArray)(new jint[10])))
      .WillOnce(testing::Return((jintArray)(new jint[10])));
  EXPECT_CALL(*mockJniEnv, GetIntArrayElements(testing::_, testing::_))
      .WillOnce(testing::Return(new jint[10]))
      .WillOnce(testing::Return(new jint[10]))
      .WillOnce(testing::Return(new jint[10]));

  tNFA_ACTIVATED mockActivated;
  mockActivated.activate_ntf.rf_disc_id = 1;
  mockActivated.activate_ntf.protocol = NFC_PROTOCOL_T5T;
  mockActivated.activate_ntf.rf_tech_param.mode = NCI_DISCOVERY_TYPE_POLL_V;
  mockActivated.activate_ntf.intf_param.type = NCI_INTERFACE_FRAME;

  tNFA_CONN_EVT_DATA mockData;
  mockData.activated = mockActivated;

  mNfcTag.connectionEventHandler(NFA_ACTIVATED_EVT, &mockData);

  delete mockUtil;
  delete mockJniEnv;
}
