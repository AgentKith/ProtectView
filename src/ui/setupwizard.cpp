#include "setupwizard.h"
#include <QMessageBox>
#include <QFile>
#include <QRandomGenerator>
#include <QGraphicsDropShadowEffect>
#include <QShowEvent>
#include <QHideEvent>

// ConnectionPage

ConnectionPage::ConnectionPage(QWidget *parent)
    : QWizardPage(parent),
      tested_(false),
      tcpSocket_(nullptr),
      nam_(new QNetworkAccessManager(this)) {
    setTitle("Connection");
    setSubTitle("Enter your UNVR host and API key, then test the connection.");

    QFormLayout *layout = new QFormLayout;

    hostEdit_ = new QLineEdit;
    hostEdit_->setPlaceholderText("192.168.1.100");
    hostEdit_->setObjectName("hostEdit");
    setError(hostEdit_, true);
    layout->addRow("Host:", hostEdit_);

    apiKeyEdit_ = new QLineEdit;
    apiKeyEdit_->setEchoMode(QLineEdit::Password);
    apiKeyEdit_->setPlaceholderText("Enter API key");
    apiKeyEdit_->setObjectName("apiKeyEdit");
    setError(apiKeyEdit_, true);
    layout->addRow("API Key:", apiKeyEdit_);

    tlsVerifyCheck_ = new QCheckBox("Verify TLS certificate");
    tlsVerifyCheck_->setChecked(true);
    layout->addRow("", tlsVerifyCheck_);

    QHBoxLayout *fpLayout = new QHBoxLayout;
    fingerprintEdit_ = new QLineEdit;
    fingerprintEdit_->setPlaceholderText("SHA256 fingerprint");
    fingerprintEdit_->setObjectName("fingerprintEdit");
    fingerprintEdit_->setVisible(false);
    detectButton_ = new QPushButton("Detect");
    detectButton_->setObjectName("detectButton");
    detectButton_->setEnabled(false);
    detectButton_->setVisible(false);
    fpLayout->addWidget(fingerprintEdit_);
    fpLayout->addWidget(detectButton_);
    layout->addRow("Fingerprint:", fpLayout);

    testButton_ = new QPushButton("Test Connection");
    testButton_->setObjectName("testButton");
    layout->addRow("", testButton_);

    resultLabel_ = new QLabel;
    resultLabel_->setObjectName("resultLabel");
    layout->addRow("", resultLabel_);

    setLayout(layout);

    registerField("host*", hostEdit_);
    registerField("apiKey*", apiKeyEdit_);

    connect(hostEdit_, &QLineEdit::textChanged, this, &ConnectionPage::onHostChanged);
    connect(hostEdit_, &QLineEdit::textChanged, this, &QWizardPage::completeChanged);
    connect(apiKeyEdit_, &QLineEdit::textChanged, this, &ConnectionPage::onApiKeyChanged);
    connect(apiKeyEdit_, &QLineEdit::textChanged, this, &QWizardPage::completeChanged);
    connect(tlsVerifyCheck_, &QCheckBox::toggled, this, &ConnectionPage::onTlsToggled);
    connect(testButton_, &QPushButton::clicked, this, &ConnectionPage::onTestConnection);
    connect(detectButton_, &QPushButton::clicked, this, &ConnectionPage::onDetectFingerprint);
}

QString ConnectionPage::getHost() const {
    return hostEdit_->text();
}

QString ConnectionPage::getApiKey() const {
    return apiKeyEdit_->text();
}

TLSMode ConnectionPage::getTlsMode() const {
    if (tlsVerifyCheck_->isChecked())
        return TLSMode::Verify;
    if (!fingerprintEdit_->text().isEmpty())
        return TLSMode::Fingerprint;
    return TLSMode::Skip;
}

QString ConnectionPage::getTlsFingerprint() const {
    return fingerprintEdit_->text();
}

bool ConnectionPage::isComplete() const {
    return tested_;
}

void ConnectionPage::onHostChanged() {
    setError(hostEdit_, hostEdit_->text().isEmpty());
    updateDetectButton();
}

void ConnectionPage::onApiKeyChanged() {
    setError(apiKeyEdit_, apiKeyEdit_->text().isEmpty());
}

