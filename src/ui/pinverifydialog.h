#pragma once

#include <QDialog>
#include <QLabel>
#include <QString>

class PINPad;

class PINVerifyDialog : public QDialog {
    Q_OBJECT
public:
    explicit PINVerifyDialog(const QString &pinHash, QWidget *parent = nullptr);
    bool wasVerified() const;

signals:
    void verified();

private:
    bool verifyPin(const QString &enteredPin) const;
    void onPinSubmitted(const QString &pin);

    PINPad *pinPad_;
    QString pinHash_;
    bool verified_;
    QLabel *errorLabel_;
};
