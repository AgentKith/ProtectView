#include "client.h"
#include <QNetworkRequest>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QUrl>
#include <QSslCertificate>
#include <QSslError>
#include <QCryptographicHash>

UNVRClient::UNVRClient(const QString &host,
                       const QString &apiKey,
                       TLSMode tlsMode,
                       const QString &tlsFingerprint,
                       QObject *parent)
    : QObject(parent),
      host_(host),
      apiKey_(apiKey),
      tlsMode_(tlsMode),
      tlsFingerprint_(tlsFingerprint),
      nam_(new QNetworkAccessManager(this)) {
    configureSsl();
}

void UNVRClient::configureSsl() {
    QSslConfiguration config = QSslConfiguration::defaultConfiguration();

    switch (tlsMode_) {
        case TLSMode::Verify:
            config.setPeerVerifyMode(QSslSocket::VerifyPeer);
            break;
        case TLSMode::Skip:
            config.setPeerVerifyMode(QSslSocket::VerifyNone);
            break;
        case TLSMode::Fingerprint:
            config.setPeerVerifyMode(QSslSocket::VerifyPeer);
            connect(nam_, &QNetworkAccessManager::sslErrors, this, &UNVRClient::onSslErrors);
            break;
    }

    QSslConfiguration::setDefaultConfiguration(config);
}

QString UNVRClient::buildUrl(const QString &path) const {
    return "https://" + host_ + "/proxy/protect/integration" + path;
}

QNetworkRequest UNVRClient::makeRequest(const QString &path) const {
    QUrl url = buildUrl(path);
    QNetworkRequest request(url);
    request.setRawHeader("X-API-Key", apiKey_.toUtf8());
    request.setRawHeader("Accept", "application/json");
    return request;
}

void UNVRClient::onSslErrors(QNetworkReply *reply, const QList<QSslError> &errors) {
    if (tlsMode_ != TLSMode::Fingerprint) {
        return;
    }

    QString expected = tlsFingerprint_.remove(':').toLower();
    bool ok = true;
    for (const QSslError &err : errors) {
        QSslCertificate cert = err.certificate();
        QByteArray digest = cert.digest(QCryptographicHash::Sha256).toHex();
        if (QString(digest).toLower() == expected) {
            reply->ignoreSslErrors();
            ok = false;
            break;
        }
    }
    if (ok) {
        reply->abort();
        emit error("SSL certificate fingerprint mismatch");
    }
}

void UNVRClient::loadCameras() {
    QNetworkRequest request = makeRequest("/v1/cameras");
    QNetworkReply *reply = nam_->get(request);
    connect(reply, &QNetworkReply::finished, this, &UNVRClient::onCamerasReplyFinished);
}

void UNVRClient::onCamerasReplyFinished() {
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    if (!reply) return;

    if (reply->error() != QNetworkReply::NoError) {
        emit error(reply->errorString());
        reply->deleteLater();
        return;
    }

    QByteArray data = reply->readAll();
    reply->deleteLater();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    QJsonArray cameraArray;

    if (doc.isArray()) {
        cameraArray = doc.array();
    } else if (doc.isObject()) {
        QJsonObject obj = doc.object();
        if (obj.contains("cameras") && obj["cameras"].isArray()) {
            cameraArray = obj["cameras"].toArray();
        } else {
            emit error("Invalid cameras response");
            return;
        }
    } else {
        emit error("Invalid cameras response");
        return;
    }

    QList<CameraInfo> cameras;
    for (const QJsonValue &val : cameraArray) {
        cameras.append(CameraInfo::fromJson(val.toObject()));
    }
    emit camerasLoaded(cameras);
}

void UNVRClient::createStream(const QString &cameraId, const QString &quality) {
    QNetworkRequest request = makeRequest("/v1/cameras/" + cameraId + "/rtsps-stream");
    request.setRawHeader("Content-Type", "application/json");

    QJsonArray qualities;
    qualities.append(quality);
    QJsonObject payload;
    payload["qualities"] = qualities;

    QNetworkReply *reply = nam_->post(request, QJsonDocument(payload).toJson());
    reply->setProperty("cameraId", cameraId);
    connect(reply, &QNetworkReply::finished, this, &UNVRClient::onStreamReplyFinished);
}

void UNVRClient::onStreamReplyFinished() {
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    if (!reply) return;

    QString cameraId = reply->property("cameraId").toString();

    if (reply->error() != QNetworkReply::NoError) {
        emit error(reply->errorString());
        reply->deleteLater();
        return;
    }

    QByteArray data = reply->readAll();
    reply->deleteLater();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (doc.isObject()) {
        StreamResponse resp = StreamResponse::fromJson(doc.object());
        emit streamCreated(cameraId, resp);
    } else {
        emit error("Invalid stream response");
    }
}

void UNVRClient::getSnapshot(const QString &cameraId) {
    QNetworkRequest request = makeRequest("/v1/cameras/" + cameraId + "/snapshot");
    QNetworkReply *reply = nam_->get(request);
    connect(reply, &QNetworkReply::finished, this, &UNVRClient::onSnapshotReplyFinished);
}

void UNVRClient::onSnapshotReplyFinished() {
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    if (!reply) return;

    if (reply->error() != QNetworkReply::NoError) {
        emit error(reply->errorString());
        reply->deleteLater();
        return;
    }

    QByteArray data = reply->readAll();
    reply->deleteLater();
    emit snapshotReceived(data);
}