void ConnectionPage::onTlsToggled(bool checked) {
    bool showFp = !checked;
    fingerprintEdit_->setVisible(showFp);
    detectButton_->setVisible(showFp);
    updateDetectButton();
}

void ConnectionPage::updateDetectButton() {
    bool enable = !tlsVerifyCheck_->isChecked() && !hostEdit_->text().isEmpty();
    detectButton_->setEnabled(enable);
}

void ConnectionPage::setError(QLineEdit *edit, bool error) {
    edit->setProperty("error", error);
    edit->setStyleSheet(error ?
        "QLineEdit { border: 2px solid #EF4444; padding: 16px 24px; }" :
        "QLineEdit { border: 2px solid #2A2A2A; padding: 16px 24px; }");
}

void ConnectionPage::showTestResult(bool success, const QString &message, QLineEdit *errorField) {
    tested_ = success;
    resultLabel_->setText(message);
    resultLabel_->setStyleSheet(success ?
        "QLabel { color: #22C55E; font-weight: 700; font-size: 28px; }" :
        "QLabel { color: #EF4444; font-size: 28px; }");
    if (errorField) {
        setError(errorField, true);
    }
    completeChanged();
    testButton_->setEnabled(true);
    testButton_->setText("Test Connection");
}

void ConnectionPage::onTestConnection() {
    if (hostEdit_->text().isEmpty()) {
        showTestResult(false, "Host is required", hostEdit_);
        return;
    }
    if (apiKeyEdit_->text().isEmpty()) {
        showTestResult(false, "API key is required", apiKeyEdit_);
        return;
    }

    testButton_->setEnabled(false);
    testButton_->setText("Testing...");
    resultLabel_->clear();

    QString host = hostEdit_->text();
    tcpSocket_ = new QTcpSocket(this);
    connect(tcpSocket_, &QTcpSocket::connected, this, &ConnectionPage::onTcpConnected);
    connect(tcpSocket_, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::errorOccurred),
            this, &ConnectionPage::onTcpError);
    tcpSocket_->connectToHost(host, 443);
}

void ConnectionPage::onTcpConnected() {
    tcpSocket_->disconnectFromHost();
    tcpSocket_->deleteLater();
    tcpSocket_ = nullptr;

    QString host = hostEdit_->text();
    QString apiKey = apiKeyEdit_->text();

    QUrl url("https://" + host + "/proxy/protect/integration/v1/cameras");
    QNetworkRequest request(url);
    request.setRawHeader("X-API-Key", apiKey.toUtf8());
    request.setRawHeader("Accept", "application/json");

    QSslConfiguration config = QSslConfiguration::defaultConfiguration();
    config.setPeerVerifyMode(QSslSocket::VerifyNone);
    request.setSslConfiguration(config);

    QNetworkReply *reply = nam_->get(request);
    connect(reply, &QNetworkReply::finished, this, &ConnectionPage::onApiReplyFinished);
}

void ConnectionPage::onTcpError(QAbstractSocket::SocketError error) {
    tcpSocket_->deleteLater();
    tcpSocket_ = nullptr;

    QString message;
    switch (error) {
        case QAbstractSocket::HostNotFoundError:
            message = "No route to host";
            break;
        case QAbstractSocket::ConnectionRefusedError:
            message = "Connection refused";
            break;
        case QAbstractSocket::SocketTimeoutError:
            message = "Connection timed out";
            break;
        default:
            message = "Cannot connect to host";
            break;
    }
    showTestResult(false, message, hostEdit_);
}

void ConnectionPage::onApiReplyFinished() {
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    if (!reply) return;

    if (reply->error() != QNetworkReply::NoError) {
        QString message;
        if (reply->error() == QNetworkReply::SslHandshakeFailedError) {
            message = "SSL handshake failed";
            reply->deleteLater();
            showTestResult(false, message, hostEdit_);
            return;
        }
        int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        if (statusCode == 401) {
            message = "Invalid API key";
        } else if (statusCode == 403) {
            message = "Insufficient rights";
        } else if (statusCode == 404) {
            message = "Endpoint not found (wrong UNVR version?)";
        } else if (statusCode >= 500) {
            message = "Server error (HTTP " + QString::number(statusCode) + ")";
        } else {
            message = reply->errorString();
        }
        reply->deleteLater();
        showTestResult(false, message, apiKeyEdit_);
        return;
    }

    reply->deleteLater();
    setError(hostEdit_, false);
    setError(apiKeyEdit_, false);
    showTestResult(true, "Connection successful!");
}

