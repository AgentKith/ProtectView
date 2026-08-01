#include <QtTest>
#include <QApplication>
#include <QKeyEvent>
#include <QCryptographicHash>
#include <QRegularExpression>
#include <QFile>
#include <QRandomGenerator>
#include "ui/pinpad.h"
#include "ui/pinverifydialog.h"
#include "ui/setupwizard.h"

class TestPINVerify : public QObject {
    Q_OBJECT
private slots:
    void testPinHashFormat();
    void testPinHashRoundtrip();
    void testWrongPinFails();
    void testEmptyPinFails();
    void testShortHashFails();
    void testPinSubmittedSignal();
    void testPinSubmittedClearsDisplay();
    void testDialogAcceptsCorrectPin();
    void testDialogRejectsWrongPin();
    void testDialogDismissOnEscape();
    void testWizardPinHashIsVerifiable();
    void testMultiplePinAttempts();
};

static QString generatePinHash(const QString &pin) {
    QByteArray salt;
    salt.reserve(16);

    QFile urandom("/dev/urandom");
    if (urandom.open(QIODevice::ReadOnly)) {
        salt = urandom.read(16);
        urandom.close();
    } else {
        for (int i = 0; i < 16; ++i) {
            salt.append(static_cast<char>(QRandomGenerator::system()->generate() & 0xFF));
        }
    }

    QByteArray preImage = salt + pin.toUtf8();
    QByteArray hash = QCryptographicHash::hash(preImage, QCryptographicHash::Sha256);
    return salt.toHex() + hash.toHex();
}

void TestPINVerify::testPinHashFormat() {
    QString hash = generatePinHash("123456");

    QCOMPARE(hash.length(), 96);

    QByteArray salt = QByteArray::fromHex(hash.left(32).toUtf8());
    QCOMPARE(salt.size(), 16);

    QString hashPart = hash.mid(32);
    QCOMPARE(hashPart.length(), 64);

    QRegularExpression hexRe("^[0-9a-f]+$");
    QVERIFY(hexRe.match(hash).hasMatch());
}

void TestPINVerify::testPinHashRoundtrip() {
    QString pin = "123456";
    QString hash = generatePinHash(pin);

    QByteArray salt = QByteArray::fromHex(hash.left(32).toUtf8());
    QString storedHash = hash.mid(32);

    QByteArray preImage = salt + pin.toUtf8();
    QByteArray computedHash = QCryptographicHash::hash(preImage, QCryptographicHash::Sha256);

    QCOMPARE(computedHash.toHex(), storedHash.toUtf8());
}

void TestPINVerify::testWrongPinFails() {
    QString correctPin = "123456";
    QString wrongPin = "654321";
    QString hash = generatePinHash(correctPin);

    QByteArray salt = QByteArray::fromHex(hash.left(32).toUtf8());
    QString storedHash = hash.mid(32);

    QByteArray preImage = salt + wrongPin.toUtf8();
    QByteArray computedHash = QCryptographicHash::hash(preImage, QCryptographicHash::Sha256);

    QVERIFY(computedHash.toHex() != storedHash.toUtf8());
}

void TestPINVerify::testEmptyPinFails() {
    QString pin = "123456";
    QString hash = generatePinHash(pin);

    QByteArray salt = QByteArray::fromHex(hash.left(32).toUtf8());
    QString storedHash = hash.mid(32);

    QByteArray preImage = salt + QString("").toUtf8();
    QByteArray computedHash = QCryptographicHash::hash(preImage, QCryptographicHash::Sha256);

    QVERIFY(computedHash.toHex() != storedHash.toUtf8());
}

void TestPINVerify::testShortHashFails() {
    PINVerifyDialog dialog("short_hash", nullptr);
    QVERIFY(!dialog.wasVerified());
}

void TestPINVerify::testPinSubmittedSignal() {
    PINPad pad;
    pad.setFocus();

    QKeyEvent press1(QEvent::KeyPress, Qt::Key_1, Qt::NoModifier);
    QKeyEvent press2(QEvent::KeyPress, Qt::Key_2, Qt::NoModifier);
    QKeyEvent press3(QEvent::KeyPress, Qt::Key_3, Qt::NoModifier);
    QKeyEvent enter(QEvent::KeyPress, Qt::Key_Return, Qt::NoModifier);

    QApplication::sendEvent(&pad, &press1);
    QApplication::sendEvent(&pad, &press2);
    QApplication::sendEvent(&pad, &press3);

    QString submittedPin;
    bool signalFired = false;
    connect(&pad, &PINPad::pinSubmitted, [&submittedPin, &signalFired](const QString &pin) {
        signalFired = true;
        submittedPin = pin;
    });

    QApplication::sendEvent(&pad, &enter);

    QVERIFY(signalFired);
    QCOMPARE(submittedPin, "123");
}

void TestPINVerify::testPinSubmittedClearsDisplay() {
    PINPad pad;
    pad.setFocus();

    QKeyEvent press1(QEvent::KeyPress, Qt::Key_1, Qt::NoModifier);
    QKeyEvent press2(QEvent::KeyPress, Qt::Key_2, Qt::NoModifier);
    QKeyEvent enter(QEvent::KeyPress, Qt::Key_Return, Qt::NoModifier);

    QApplication::sendEvent(&pad, &press1);
    QApplication::sendEvent(&pad, &press2);
    QCOMPARE(pad.enteredPin().length(), 2);

    QApplication::sendEvent(&pad, &enter);
    QVERIFY(pad.enteredPin().isEmpty());
}

