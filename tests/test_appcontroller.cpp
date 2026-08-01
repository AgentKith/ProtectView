#include <QtTest>
#include <QTemporaryDir>
#include <QFile>
#include <QJsonDocument>
#include <QCryptographicHash>
#include "app/appcontroller.h"
#include "app/storage.h"
#include "app/fingerprint.h"

class TestAppController : public QObject {
    Q_OBJECT
private slots:
    void testInitialState() {
        AppController controller;
        QCOMPARE(controller.getState(), AppController::State::NoConfig);
    }

    void testSaveAndLoadConfig() {
        QTemporaryDir tempDir;
        QString fingerprint = getDeviceFingerprint();
        Storage storage(tempDir.path(), fingerprint);

        AppConfig config;
        config.unvr.host = "192.168.1.100";
        config.unvr.apiKey = "test-api-key";

        QJsonDocument doc(config.toJson());
        QByteArray json = doc.toJson();
        QByteArray encrypted = storage.encrypt(json);
        storage.save("config.enc", encrypted);

        QByteArray loaded = storage.load("config.enc");
        QByteArray decrypted = storage.decrypt(loaded);
        QJsonDocument loadedDoc = QJsonDocument::fromJson(decrypted);
        AppConfig loadedConfig = AppConfig::fromJson(loadedDoc.object());

        QCOMPARE(loadedConfig.unvr.host, "192.168.1.100");
        QCOMPARE(loadedConfig.unvr.apiKey, "test-api-key");
    }

    void testEncryptDecryptRoundtrip() {
        QTemporaryDir tempDir;
        QString fingerprint = getDeviceFingerprint();
        Storage storage(tempDir.path(), fingerprint);

        QByteArray plaintext = "test data for encryption";
        QByteArray encrypted = storage.encrypt(plaintext);
        QByteArray decrypted = storage.decrypt(encrypted);

        QCOMPARE(decrypted, plaintext);
    }

    void testDifferentFingerprintsProduceDifferentKeys() {
        QTemporaryDir tempDir1;
        QTemporaryDir tempDir2;

        Storage storage1(tempDir1.path(), "fingerprint1");
        Storage storage2(tempDir2.path(), "fingerprint2");

        QByteArray plaintext = "secret data";
        QByteArray encrypted1 = storage1.encrypt(plaintext);
        QByteArray decrypted2 = storage2.decrypt(encrypted1);

        QVERIFY(decrypted2 != plaintext);
    }

    void testPinHashSavedAndLoaded() {
        QTemporaryDir tempDir;
        QString fingerprint = getDeviceFingerprint();
        Storage storage(tempDir.path(), fingerprint);

        AppConfig config;
        config.unvr.host = "192.168.1.100";
        config.unvr.apiKey = "test-api-key";
        config.pinHash = "abc123def456";

        QJsonDocument doc(config.toJson());
        QByteArray json = doc.toJson();
        QByteArray encrypted = storage.encrypt(json);
        storage.save("config.enc", encrypted);

        QByteArray loaded = storage.load("config.enc");
        QByteArray decrypted = storage.decrypt(loaded);
        QJsonDocument loadedDoc = QJsonDocument::fromJson(decrypted);
        AppConfig loadedConfig = AppConfig::fromJson(loadedDoc.object());

        QCOMPARE(loadedConfig.pinHash, "abc123def456");
    }

    void testPinHashRoundtripThroughStorage() {
        QTemporaryDir tempDir;
        QString fingerprint = getDeviceFingerprint();
        Storage storage(tempDir.path(), fingerprint);

        QByteArray salt;
        salt.reserve(16);
        for (int i = 0; i < 16; ++i) {
            salt.append(static_cast<char>(i + 1));
        }
        QString testPin = "123456";
        QByteArray preImage = salt + testPin.toUtf8();
        QByteArray hash = QCryptographicHash::hash(preImage, QCryptographicHash::Sha256);
        QString pinHash = QString(salt.toHex()) + QString(hash.toHex());

        AppConfig config;
        config.unvr.host = "192.168.1.100";
        config.pinHash = pinHash;

        QJsonDocument doc(config.toJson());
        QByteArray json = doc.toJson();
        QByteArray encrypted = storage.encrypt(json);
        storage.save("config.enc", encrypted);

        QByteArray loaded = storage.load("config.enc");
        QByteArray decrypted = storage.decrypt(loaded);
        QJsonDocument loadedDoc = QJsonDocument::fromJson(decrypted);
        AppConfig loadedConfig = AppConfig::fromJson(loadedDoc.object());

        QCOMPARE(loadedConfig.pinHash, pinHash);

        QByteArray loadedSalt = QByteArray::fromHex(loadedConfig.pinHash.left(32).toUtf8());
        QString loadedStoredHash = loadedConfig.pinHash.mid(32);
        QByteArray recomputedHash = QCryptographicHash::hash(loadedSalt + testPin.toUtf8(), QCryptographicHash::Sha256);
        QCOMPARE(recomputedHash.toHex(), loadedStoredHash.toUtf8());
    }
};

QTEST_MAIN(TestAppController)
#include "test_appcontroller.moc"
