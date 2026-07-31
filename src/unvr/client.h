#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QSslConfiguration>
#include <QSslSocket>
#include "types.h"
#include "app/config.h"

class UNVRClient : public QObject {
    Q_OBJECT

public:
    explicit UNVRClient(const QString &host,
                        const QString &apiKey,
                        TLSMode tlsMode = TLSMode::Verify,
                        const QString &tlsFingerprint = QString(),
                        QObject *parent = nullptr);

    void loadCameras();
    void createStream(const QString &cameraId, const QString &quality = "high");
    void getSnapshot(const QString &cameraId);

signals:
    void camerasLoaded(const QList<CameraInfo> &cameras);
    void streamCreated(const QString &cameraId, const StreamResponse &response);
    void snapshotReceived(const QByteArray &data);
    void error(const QString &message);

private slots:
    void onCamerasReplyFinished();
    void onStreamReplyFinished();
    void onSnapshotReplyFinished();
    void onSslErrors(QNetworkReply *reply, const QList<QSslError> &errors);

private:
    QString buildUrl(const QString &path) const;
    QNetworkRequest makeRequest(const QString &path) const;
    void configureSsl();

    QString host_;
    QString apiKey_;
    TLSMode tlsMode_;
    QString tlsFingerprint_;
    QNetworkAccessManager *nam_;
};

#endif
