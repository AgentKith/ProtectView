#include "pinverifydialog.h"
#include "pinpad.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QCryptographicHash>

PINVerifyDialog::PINVerifyDialog(const QString &pinHash, QWidget *parent)
    : QDialog(parent),
      pinPad_(new PINPad(this)),
      pinHash_(pinHash),
      verified_(false) {
    setWindowTitle("Enter PIN");
    setFixedSize(320, 400);
    setModal(true);

    QLabel *label = new QLabel("Enter PIN to access settings");
    label->setAlignment(Qt::AlignCenter);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(label);
    layout->addWidget(pinPad_);
    setLayout(layout);

    connect(pinPad_, &PINPad::pinAccepted, this, [this, label]() {
        if (verifyPin(pinPad_->enteredPin())) {
            verified_ = true;
            emit verified();
            accept();
        } else {
            label->setText("Incorrect PIN, try again");
            label->setStyleSheet("color: #ef4444;");
        }
    });

    connect(pinPad_, &PINPad::pinRejected, this, [this, label]() {
        label->setText("Incorrect PIN, try again");
        label->setStyleSheet("color: #ef4444;");
    });
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
