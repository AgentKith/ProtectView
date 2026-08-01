#include <QtTest>
#include <QTemporaryDir>
#include <QFile>
#include <QJsonDocument>
#include <QCoreApplication>
#include "app/appcontroller.h"
#include "app/storage.h"
#include "app/fingerprint.h"

class TestStartup : public QObject {
    Q_OBJECT
private slots:
    void testConstructorDoesNotCrash() {
        AppController controller;
        QCOMPARE(controller.getState(), AppController::State::NoConfig);
    }

    void testConstructorWithKioskModeDoesNotCrash() {
        AppController controller(true);
        QCOMPARE(controller.getState(), AppController::State::NoConfig);
    }

    void testConfigRoundtripDoesNotCrash() {
        QTemporaryDir tempDir;
        QString fingerprint = getDeviceFingerprint();
        Storage storage(tempDir.path(), fingerprint);

        AppConfig config;
        config.unvr.host = "192.168.1.100";
        config.unvr.apiKey = "test-key";
        config.unvr.tlsMode = TLSMode::Skip;
        config.video.ffmpegPath = "/usr/bin/ffmpeg";
        config.video.quality = "high";
        config.pinHash = "ab12cd34";

        QJsonDocument doc(config.toJson());
        QByteArray json = doc.toJson();
        QByteArray encrypted = storage.encrypt(json);
        storage.save("config.enc", encrypted);

        QByteArray loaded = storage.load("config.enc");
        QByteArray decrypted = storage.decrypt(loaded);
        QJsonDocument loadedDoc = QJsonDocument::fromJson(decrypted);
        AppConfig loadedConfig = AppConfig::fromJson(loadedDoc.object());

        QCOMPARE(loadedConfig.unvr.host, "192.168.1.100");
        QCOMPARE(loadedConfig.unvr.apiKey, "test-key");
        QCOMPARE(loadedConfig.unvr.tlsMode, TLSMode::Skip);
        QCOMPARE(loadedConfig.video.ffmpegPath, "/usr/bin/ffmpeg");
        QCOMPARE(loadedConfig.video.quality, "high");
        QCOMPARE(loadedConfig.pinHash, "ab12cd34");
    }

    void testSetAndGetConfigDoesNotCrash() {
        AppController controller;
        AppConfig config;
        config.unvr.host = "10.0.0.1";
        config.unvr.apiKey = "set-get-key";
        controller.setConfig(config);
        QCOMPARE(controller.getConfig().unvr.host, "10.0.0.1");
        QCOMPARE(controller.getConfig().unvr.apiKey, "set-get-key");
    }

    void testMultipleControllersDoNotCrash() {
        AppController c1;
        AppController c2(true);
        AppController c3;

        QCOMPARE(c1.getState(), AppController::State::NoConfig);
        QCOMPARE(c2.getState(), AppController::State::NoConfig);
        QCOMPARE(c3.getState(), AppController::State::NoConfig);
    }

    void testDefaultConfigValues() {
        AppController controller;
        AppConfig config;
        controller.setConfig(config);

        AppConfig retrieved = controller.getConfig();
        QCOMPARE(retrieved.version, 1);
        QCOMPARE(retrieved.unvr.port, 443);
        QCOMPARE(retrieved.unvr.tlsMode, TLSMode::Verify);
        QCOMPARE(retrieved.video.mode, VideoMode::PerCamera);
        QCOMPARE(retrieved.video.quality, "medium");
        QCOMPARE(retrieved.layout.mode, LayoutMode::Auto);
        QCOMPARE(retrieved.appearance.theme, ThemeMode::System);
        QCOMPARE(retrieved.appearance.fullscreen, true);
        QCOMPARE(retrieved.appearance.kiosk, false);
    }
};

QTEST_MAIN(TestStartup)
#include "test_startup.moc"
