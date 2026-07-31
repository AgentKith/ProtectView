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
    void addCamera(const QString &cameraName);
    void setCameraFrame(int index, const QImage &frame);
    void setCameraError(int index, const QString &message);
    int cameraCount() const;

signals:
    void settingsRequested();
    void retryCamera(int index);

private slots:
    void onMenuRequested();

private:
    void arrangeGrid();

    QGridLayout *gridLayout_;
    QVector<CameraTileWidget *> tiles_;
    EdgeMenu *edgeMenu_;
};
