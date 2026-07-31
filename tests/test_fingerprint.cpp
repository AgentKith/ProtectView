#include <QtTest>
#include "../src/app/fingerprint.h"

class TestFingerprint : public QObject {
    Q_OBJECT

private slots:
    void testFingerprintNotEmpty();
    void testFingerprintConsistent();
};

void TestFingerprint::testFingerprintNotEmpty() {
    QString fp = getDeviceFingerprint();
    QVERIFY(!fp.isEmpty());
}

void TestFingerprint::testFingerprintConsistent() {
    QString fp1 = getDeviceFingerprint();
    QString fp2 = getDeviceFingerprint();
    QCOMPARE(fp1, fp2);
}

QTEST_MAIN(TestFingerprint)
#include "test_fingerprint.moc"
