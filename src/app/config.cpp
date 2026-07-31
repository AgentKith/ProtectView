#include "config.h"

QString videoModeToString(VideoMode mode) {
    switch (mode) {
        case VideoMode::PerCamera: return "per-camera";
        case VideoMode::Composite: return "composite";
        case VideoMode::Snapshot: return "snapshot";
    }
    return "per-camera";
}

VideoMode stringToVideoMode(const QString &str) {
    if (str == "composite") return VideoMode::Composite;
    if (str == "snapshot") return VideoMode::Snapshot;
    return VideoMode::PerCamera;
}

QString tlsModeToString(TLSMode mode) {
    switch (mode) {
        case TLSMode::Verify: return "verify";
        case TLSMode::Skip: return "skip";
        case TLSMode::Fingerprint: return "fingerprint";
    }
    return "verify";
}

TLSMode stringToTLSMode(const QString &str) {
    if (str == "skip") return TLSMode::Skip;
    if (str == "fingerprint") return TLSMode::Fingerprint;
    return TLSMode::Verify;
}

QString layoutModeToString(LayoutMode mode) {
    switch (mode) {
        case LayoutMode::Auto: return "auto";
        case LayoutMode::Custom: return "custom";
    }
    return "auto";
}

LayoutMode stringToLayoutMode(const QString &str) {
    if (str == "custom") return LayoutMode::Custom;
    return LayoutMode::Auto;
}

QString themeModeToString(ThemeMode mode) {
    switch (mode) {
        case ThemeMode::System: return "system";
        case ThemeMode::Light: return "light";
        case ThemeMode::Dark: return "dark";
    }
    return "system";
}

ThemeMode stringToThemeMode(const QString &str) {
    if (str == "light") return ThemeMode::Light;
    if (str == "dark") return ThemeMode::Dark;
    return ThemeMode::System;
}

QJsonObject AppConfig::toJson() const {
    QJsonObject obj;
    obj["version"] = version;

    QJsonObject unvrObj;
    unvrObj["host"] = unvr.host;
    unvrObj["port"] = unvr.port;
    unvrObj["tls_mode"] = tlsModeToString(unvr.tlsMode);
    unvrObj["tls_fingerprint"] = unvr.tlsFingerprint;
    unvrObj["api_key"] = unvr.apiKey;
    obj["unvr"] = unvrObj;

    obj["pin_hash"] = pinHash;

    QJsonObject videoObj;
    videoObj["mode"] = videoModeToString(video.mode);
    videoObj["ffmpeg_path"] = video.ffmpegPath;
    videoObj["quality"] = video.quality;
    obj["video"] = videoObj;

    QJsonObject layoutObj;
    layoutObj["mode"] = layoutModeToString(layout.mode);
    layoutObj["rows"] = layout.rows;
    layoutObj["cols"] = layout.cols;
    QJsonArray camerasArr;
    for (const QString &cam : layout.cameras) {
        camerasArr.append(cam);
    }
    layoutObj["cameras"] = camerasArr;
    obj["layout"] = layoutObj;

    QJsonObject appearanceObj;
    appearanceObj["theme"] = themeModeToString(appearance.theme);
    appearanceObj["fullscreen"] = appearance.fullscreen;
    appearanceObj["kiosk"] = appearance.kiosk;
    obj["appearance"] = appearanceObj;

    return obj;
}

AppConfig AppConfig::fromJson(const QJsonObject &obj) {
    AppConfig config;
    config.version = obj.value("version").toInt(1);

    QJsonObject unvrObj = obj.value("unvr").toObject();
    config.unvr.host = unvrObj.value("host").toString();
    config.unvr.port = unvrObj.value("port").toInt(443);
    config.unvr.tlsMode = stringToTLSMode(unvrObj.value("tls_mode").toString("verify"));
    config.unvr.tlsFingerprint = unvrObj.value("tls_fingerprint").toString();
    config.unvr.apiKey = unvrObj.value("api_key").toString();

    config.pinHash = obj.value("pin_hash").toString();

    QJsonObject videoObj = obj.value("video").toObject();
    config.video.mode = stringToVideoMode(videoObj.value("mode").toString("per-camera"));
    config.video.ffmpegPath = videoObj.value("ffmpeg_path").toString();
    config.video.quality = videoObj.value("quality").toString("medium");

    QJsonObject layoutObj = obj.value("layout").toObject();
    config.layout.mode = stringToLayoutMode(layoutObj.value("mode").toString("auto"));
    config.layout.rows = layoutObj.value("rows").toInt(0);
    config.layout.cols = layoutObj.value("cols").toInt(0);
    QJsonArray camerasArr = layoutObj.value("cameras").toArray();
    for (const QJsonValue &val : camerasArr) {
        config.layout.cameras.append(val.toString());
    }

    QJsonObject appearanceObj = obj.value("appearance").toObject();
    config.appearance.theme = stringToThemeMode(appearanceObj.value("theme").toString("system"));
    config.appearance.fullscreen = appearanceObj.value("fullscreen").toBool(true);
    config.appearance.kiosk = appearanceObj.value("kiosk").toBool(false);

    return config;
}

QJsonDocument AppConfig::toDocument() const {
    return QJsonDocument(toJson());
}

AppConfig AppConfig::fromDocument(const QJsonDocument &doc) {
    return fromJson(doc.object());
}
