#include "ffmpegplayer.h"

FFmpegPlayer::FFmpegPlayer(const QString &streamUrl, QObject *parent)
    : VideoPlayer(parent),
      streamUrl_(streamUrl),
      width_(640),
      height_(480) {
    frameSize_ = width_ * height_ * 4;

    connect(&ffmpeg_, &QProcess::readyRead, this, &FFmpegPlayer::onReadyRead);
    connect(&ffmpeg_, QOverload<QProcess::ProcessError>::of(&QProcess::errorOccurred),
            this, &FFmpegPlayer::onError);
}

void FFmpegPlayer::start() {
    QStringList args {
        "-re", "-i", streamUrl_,
        "-vcodec", "rawvideo",
        "-pix_fmt", "rgba",
        "-s", QString("%1x%2").arg(width_).arg(height_),
        "-f", "rawvideo", "-"
    };

    ffmpeg_.start("ffmpeg", args);
    if (!ffmpeg_.waitForStarted(5000)) {
        emit errorOccurred("Failed to start FFmpeg");
    }
}

void FFmpegPlayer::stop() {
    ffmpeg_.terminate();
    ffmpeg_.waitForFinished(3000);
    if (ffmpeg_.state() != QProcess::NotRunning) {
        ffmpeg_.kill();
    }
}

void FFmpegPlayer::onReadyRead() {
    incoming_.append(ffmpeg_.readAll());
    while (incoming_.size() >= frameSize_) {
        QByteArray frameData = incoming_.left(frameSize_);
        incoming_.remove(0, frameSize_);

        const uchar *bits = reinterpret_cast<const uchar *>(frameData.constData());
        QImage frame = QImage(bits, width_, height_, QImage::Format_RGBA8888).copy();
        emit frameReady(frame);
    }
}

void FFmpegPlayer::onError(QProcess::ProcessError error) {
    if (error == QProcess::Crashed) {
        emit errorOccurred("FFmpeg process crashed");
    } else if (error == QProcess::FailedToStart) {
        emit errorOccurred("Failed to start FFmpeg");
    }
}
