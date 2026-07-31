#pragma once

#include <QWidget>
#include <QLabel>
#include <QTimer>
#include <QImage>
#include <QString>

class CameraTileWidget : public QWidget {
    Q_OBJECT
public:
    explicit CameraTileWidget(const QString &cameraName, QWidget *parent = nullptr);
    void setFrame(const QImage &frame);
    void setError(const QString &message);
    void clearError();
    QString cameraName() const;

protected:
    void paintEvent(QPaintEvent *event) override;
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;

signals:
    void retryRequested();
    void menuRequested();

private:
    QString cameraName_;
    QImage currentFrame_;
    QString errorMessage_;
    bool hasError_;
    bool hasFrame_;
    QTimer retryTimer_;
};
