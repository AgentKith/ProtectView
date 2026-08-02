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

    explicit AppController(bool kioskMode = false, QObject *parent = nullptr);

    void initialize();
    State getState() const;
    AppConfig getConfig() const;
    void setConfig(const AppConfig &config);
    MainWindow *getMainWindow() const;

    void retryCamera(int index);
    void showSettingsDialog();

    bool loadConfig();
    void saveConfig();

signals:
    void stateChanged(State state);
    void camerasLoaded(MainWindow *window);

private slots:
    void onCamerasLoaded(const QList<CameraInfo> &cameras);

private:
    void showWizard();
    void showSettings();
    void connectToUNVR();
    QString getConfigDir() const;
    QString getConfigPath() const;
    void setState(State newState);

    State state_;
    AppConfig config_;
    bool kioskMode_;
    UNVRClient *client_;
    CameraManager *manager_;
    QPointer<MainWindow> window_;
};
