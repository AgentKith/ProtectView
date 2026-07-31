#include "types.h"
#include <QJsonArray>

CameraInfo CameraInfo::fromJson(const QJsonObject &obj) {
    CameraInfo info;
    info.id = obj.value("id").toString();
    info.name = obj.value("name").toString();
    info.type = obj.value("type").toString();
    info.width = obj.value("width").isString() ? obj.value("width").toString() : QString::number(obj.value("width").toInt());
    info.height = obj.value("height").isString() ? obj.value("height").toString() : QString::number(obj.value("height").toInt());
    info.codec = obj.value("codec").toString();
    info.enabled = obj.value("enabled").toBool(true);
    return info;
}

QJsonObject CameraInfo::toJson() const {
    QJsonObject obj;
    obj["id"] = id;
    obj["name"] = name;
    obj["type"] = type;
    obj["width"] = width;
    obj["height"] = height;
    obj["codec"] = codec;
    obj["enabled"] = enabled;
    return obj;
}

StreamResponse StreamResponse::fromJson(const QJsonObject &obj) {
    StreamResponse resp;
    resp.high = obj.value("high").toString();
    resp.medium = obj.value("medium").toString();
    resp.low = obj.value("low").toString();
    resp.package = obj.value("package").toString();
    return resp;
}
