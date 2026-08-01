#pragma once

#include "player.h"
#include <QProcess>
#include <QTimer>
#include <QString>

class FFmpegPlayer : public VideoPlayer {
    Q_OBJECT
public:
    explicit FFmpegPlayer(const QString &streamUrl, QObject *parent = nullptr);
    ~FFmpegPlayer() override;
    void start() override;
    void stop() override;

private slots:
    void onReadyRead();
    void onError(QProcess::ProcessError error);

private:
    QString streamUrl_;
    QProcess ffmpeg_;
    QImage currentFrame_;
    QByteArray incoming_;
    int width_;
    int height_;
    int frameSize_;
};
