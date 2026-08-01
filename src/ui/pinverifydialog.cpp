#include "pinverifydialog.h"
#include "pinpad.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QCryptographicHash>
#include <QGraphicsDropShadowEffect>

PINVerifyDialog::PINVerifyDialog(const QString &pinHash, QWidget *parent)
    : QDialog(parent),
      pinPad_(new PINPad(this)),
      pinHash_(pinHash),
      verified_(false),
      errorLabel_(new QLabel("Enter PIN to access settings")) {
    setWindowTitle("Enter PIN");
    setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
    setFixedSize(448, 800);
    setModal(true);

    {
        QGraphicsDropShadowEffect *glow = new QGraphicsDropShadowEffect(this);
        glow->setColor(QColor(255, 107, 0));
        glow->setBlurRadius(40);
        glow->setOffset(0, 0);
        setGraphicsEffect(glow);
    }

    errorLabel_->setAlignment(Qt::AlignCenter);
    errorLabel_->setMinimumHeight(80);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(errorLabel_);
    layout->addWidget(pinPad_);
    setLayout(layout);

    connect(pinPad_, &PINPad::pinSubmitted, this, &PINVerifyDialog::onPinSubmitted);
    connect(pinPad_, &PINPad::pinDismissed, this, &QDialog::reject);
}

void PINVerifyDialog::onPinSubmitted(const QString &pin) {
    if (verifyPin(pin)) {
        verified_ = true;
        emit verified();
        accept();
    } else {
        errorLabel_->setText("Incorrect PIN, try again");
        errorLabel_->setStyleSheet("color: #EF4444; font-size: 28px;");
    }
}

bool PINVerifyDialog::wasVerified() const {
    return verified_;
}

bool PINVerifyDialog::verifyPin(const QString &enteredPin) const {
    if (pinHash_.length() < 64) return false;

    QByteArray salt = QByteArray::fromHex(pinHash_.left(32).toUtf8());
    QString storedHash = pinHash_.mid(32);

    QByteArray preImage = salt + enteredPin.toUtf8();
    QByteArray computedHash = QCryptographicHash::hash(preImage, QCryptographicHash::Sha256);
    return computedHash.toHex() == storedHash.toUtf8();
}
