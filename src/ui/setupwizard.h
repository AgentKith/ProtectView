#pragma once

#include <QWizard>
#include <QWizardPage>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QCheckBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QString>
#include <QTcpSocket>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QSslCertificate>
#include <QSslError>
#include <QSslSocket>
#include <QCryptographicHash>
#include <QRegularExpressionValidator>
#include "app/config.h"

class ConnectionPage : public QWizardPage {
    Q_OBJECT
public:
    explicit ConnectionPage(QWidget *parent = nullptr);
    QString getHost() const;
    QString getApiKey() const;
    TLSMode getTlsMode() const;
    QString getTlsFingerprint() const;
protected:
    bool isComplete() const override;
private slots:
    void onTestConnection();
    void onDetectFingerprint();
    void onTlsToggled(bool checked);
    void onHostChanged();
    void onApiKeyChanged();
private slots:
    void onTcpConnected();
    void onTcpError(QAbstractSocket::SocketError);
    void onApiReplyFinished();
private:
    void setError(QLineEdit *edit, bool error);
    void showTestResult(bool success, const QString &message, QLineEdit *errorField = nullptr);
    void updateDetectButton();

    QLineEdit *hostEdit_;
    QLineEdit *apiKeyEdit_;
    QCheckBox *tlsVerifyCheck_;
    QLineEdit *fingerprintEdit_;
    QPushButton *detectButton_;
    QPushButton *testButton_;
    QLabel *resultLabel_;
    bool tested_;
    QTcpSocket *tcpSocket_;
    QNetworkAccessManager *nam_;
};

class PinPage : public QWizardPage {
    Q_OBJECT
public:
    explicit PinPage(QWidget *parent = nullptr);
    QString getPinHash() const;
protected:
    bool isComplete() const override;
private:
    void setError(QLineEdit *edit, bool error);
    void checkMatch();

    QLineEdit *pinEdit_;
    QLineEdit *confirmEdit_;
    QLabel *mismatchLabel_;
    mutable QString pinHash_;
};

class SetupWizard : public QWizard {
    Q_OBJECT
public:
    explicit SetupWizard(QWidget *parent = nullptr);
    QString getHost() const;
    QString getApiKey() const;
    QString getPinHash() const;
    TLSMode getTlsMode() const;
    QString getTlsFingerprint() const;

private:
    QString host_;
    QString apiKey_;
    QString pinHash_;
    TLSMode tlsMode_;
    QString tlsFingerprint_;
};
