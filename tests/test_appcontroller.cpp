#include <QtTest>
#include <QTemporaryDir>
#include <QFile>
#include <QJsonDocument>
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
};

QTEST_MAIN(TestAppController)
#include "test_appcontroller.moc"
