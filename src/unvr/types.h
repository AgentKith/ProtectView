#ifndef TYPES_H
#define TYPES_H

#include <QString>
#include <QList>
#include <QJsonObject>

struct CameraInfo {
    QString id;
    QString name;
    QString type;
    QString width;
    QString height;
    QString codec;
    bool enabled = true;

    static CameraInfo fromJson(const QJsonObject &obj);
    QJsonObject toJson() const;
};

struct StreamResponse {
    QString high;
    QString medium;
    QString low;
    QString package;

    static StreamResponse fromJson(const QJsonObject &obj);
};

#endif
