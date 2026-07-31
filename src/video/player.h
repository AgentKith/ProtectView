#pragma once

#include <QObject>
#include <QImage>

class VideoPlayer : public QObject {
    Q_OBJECT
public:
    explicit VideoPlayer(QObject *parent = nullptr) : QObject(parent) {}
    virtual ~VideoPlayer() = default;
    virtual void start() = 0;
    virtual void stop() = 0;

signals:
    void frameReady(const QImage &frame);
    void errorOccurred(const QString &message);
};
