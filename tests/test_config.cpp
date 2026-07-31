#include <QtTest>
#include "../src/app/config.h"

class TestConfig : public QObject {
    Q_OBJECT

private slots:
    void testDefaultValues();
    void testToJsonFromJsonRoundtrip();
    void testToDocumentFromDocumentRoundtrip();
    void testVideoModeConversion();
    void testTLSModeConversion();
    void testLayoutModeConversion();
    void testThemeModeConversion();
    void testCamerasList();
    void testPartialJson();
};

void TestConfig::testDefaultValues() {
    AppConfig config;
    QCOMPARE(config.version, 1);
    QVERIFY(config.unvr.host.isEmpty());
    QCOMPARE(config.unvr.port, 443);
    QCOMPARE(config.unvr.tlsMode, TLSMode::Verify);
    QVERIFY(config.unvr.tlsFingerprint.isEmpty());
    QVERIFY(config.unvr.apiKey.isEmpty());
    QVERIFY(config.pinHash.isEmpty());
    QCOMPARE(config.video.mode, VideoMode::PerCamera);
    QVERIFY(config.video.ffmpegPath.isEmpty());
    QCOMPARE(config.video.quality, "medium");
    QCOMPARE(config.layout.mode, LayoutMode::Auto);
    QCOMPARE(config.layout.rows, 0);
    QCOMPARE(config.layout.cols, 0);
    QVERIFY(config.layout.cameras.isEmpty());
    QCOMPARE(config.appearance.theme, ThemeMode::System);
    QVERIFY(config.appearance.fullscreen);
    QVERIFY(!config.appearance.kiosk);
}

void TestConfig::testToJsonFromJsonRoundtrip() {
    AppConfig original;
    original.version = 2;
    original.unvr.host = "192.168.1.100";
    original.unvr.port = 8443;
    original.unvr.tlsMode = TLSMode::Fingerprint;
    original.unvr.tlsFingerprint = "ab:cd:ef";
    original.unvr.apiKey = "test-api-key";
    original.pinHash = "hashed-pin";
    original.video.mode = VideoMode::Composite;
    original.video.ffmpegPath = "/usr/bin/ffmpeg";
    original.video.quality = "high";
    original.layout.mode = LayoutMode::Custom;
    original.layout.rows = 2;
    original.layout.cols = 3;
    original.layout.cameras = {"cam1", "cam2", "cam3"};
    original.appearance.theme = ThemeMode::Dark;
    original.appearance.fullscreen = false;
    original.appearance.kiosk = true;

    QJsonObject json = original.toJson();
    AppConfig restored = AppConfig::fromJson(json);

    QCOMPARE(restored.version, original.version);
    QCOMPARE(restored.unvr.host, original.unvr.host);
    QCOMPARE(restored.unvr.port, original.unvr.port);
    QCOMPARE(restored.unvr.tlsMode, original.unvr.tlsMode);
    QCOMPARE(restored.unvr.tlsFingerprint, original.unvr.tlsFingerprint);
    QCOMPARE(restored.unvr.apiKey, original.unvr.apiKey);
    QCOMPARE(restored.pinHash, original.pinHash);
    QCOMPARE(restored.video.mode, original.video.mode);
    QCOMPARE(restored.video.ffmpegPath, original.video.ffmpegPath);
    QCOMPARE(restored.video.quality, original.video.quality);
    QCOMPARE(restored.layout.mode, original.layout.mode);
    QCOMPARE(restored.layout.rows, original.layout.rows);
    QCOMPARE(restored.layout.cols, original.layout.cols);
    QCOMPARE(restored.layout.cameras, original.layout.cameras);
    QCOMPARE(restored.appearance.theme, original.appearance.theme);
    QCOMPARE(restored.appearance.fullscreen, original.appearance.fullscreen);
    QCOMPARE(restored.appearance.kiosk, original.appearance.kiosk);
}

void TestConfig::testToDocumentFromDocumentRoundtrip() {
    AppConfig original;
    original.unvr.host = "10.0.0.1";
    original.unvr.apiKey = "secret";

    QJsonDocument doc = original.toDocument();
    AppConfig restored = AppConfig::fromDocument(doc);

    QCOMPARE(restored.unvr.host, original.unvr.host);
    QCOMPARE(restored.unvr.apiKey, original.unvr.apiKey);
}