void ConnectionPage::onDetectFingerprint() {
    if (hostEdit_->text().isEmpty()) return;

    detectButton_->setEnabled(false);
    detectButton_->setText("Detecting...");

    QString host = hostEdit_->text();
    tcpSocket_ = new QTcpSocket(this);
    connect(tcpSocket_, &QTcpSocket::connected, this, [this, host]() {
        QSslConfiguration config = QSslConfiguration::defaultConfiguration();
        config.setPeerVerifyMode(QSslSocket::VerifyNone);

        QTcpSocket *plainSocket = qobject_cast<QTcpSocket *>(sender());
        if (plainSocket) {
            plainSocket->deleteLater();
            tcpSocket_ = nullptr;
        }

        QSslSocket *sslSocket = new QSslSocket(this);
        sslSocket->setSslConfiguration(config);
        connect(sslSocket, QOverload<const QList<QSslError> &>::of(&QSslSocket::sslErrors),
                this, [this](const QList<QSslError> &errors) {
                    Q_UNUSED(errors);
                });
        connect(sslSocket, &QSslSocket::encrypted, this, [this, sslSocket]() {
            QSslCertificate cert = sslSocket->peerCertificate();
            if (!cert.isNull()) {
                QByteArray digest = cert.digest(QCryptographicHash::Sha256).toHex();
                QString fp = QString(digest).toUpper();
                fp = fp.insert(8, ":").insert(17, ":").insert(26, ":").insert(35, ":")
                         .insert(44, ":").insert(53, ":").insert(62, ":").insert(71, ":");
                fingerprintEdit_->setText(fp);
            }
            sslSocket->disconnectFromHost();
            sslSocket->deleteLater();
            detectButton_->setEnabled(true);
            detectButton_->setText("Detect");
        });
        connect(sslSocket, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::errorOccurred),
                this, [this, sslSocket](QAbstractSocket::SocketError) {
                    sslSocket->deleteLater();
                    detectButton_->setEnabled(true);
                    detectButton_->setText("Detect");
                });
        sslSocket->connectToHostEncrypted(host, 443);
    });
    connect(tcpSocket_, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::errorOccurred),
            this, [this](QAbstractSocket::SocketError) {
                tcpSocket_->deleteLater();
                tcpSocket_ = nullptr;
                detectButton_->setEnabled(true);
                detectButton_->setText("Detect");
            });
    tcpSocket_->connectToHost(host, 443);
}

// PinPage

PinPage::PinPage(QWidget *parent)
    : QWizardPage(parent) {
    setTitle("Set PIN");
    setSubTitle("Create a 6-digit PIN to protect settings.");

    QFormLayout *layout = new QFormLayout;

    pinEdit_ = new QLineEdit;
    pinEdit_->setEchoMode(QLineEdit::Password);
    pinEdit_->setMaxLength(6);
    pinEdit_->setPlaceholderText("123456");
    pinEdit_->setObjectName("pinEdit");
    QRegularExpressionValidator *pinValidator =
        new QRegularExpressionValidator(QRegularExpression("^[0-9]{0,6}$"), this);
    pinEdit_->setValidator(pinValidator);
    setError(pinEdit_, true);
    layout->addRow("PIN:", pinEdit_);

    confirmEdit_ = new QLineEdit;
    confirmEdit_->setEchoMode(QLineEdit::Password);
    confirmEdit_->setMaxLength(6);
    confirmEdit_->setPlaceholderText("Confirm PIN");
    confirmEdit_->setObjectName("confirmPinEdit");
    QRegularExpressionValidator *confirmValidator =
        new QRegularExpressionValidator(QRegularExpression("^[0-9]{0,6}$"), this);
    confirmEdit_->setValidator(confirmValidator);
    setError(confirmEdit_, true);
    layout->addRow("Confirm PIN:", confirmEdit_);

    mismatchLabel_ = new QLabel("PINs do not match");
    mismatchLabel_->setObjectName("mismatchLabel");
    mismatchLabel_->setStyleSheet("QLabel { color: #EF4444; font-size: 28px; }");
    mismatchLabel_->setVisible(false);
    layout->addRow("", mismatchLabel_);

    setLayout(layout);

    registerField("pin*", pinEdit_);
    registerField("confirmPin*", confirmEdit_);

    connect(pinEdit_, &QLineEdit::textChanged, this, [this]() {
        setError(pinEdit_, pinEdit_->text().isEmpty());
        checkMatch();
        completeChanged();
    });
    connect(confirmEdit_, &QLineEdit::textChanged, this, [this]() {
        setError(confirmEdit_, confirmEdit_->text().isEmpty());
        checkMatch();
        completeChanged();
    });
}