void TestPINVerify::testDialogAcceptsCorrectPin() {
    QString pin = "123456";
    QString hash = generatePinHash(pin);

    PINVerifyDialog dialog(hash, nullptr);

    PINPad *pad = dialog.findChild<PINPad *>();
    QVERIFY(pad != nullptr);
    pad->setFocus();

    for (QChar c : pin) {
        int digit = c.digitValue();
        QKeyEvent keyEvent(QEvent::KeyPress, Qt::Key_0 + digit, Qt::NoModifier);
        QApplication::sendEvent(pad, &keyEvent);
    }

    QKeyEvent enter(QEvent::KeyPress, Qt::Key_Return, Qt::NoModifier);
    QApplication::sendEvent(pad, &enter);

    QTRY_VERIFY_WITH_TIMEOUT(dialog.wasVerified(), 500);
}

void TestPINVerify::testDialogRejectsWrongPin() {
    QString correctPin = "123456";
    QString wrongPin = "654321";
    QString hash = generatePinHash(correctPin);

    PINVerifyDialog dialog(hash, nullptr);

    PINPad *pad = dialog.findChild<PINPad *>();
    QVERIFY(pad != nullptr);
    pad->setFocus();

    for (QChar c : wrongPin) {
        int digit = c.digitValue();
        QKeyEvent keyEvent(QEvent::KeyPress, Qt::Key_0 + digit, Qt::NoModifier);
        QApplication::sendEvent(pad, &keyEvent);
    }

    QKeyEvent enter(QEvent::KeyPress, Qt::Key_Return, Qt::NoModifier);
    QApplication::sendEvent(pad, &enter);

    QTest::qWait(100);
    QVERIFY(!dialog.wasVerified());
}

void TestPINVerify::testDialogDismissOnEscape() {
    QString pin = "123456";
    QString hash = generatePinHash(pin);

    PINVerifyDialog dialog(hash, nullptr);

    PINPad *pad = dialog.findChild<PINPad *>();
    QVERIFY(pad != nullptr);
    pad->setFocus();

    bool dismissed = false;
    connect(&dialog, &QDialog::finished, [&dismissed](int result) {
        if (result == QDialog::Rejected) {
            dismissed = true;
        }
    });

    QKeyEvent escape(QEvent::KeyPress, Qt::Key_Escape, Qt::NoModifier);
    QApplication::sendEvent(pad, &escape);

    QTRY_VERIFY_WITH_TIMEOUT(dismissed, 500);
    QVERIFY(!dialog.wasVerified());
}

void TestPINVerify::testWizardPinHashIsVerifiable() {
    SetupWizard wizard;

    wizard.setCurrentId(0);
    QLineEdit *hostEdit = wizard.findChild<QLineEdit *>("hostEdit");
    hostEdit->setText("192.168.1.100");
    QLineEdit *apiKeyEdit = wizard.findChild<QLineEdit *>("apiKeyEdit");
    apiKeyEdit->setText("test-api-key");

    QString testPin = "987654";
    wizard.setCurrentId(1);
    QLineEdit *pinEdit = wizard.findChild<QLineEdit *>("pinEdit");
    pinEdit->setText(testPin);
    QLineEdit *confirmEdit = wizard.findChild<QLineEdit *>("confirmPinEdit");
    confirmEdit->setText(testPin);

    QWizardPage *pinPage = wizard.page(1);
    QVERIFY(pinPage->isComplete());

    wizard.accept();

    QString hash = wizard.getPinHash();
    QVERIFY(!hash.isEmpty());
    QCOMPARE(hash.length(), 96);

    QByteArray salt = QByteArray::fromHex(hash.left(32).toUtf8());
    QString storedHash = hash.mid(32);

    QByteArray preImage = salt + testPin.toUtf8();
    QByteArray computedHash = QCryptographicHash::hash(preImage, QCryptographicHash::Sha256);

    QCOMPARE(computedHash.toHex(), storedHash.toUtf8());
}

void TestPINVerify::testMultiplePinAttempts() {
    QString correctPin = "111111";
    QString hash = generatePinHash(correctPin);

    PINVerifyDialog dialog(hash, nullptr);

    PINPad *pad = dialog.findChild<PINPad *>();
    QVERIFY(pad != nullptr);
    pad->setFocus();

    QString wrongPin = "000000";
    for (QChar c : wrongPin) {
        int digit = c.digitValue();
        QKeyEvent keyEvent(QEvent::KeyPress, Qt::Key_0 + digit, Qt::NoModifier);
        QApplication::sendEvent(pad, &keyEvent);
    }
    QKeyEvent enter1(QEvent::KeyPress, Qt::Key_Return, Qt::NoModifier);
    QApplication::sendEvent(pad, &enter1);
    QTest::qWait(50);
    QVERIFY(!dialog.wasVerified());

    for (QChar c : correctPin) {
        int digit = c.digitValue();
        QKeyEvent keyEvent(QEvent::KeyPress, Qt::Key_0 + digit, Qt::NoModifier);
        QApplication::sendEvent(pad, &keyEvent);
    }
    QKeyEvent enter2(QEvent::KeyPress, Qt::Key_Return, Qt::NoModifier);
    QApplication::sendEvent(pad, &enter2);

    QTRY_VERIFY_WITH_TIMEOUT(dialog.wasVerified(), 500);
}

QTEST_MAIN(TestPINVerify)
#include "test_pin_verify.moc"