void TestConfig::testVideoModeConversion() {
    QCOMPARE(videoModeToString(VideoMode::PerCamera), "per-camera");
    QCOMPARE(videoModeToString(VideoMode::Composite), "composite");
    QCOMPARE(videoModeToString(VideoMode::Snapshot), "snapshot");

    QCOMPARE(stringToVideoMode("per-camera"), VideoMode::PerCamera);
    QCOMPARE(stringToVideoMode("composite"), VideoMode::Composite);
    QCOMPARE(stringToVideoMode("snapshot"), VideoMode::Snapshot);
    QCOMPARE(stringToVideoMode("unknown"), VideoMode::PerCamera);
}

void TestConfig::testTLSModeConversion() {
    QCOMPARE(tlsModeToString(TLSMode::Verify), "verify");
    QCOMPARE(tlsModeToString(TLSMode::Skip), "skip");
    QCOMPARE(tlsModeToString(TLSMode::Fingerprint), "fingerprint");

    QCOMPARE(stringToTLSMode("verify"), TLSMode::Verify);
    QCOMPARE(stringToTLSMode("skip"), TLSMode::Skip);
    QCOMPARE(stringToTLSMode("fingerprint"), TLSMode::Fingerprint);
    QCOMPARE(stringToTLSMode("unknown"), TLSMode::Verify);
}

void TestConfig::testLayoutModeConversion() {
    QCOMPARE(layoutModeToString(LayoutMode::Auto), "auto");
    QCOMPARE(layoutModeToString(LayoutMode::Custom), "custom");

    QCOMPARE(stringToLayoutMode("auto"), LayoutMode::Auto);
    QCOMPARE(stringToLayoutMode("custom"), LayoutMode::Custom);
    QCOMPARE(stringToLayoutMode("unknown"), LayoutMode::Auto);
}

void TestConfig::testThemeModeConversion() {
    QCOMPARE(themeModeToString(ThemeMode::System), "system");
    QCOMPARE(themeModeToString(ThemeMode::Light), "light");
    QCOMPARE(themeModeToString(ThemeMode::Dark), "dark");

    QCOMPARE(stringToThemeMode("system"), ThemeMode::System);
    QCOMPARE(stringToThemeMode("light"), ThemeMode::Light);
    QCOMPARE(stringToThemeMode("dark"), ThemeMode::Dark);
    QCOMPARE(stringToThemeMode("unknown"), ThemeMode::System);
}

void TestConfig::testCamerasList() {
    AppConfig config;
    config.layout.cameras = {"front", "back", "side"};

    QJsonObject json = config.toJson();
    QJsonArray cameras = json["layout"].toObject()["cameras"].toArray();
    QCOMPARE(cameras.size(), 3);
    QCOMPARE(cameras[0].toString(), "front");
    QCOMPARE(cameras[1].toString(), "back");
    QCOMPARE(cameras[2].toString(), "side");

    AppConfig restored = AppConfig::fromJson(json);
    QCOMPARE(restored.layout.cameras.size(), 3);
    QCOMPARE(restored.layout.cameras[0], "front");
}

void TestConfig::testPartialJson() {
    QJsonObject json;
    json["version"] = 1;

    AppConfig config = AppConfig::fromJson(json);
    QCOMPARE(config.version, 1);
    QVERIFY(config.unvr.host.isEmpty());
    QCOMPARE(config.unvr.port, 443);
    QCOMPARE(config.unvr.tlsMode, TLSMode::Verify);
    QCOMPARE(config.video.mode, VideoMode::PerCamera);
    QCOMPARE(config.video.quality, "medium");
    QCOMPARE(config.layout.mode, LayoutMode::Auto);
    QCOMPARE(config.appearance.theme, ThemeMode::System);
    QVERIFY(config.appearance.fullscreen);
    QVERIFY(!config.appearance.kiosk);
}

QTEST_MAIN(TestConfig)
#include "test_config.moc"
