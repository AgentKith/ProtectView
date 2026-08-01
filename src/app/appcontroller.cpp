#include "appcontroller.h"
#include <QCoreApplication>
#include <QMessageBox>
#include <QDir>
#include <QStandardPaths>
#include <QFile>
#include <QUuid>
#include <QJsonDocument>
#include "app/storage.h"
#include "app/fingerprint.h"
#include "app/cameramanager.h"
#include "unvr/client.h"
#include "ui/setupwizard.h"
#include "ui/mainwindow.h"
#include "ui/settingsdialog.h"
#include "ui/pinverifydialog.h"

AppController::AppController(bool kioskMode, QObject *parent)
    : QObject(parent),
      state_(State::NoConfig),
      kioskMode_(kioskMode),
      client_(nullptr),
      manager_(nullptr),
      window_(nullptr) {
}

void AppController::initialize() {
    if (!loadConfig()) {
        showWizard();
        return;
    }

    connectToUNVR();
}

bool AppController::loadConfig() {
    QString configDir = getConfigDir();
    QString configPath = getConfigPath();
    if (!QFile::exists(configPath)) {
        return false;
    }

    QString fingerprint = getDeviceFingerprint();
    Storage storage(configDir, fingerprint);
    QByteArray encrypted = storage.load("config.enc");
    QByteArray decrypted = storage.decrypt(encrypted);
    QJsonDocument doc = QJsonDocument::fromJson(decrypted);
    config_ = AppConfig::fromJson(doc.object());
    if (config_.unvr.host.isEmpty()) {
        return false;
    }
    return true;
}

void AppController::showWizard() {
    setState(State::Wizard);

    SetupWizard wizard;
    if (wizard.exec() == QDialog::Accepted) {
        config_.unvr.host = wizard.getHost();
        config_.unvr.apiKey = wizard.getApiKey();
        config_.unvr.tlsMode = wizard.getTlsMode();
        config_.unvr.tlsFingerprint = wizard.getTlsFingerprint();
        config_.pinHash = wizard.getPinHash();

        saveConfig();
        connectToUNVR();
    } else {
        QMessageBox::information(nullptr, "Setup Cancelled",
                                  "Setup was cancelled. Please restart the application to configure.");
        QCoreApplication::quit();
    }
}

void AppController::connectToUNVR() {
    setState(State::Connecting);

    client_ = new UNVRClient(config_.unvr.host, config_.unvr.apiKey,
                              config_.unvr.tlsMode, config_.unvr.tlsFingerprint, this);
    manager_ = new CameraManager(config_, client_, this);

    connect(client_, &UNVRClient::camerasLoaded, this, &AppController::onCamerasLoaded);
    connect(client_, &UNVRClient::error, this, [this](const QString &message) {
        QMessageBox::critical(nullptr, "Connection Error", message);
        setState(State::Settings);
    });

    client_->loadCameras();
}

void AppController::onCamerasLoaded(const QList<CameraInfo> &cameras) {
    setState(State::Connected);

    manager_->setCameras(cameras);

    window_ = new MainWindow;
    window_->setKioskMode(kioskMode_);
    for (int i = 0; i < manager_->cameraCount(); ++i) {
        window_->addCamera(manager_->cameraName(i));
    }

    connect(manager_, &CameraManager::frameReady, window_, &MainWindow::setCameraFrame);
    connect(manager_, &CameraManager::errorOccurred, window_, &MainWindow::setCameraError);
    connect(window_, &MainWindow::retryCamera, this, &AppController::retryCamera);
    connect(window_, &MainWindow::settingsRequested, this, &AppController::showSettingsDialog);

    window_->show();
    manager_->startAll();
    emit camerasLoaded(window_);
}

void AppController::onStreamCreated(const StreamResponse &response) {
    Q_UNUSED(response);
}

void AppController::retryCamera(int index) {
    if (manager_) {
        manager_->startCamera(index);
    }
}

static int qualityIndex(const QString &q) {
    if (q == "low") return 0;
    if (q == "high") return 2;
    return 1;
}

static QString qualityString(int idx) {
    if (idx == 0) return "low";
    if (idx == 2) return "high";
    return "medium";
}

void AppController::showSettingsDialog() {
    if (!config_.pinHash.isEmpty()) {
        PINVerifyDialog pinDialog(config_.pinHash, window_);
        if (pinDialog.exec() != QDialog::Accepted) {
            return;
        }
    }

    SettingsDialog dialog(window_);
    dialog.loadFrom(config_.unvr.host, config_.unvr.apiKey, config_.video.ffmpegPath,
                    qualityIndex(config_.video.quality),
                    config_.unvr.tlsMode == TLSMode::Skip,
                    5);
    if (dialog.exec() == QDialog::Accepted) {
        config_.unvr.host = dialog.unvrHost();
        config_.unvr.apiKey = dialog.apiKey();
        config_.video.ffmpegPath = dialog.ffmpegPath();
        config_.video.quality = qualityString(dialog.videoQuality());
        config_.unvr.tlsMode = dialog.skipTlsVerify() ? TLSMode::Skip : TLSMode::Verify;
        saveConfig();
    }
}

AppConfig AppController::getConfig() const {
    return config_;
}

void AppController::setConfig(const AppConfig &config) {
    config_ = config;
}

MainWindow *AppController::getMainWindow() const {
    return window_;
}

void AppController::saveConfig() {
    QString configDir = getConfigDir();
    QString fingerprint = getDeviceFingerprint();
    Storage storage(configDir, fingerprint);

    QJsonDocument doc(config_.toJson());
    QByteArray json = doc.toJson();
    QByteArray encrypted = storage.encrypt(json);
    storage.save("config.enc", encrypted);
}

QString AppController::getConfigDir() const {
    return QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/ProtectView";
}

QString AppController::getConfigPath() const {
    return getConfigDir() + "/config.enc";
}

void AppController::setState(State newState) {
    state_ = newState;
    emit stateChanged(state_);
}

AppController::State AppController::getState() const {
    return state_;
}
