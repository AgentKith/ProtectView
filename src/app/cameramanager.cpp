#include "cameramanager.h"
#include "app/config.h"
#include "unvr/client.h"
#include "video/ffmpegplayer.h"
#include "video/snapshotplayer.h"

CameraManager::CameraManager(const AppConfig &config, UNVRClient *client, QObject *parent)
    : QObject(parent),
      config_(config),
      client_(client) {
    connect(client_, &UNVRClient::streamCreated, this, &CameraManager::onStreamCreated);
}

void CameraManager::setCameras(const QList<CameraInfo> &cameras) {
    stopAll();
    cameras_ = cameras;
    players_.resize(cameras_.size(), nullptr);
}

int CameraManager::cameraCount() const {
    return cameras_.size();
}

QString CameraManager::cameraName(int index) const {
    if (index >= 0 && index < cameras_.size()) {
        return cameras_[index].name;
    }
    return QString();
}

void CameraManager::startCamera(int index) {
    if (index < 0 || index >= cameras_.size()) return;
    if (!cameras_[index].enabled) return;

    stopCamera(index);

    if (config_.video.mode == VideoMode::Snapshot) {
        QString snapshotUrl = QString("https://%s/proxy/protect/integration/v1/cameras/%s/snapshot")
                                    .arg(config_.unvr.host)
                                    .arg(cameras_[index].id);
        SnapshotPlayer *player = new SnapshotPlayer(snapshotUrl, 200, this);
        player->setApiKey(config_.unvr.apiKey);
        players_[index] = player;
        connect(player, &VideoPlayer::frameReady, this, [this, index](const QImage &frame) {
            emit frameReady(index, frame);
        });
        connect(player, &VideoPlayer::errorOccurred, this, [this, index](const QString &message) {
            emit errorOccurred(index, message);
        });
        player->start();
    } else {
        startCameraWithStream(cameras_[index].id, index);
    }
}

void CameraManager::stopCamera(int index) {
    if (index < 0 || index >= players_.size()) return;
    if (players_[index]) {
        players_[index]->stop();
        players_[index]->deleteLater();
        players_[index] = nullptr;
    }
}

void CameraManager::startAll() {
    for (int i = 0; i < cameras_.size(); ++i) {
        startCamera(i);
    }
}

void CameraManager::stopAll() {
    for (int i = 0; i < players_.size(); ++i) {
        stopCamera(i);
    }
    pendingStream_.clear();
}

void CameraManager::startCameraWithStream(const QString &cameraId, int index) {
    pendingStream_[cameraId] = index;
    client_->createStream(cameraId, config_.video.quality);
}

void CameraManager::onStreamCreated(const QString &cameraId, const StreamResponse &response) {
    if (!pendingStream_.contains(cameraId)) {
        return;
    }
    int index = pendingStream_.take(cameraId);

    QString streamUrl;
    if (config_.video.quality == "low" && !response.low.isEmpty()) {
        streamUrl = response.low;
    } else if (config_.video.quality == "high" && !response.high.isEmpty()) {
        streamUrl = response.high;
    } else if (!response.medium.isEmpty()) {
        streamUrl = response.medium;
    } else if (!response.high.isEmpty()) {
        streamUrl = response.high;
    }

    if (streamUrl.isEmpty()) {
        emit errorOccurred(index, "No stream URL returned");
        return;
    }

    FFmpegPlayer *player = new FFmpegPlayer(streamUrl, this);
    players_[index] = player;
    connect(player, &VideoPlayer::frameReady, this, [this, index](const QImage &frame) {
        emit frameReady(index, frame);
    });
    connect(player, &VideoPlayer::errorOccurred, this, [this, index](const QString &message) {
        emit errorOccurred(index, message);
    });
    player->start();
}
