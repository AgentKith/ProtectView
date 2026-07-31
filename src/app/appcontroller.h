#pragma once

#include <QObject>
#include <QPointer>
#include <QString>
#include "app/config.h"
#include "unvr/types.h"

class UNVRClient;
class CameraManager;
class MainWindow;

class AppController : public QObject {
    Q_OBJECT
public:
    enum class State {
        NoConfig,
        Wizard,
        Settings,
        Connecting,
        Connected
    };

    explicit AppController(QObject *parent = nullptr);

    void initialize();
    State getState() const;
    AppConfig getConfig() const;
    MainWindow *getMainWindow() const;

    void retryCamera(int index);
    void showSettingsDialog();

signals:
    void stateChanged(State state);
    void camerasLoaded(MainWindow *window);

private slots:
    void onCamerasLoaded(const QList<CameraInfo> &cameras);
    void onStreamCreated(const StreamResponse &response);

private:
    bool loadConfig();
    void showWizard();
    void showSettings();
    void saveConfig();
    void connectToUNVR();
    QString getConfigDir() const;
    QString getConfigPath() const;
    void setState(State newState);

    State state_;
    AppConfig config_;
    UNVRClient *client_;
    CameraManager *manager_;
    QPointer<MainWindow> window_;
};
