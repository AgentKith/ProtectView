#include <QtTest>
#include <QTemporaryDir>
#include "../src/app/storage.h"

class TestStorage : public QObject {
    Q_OBJECT

private slots:
    void testEncryptDecryptRoundtrip();
    void testSaveLoad();
    void testSaveLoadEncrypted();
    void testSaltGeneration();
    void testDecryptEmptyReturnsEmpty();
    void testDecryptTooShortReturnsEmpty();
    void testLoadNonExistentReturnsEmpty();
    void testDifferentFingerprintsDifferentKeys();
};

void TestStorage::testEncryptDecryptRoundtrip() {
    QTemporaryDir tempDir;
    Storage storage(tempDir.path(), "test-fingerprint");
    QByteArray plaintext = "Hello, World! This is a test message.";
    QByteArray encrypted = storage.encrypt(plaintext);
    QVERIFY(!encrypted.isEmpty());
    QVERIFY(encrypted != plaintext);

    QByteArray decrypted = storage.decrypt(encrypted);
    QCOMPARE(decrypted, plaintext);
}

void TestStorage::testSaveLoad() {
    QTemporaryDir tempDir;
    Storage storage(tempDir.path(), "test-fingerprint");
    QByteArray data = "Test data to save";
    QVERIFY(storage.save("test.txt", data));

    QByteArray loaded = storage.load("test.txt");
    QCOMPARE(loaded, data);
}

void TestStorage::testSaveLoadEncrypted() {
    QTemporaryDir tempDir;
    Storage storage(tempDir.path(), "test-fingerprint");
    QByteArray plaintext = "Secret config data";
    QByteArray encrypted = storage.encrypt(plaintext);
    QVERIFY(storage.save("config.enc", encrypted));

    QByteArray loaded = storage.load("config.enc");
    QByteArray decrypted = storage.decrypt(loaded);
    QCOMPARE(decrypted, plaintext);
}

void TestStorage::testSaltGeneration() {
    QTemporaryDir tempDir;
    Storage storage(tempDir.path(), "test-fingerprint");
    QVERIFY(storage.hasSalt());
    QCOMPARE(storage.getSalt().size(), 16);
}

void TestStorage::testDecryptEmptyReturnsEmpty() {
    QTemporaryDir tempDir;
    Storage storage(tempDir.path(), "test-fingerprint");
    QByteArray result = storage.decrypt({});
    QVERIFY(result.isEmpty());
}

void TestStorage::testDecryptTooShortReturnsEmpty() {
    QTemporaryDir tempDir;
    Storage storage(tempDir.path(), "test-fingerprint");
    QByteArray shortData(10, 0);
    QByteArray result = storage.decrypt(shortData);
    QVERIFY(result.isEmpty());
}

void TestStorage::testLoadNonExistentReturnsEmpty() {
    QTemporaryDir tempDir;
    Storage storage(tempDir.path(), "test-fingerprint");
    QByteArray result = storage.load("nonexistent.txt");
    QVERIFY(result.isEmpty());
}

void TestStorage::testDifferentFingerprintsDifferentKeys() {
    QTemporaryDir tempDir1;
    QTemporaryDir tempDir2;

    Storage storage1(tempDir1.path(), "fingerprint-1");
    Storage storage2(tempDir2.path(), "fingerprint-2");

    QByteArray plaintext = "Same plaintext";
    QByteArray encrypted1 = storage1.encrypt(plaintext);

    QByteArray decrypted1 = storage1.decrypt(encrypted1);
    QByteArray decrypted2Wrong = storage2.decrypt(encrypted1);

    QCOMPARE(decrypted1, plaintext);
    QVERIFY(decrypted2Wrong.isEmpty());
}

QTEST_MAIN(TestStorage)
#include "test_storage.moc"
