#include "snapshotplayer.h"
#include <QNetworkRequest>
#include <QNetworkReply>

SnapshotPlayer::SnapshotPlayer(const QString &snapshotUrl, int intervalMs, QObject *parent)
    : VideoPlayer(parent),
      snapshotUrl_(snapshotUrl),
      intervalMs_(intervalMs),
      nam_(new QNetworkAccessManager(this)),
      timer_(new QTimer(this)) {
    connect(timer_, &QTimer::timeout, this, &SnapshotPlayer::onTimerTimeout);
}

void SnapshotPlayer::setApiKey(const QString &apiKey) {
    apiKey_ = apiKey;
}

void SnapshotPlayer::start() {
    timer_->start(intervalMs_);
}

void SnapshotPlayer::stop() {
    timer_->stop();
}

void SnapshotPlayer::onTimerTimeout() {
    QNetworkRequest request(snapshotUrl_);
    if (!apiKey_.isEmpty()) {
        request.setRawHeader("X-API-Key", apiKey_.toUtf8());
    }
    QNetworkReply *reply = nam_->get(request);
    connect(reply, &QNetworkReply::finished, this, &SnapshotPlayer::onReplyFinished);
}

void SnapshotPlayer::onReplyFinished() {
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    if (!reply) return;

    if (reply->error() != QNetworkReply::NoError) {
        emit errorOccurred(reply->errorString());
        reply->deleteLater();
        return;
    }

    QByteArray data = reply->readAll();
    reply->deleteLater();

    QImage frame;
    if (frame.loadFromData(data)) {
        emit frameReady(frame);
    } else {
        emit errorOccurred("Failed to decode image");
    }
}
