#include "pinverifydialog.h"
#include "pinpad.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QCryptographicHash>

PINVerifyDialog::PINVerifyDialog(const QString &pinHash, QWidget *parent)
    : QDialog(parent),
      pinPad_(new PINPad(this)),
      pinHash_(pinHash),
      verified_(false),
      errorLabel_(new QLabel("Enter PIN to access settings")) {
    setWindowTitle("Enter PIN");
    setFixedSize(320, 400);
    setModal(true);

    errorLabel_->setAlignment(Qt::AlignCenter);

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
        errorLabel_->setStyleSheet("color: #ef4444;");
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
