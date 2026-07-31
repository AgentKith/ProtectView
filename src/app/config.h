#pragma once

#include <QString>
#include <QStringList>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

enum class VideoMode {
    PerCamera,
    Composite,
    Snapshot
};

enum class TLSMode {
    Verify,
    Skip,
    Fingerprint
};

enum class LayoutMode {
    Auto,
    Custom
};

enum class ThemeMode {
    System,
    Light,
    Dark
};

struct UNVRConfig {
    QString host;
    int port = 443;
    TLSMode tlsMode = TLSMode::Verify;
    QString tlsFingerprint;
    QString apiKey;
};

struct VideoConfig {
    VideoMode mode = VideoMode::PerCamera;
    QString ffmpegPath;
    QString quality = "medium";
};

struct LayoutConfig {
    LayoutMode mode = LayoutMode::Auto;
    int rows = 0;
    int cols = 0;
    QStringList cameras;
};

struct AppearanceConfig {
    ThemeMode theme = ThemeMode::System;
    bool fullscreen = true;
    bool kiosk = false;
};

struct AppConfig {
    int version = 1;
    UNVRConfig unvr;
    QString pinHash;
    VideoConfig video;
    LayoutConfig layout;
    AppearanceConfig appearance;

    QJsonObject toJson() const;
    static AppConfig fromJson(const QJsonObject &obj);
    QJsonDocument toDocument() const;
    static AppConfig fromDocument(const QJsonDocument &doc);
};

QString videoModeToString(VideoMode mode);
VideoMode stringToVideoMode(const QString &str);

QString tlsModeToString(TLSMode mode);
TLSMode stringToTLSMode(const QString &str);

QString layoutModeToString(LayoutMode mode);
LayoutMode stringToLayoutMode(const QString &str);

QString themeModeToString(ThemeMode mode);
ThemeMode stringToThemeMode(const QString &str);
