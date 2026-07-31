#pragma once

#include <QObject>
#include <QVector>
#include <QMap>
#include <QString>
#include <QImage>
#include "app/config.h"
#include "unvr/types.h"

class VideoPlayer;
class UNVRClient;

class CameraManager : public QObject {
    Q_OBJECT
public:
    explicit CameraManager(const AppConfig &config, UNVRClient *client, QObject *parent = nullptr);
    void setCameras(const QList<CameraInfo> &cameras);
    int cameraCount() const;
    QString cameraName(int index) const;
    void startCamera(int index);
    void stopCamera(int index);
    void startAll();
    void stopAll();

signals:
    void frameReady(int index, const QImage &frame);
    void errorOccurred(int index, const QString &message);

private slots:
    void onStreamCreated(const QString &cameraId, const StreamResponse &response);

private:
    void startCameraWithStream(const QString &cameraId, int index);

    AppConfig config_;
    UNVRClient *client_;
    QList<CameraInfo> cameras_;
    QVector<VideoPlayer *> players_;
    QMap<QString, int> pendingStream_;
};
