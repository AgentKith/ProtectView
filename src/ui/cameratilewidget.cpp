#include "cameratilewidget.h"
#include <QPainter>
#include <QPaintEvent>
#include <QMouseEvent>

CameraTileWidget::CameraTileWidget(const QString &cameraName, QWidget *parent)
    : QWidget(parent),
      cameraName_(cameraName),
      hasError_(false),
      hasFrame_(false) {
    setAutoFillBackground(true);
    retryTimer_.setInterval(5000);
    connect(&retryTimer_, &QTimer::timeout, this, &CameraTileWidget::retryRequested);
}

void CameraTileWidget::setFrame(const QImage &frame) {
    currentFrame_ = frame;
    hasFrame_ = true;
    hasError_ = false;
    errorMessage_.clear();
    retryTimer_.stop();
    update();
}

void CameraTileWidget::setError(const QString &message) {
    hasError_ = true;
    errorMessage_ = message;
    hasFrame_ = false;
    retryTimer_.start();
    update();
}

void CameraTileWidget::clearError() {
    hasError_ = false;
    errorMessage_.clear();
    retryTimer_.stop();
    update();
}

QString CameraTileWidget::cameraName() const {
    return cameraName_;
}

void CameraTileWidget::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    if (hasFrame_ && !currentFrame_.isNull()) {
        painter.drawImage(rect(), currentFrame_);
    } else if (hasError_) {
        painter.fillRect(rect(), QColor(26, 26, 26));

        painter.setPen(QColor(239, 68, 68));
        painter.setFont(QFont("Plus Jakarta Sans", 28, QFont::DemiBold));
        painter.drawText(rect(), Qt::AlignCenter, cameraName_ + "\n" + errorMessage_);
    } else {
        painter.fillRect(rect(), QColor(26, 26, 26));

        painter.setPen(QColor(224, 224, 224));
        painter.setFont(QFont("Plus Jakarta Sans", 28, QFont::Medium));
        painter.drawText(rect(), Qt::AlignCenter, cameraName_);
    }
}

void CameraTileWidget::enterEvent(QEnterEvent *event) {
    QWidget::enterEvent(event);
}

void CameraTileWidget::leaveEvent(QEvent *event) {
    QWidget::leaveEvent(event);
}

void CameraTileWidget::mouseDoubleClickEvent(QMouseEvent *event) {
    emit menuRequested();
    QWidget::mouseDoubleClickEvent(event);
}
