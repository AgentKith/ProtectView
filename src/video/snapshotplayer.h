#pragma once

#include "player.h"
#include <QNetworkAccessManager>
#include <QTimer>
#include <QString>

class SnapshotPlayer : public VideoPlayer {
    Q_OBJECT
public:
    explicit SnapshotPlayer(const QString &snapshotUrl, int intervalMs = 200, QObject *parent = nullptr);
    void setApiKey(const QString &apiKey);
    void start() override;
    void stop() override;

private slots:
    void onTimerTimeout();
    void onReplyFinished();

private:
    QString snapshotUrl_;
    QString apiKey_;
    int intervalMs_;
    QNetworkAccessManager *nam_;
    QTimer *timer_;
};