void PinPage::setError(QLineEdit *edit, bool error) {
    edit->setProperty("error", error);
    edit->setStyleSheet(error ?
        "QLineEdit { border: 2px solid #EF4444; padding: 16px 24px; }" :
        "QLineEdit { border: 2px solid #2A2A2A; padding: 16px 24px; }");
}

void PinPage::checkMatch() {
    QString pin = pinEdit_->text();
    QString confirm = confirmEdit_->text();
    if (pin.length() == 6 && confirm.length() == 6) {
        bool match = pin == confirm;
        mismatchLabel_->setVisible(!match);
        setError(confirmEdit_, !match);
    } else {
        mismatchLabel_->setVisible(false);
    }
}

bool PinPage::isComplete() const {
    QString pin = pinEdit_->text();
    QString confirm = confirmEdit_->text();
    if (pin.length() != 6 || confirm.length() != 6 || pin != confirm) {
        return false;
    }

    QByteArray salt;
    salt.reserve(16);

    QFile urandom("/dev/urandom");
    if (urandom.open(QIODevice::ReadOnly)) {
        salt = urandom.read(16);
        urandom.close();
    } else {
        QRandomGenerator *rng = QRandomGenerator::system();
        for (int i = 0; i < 16; ++i) {
            salt.append(static_cast<char>(rng->generate() & 0xFF));
        }
    }

    QByteArray preImage = salt + pin.toUtf8();
    QByteArray hash = QCryptographicHash::hash(preImage, QCryptographicHash::Sha256);
    pinHash_ = salt.toHex() + hash.toHex();
    return true;
}

QString PinPage::getPinHash() const {
    return pinHash_;
}

// SetupWizard

SetupWizard::SetupWizard(QWidget *parent)
    : QWizard(parent) {
    setObjectName("SetupWizard");
    setWindowTitle("Setup UNVR Carousel");
    setWizardStyle(QWizard::ModernStyle);

    {
        QGraphicsDropShadowEffect *glow = new QGraphicsDropShadowEffect(this);
        glow->setColor(QColor(255, 107, 0));
        glow->setBlurRadius(40);
        glow->setOffset(0, 0);
        setGraphicsEffect(glow);
    }

    ConnectionPage *connPage = new ConnectionPage;
    addPage(connPage);

    PinPage *pinPage = new PinPage;
    addPage(pinPage);

    connect(this, &QWizard::finished, this, [this, connPage, pinPage](int) {
        host_ = connPage->getHost();
        apiKey_ = connPage->getApiKey();
        tlsMode_ = connPage->getTlsMode();
        tlsFingerprint_ = connPage->getTlsFingerprint();
        pinHash_ = pinPage->getPinHash();
    });
}

QString SetupWizard::getHost() const {
    return host_;
}

QString SetupWizard::getApiKey() const {
    return apiKey_;
}

QString SetupWizard::getPinHash() const {
    return pinHash_;
}

TLSMode SetupWizard::getTlsMode() const {
    return tlsMode_;
}

QString SetupWizard::getTlsFingerprint() const {
    return tlsFingerprint_;
}

void SetupWizard::showEvent(QShowEvent *event) {
    if (parentWidget()) parentWidget()->clearFocus();
    QWizard::showEvent(event);
}

void SetupWizard::hideEvent(QHideEvent *event) {
    clearFocus();
    QWizard::hideEvent(event);
}
