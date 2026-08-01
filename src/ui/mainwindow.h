#pragma once

#include <QMainWindow>
#include <QGridLayout>
#include <QVector>
#include <QString>

class CameraTileWidget;
class EdgeMenu;

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    void setKioskMode(bool kiosk);
    void addCamera(const QString &cameraName);
    void setCameraFrame(int index, const QImage &frame);
    void setCameraError(int index, const QString &message);
    int cameraCount() const;

signals:
    void settingsRequested();
    void retryCamera(int index);

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void onMenuRequested();

private:
    void arrangeGrid();
    void grabKioskKeys();

    QGridLayout *gridLayout_;
    QVector<CameraTileWidget *> tiles_;
    EdgeMenu *edgeMenu_;
    bool kioskMode_;
};
