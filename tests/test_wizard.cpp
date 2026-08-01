#include <QtTest>
#include <QApplication>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QCheckBox>
#include <QMessageBox>
#include <QCryptographicHash>
#include <QRegularExpression>
#include "../src/ui/setupwizard.h"

class TestWizard : public QObject {
    Q_OBJECT

private slots:
    void testWizardCreation();
    void testWizardPages();
    void testConnectionValidation();
    void testTlsToggle();
    void testPinValidation();
    void testPinMismatch();
    void testFinishReturnsConfig();
    void testPinHashFormat();
    void testPinHashIsVerifiable();
};

void TestWizard::testWizardCreation() {
    SetupWizard wizard;
    QVERIFY(!wizard.windowTitle().isEmpty());
}

void TestWizard::testWizardPages() {
    SetupWizard wizard;

    QWizardPage *connPage = wizard.page(0);
    QVERIFY(connPage != nullptr);
    QCOMPARE(connPage->title(), "Connection");

    QWizardPage *pinPage = wizard.page(1);
    QVERIFY(pinPage != nullptr);
    QCOMPARE(pinPage->title(), "Set PIN");
}

void TestWizard::testConnectionValidation() {
    SetupWizard wizard;
    wizard.setCurrentId(0);

    QWizardPage *connPage = wizard.page(0);
    QVERIFY(connPage != nullptr);
    QVERIFY(!connPage->isComplete());

    QLineEdit *hostEdit = wizard.findChild<QLineEdit *>("hostEdit");
    QVERIFY(hostEdit != nullptr);
    QLineEdit *apiKeyEdit = wizard.findChild<QLineEdit *>("apiKeyEdit");
    QVERIFY(apiKeyEdit != nullptr);

    hostEdit->setText("192.168.1.100");
    apiKeyEdit->setText("test-api-key-12345");

    QVERIFY(!connPage->isComplete());
}

void TestWizard::testTlsToggle() {
    SetupWizard wizard;
    wizard.setCurrentId(0);

    QCheckBox *tlsCheck = wizard.findChild<QCheckBox *>();
    QVERIFY(tlsCheck != nullptr);
    QVERIFY(tlsCheck->isChecked());

    QLineEdit *fingerprintEdit = wizard.findChild<QLineEdit *>("fingerprintEdit");
    QVERIFY(fingerprintEdit != nullptr);
    QVERIFY(!fingerprintEdit->isVisible());

    QPushButton *detectButton = wizard.findChild<QPushButton *>("detectButton");
    QVERIFY(detectButton != nullptr);
    QVERIFY(!detectButton->isVisible());
}

void TestWizard::testPinValidation() {
    SetupWizard wizard;
    wizard.setCurrentId(1);

    QWizardPage *pinPage = wizard.page(1);
    QVERIFY(pinPage != nullptr);
    QVERIFY(!pinPage->isComplete());

    QLineEdit *pinEdit = wizard.findChild<QLineEdit *>("pinEdit");
    QVERIFY(pinEdit != nullptr);
    QLineEdit *confirmEdit = wizard.findChild<QLineEdit *>("confirmPinEdit");
    QVERIFY(confirmEdit != nullptr);

    pinEdit->setText("12345");
    QVERIFY(!pinPage->isComplete());

    pinEdit->setText("123456");
    QVERIFY(!pinPage->isComplete());

    confirmEdit->setText("123456");
    QVERIFY(pinPage->isComplete());
}

void TestWizard::testPinMismatch() {
    SetupWizard wizard;
    wizard.setCurrentId(1);

    QLineEdit *pinEdit = wizard.findChild<QLineEdit *>("pinEdit");
    pinEdit->setText("123456");

    QLineEdit *confirmEdit = wizard.findChild<QLineEdit *>("confirmPinEdit");
    confirmEdit->setText("654321");

    QWizardPage *pinPage = wizard.page(1);
    QVERIFY(!pinPage->isComplete());

    QLabel *mismatchLabel = wizard.findChild<QLabel *>("mismatchLabel");
    QVERIFY(mismatchLabel != nullptr);
}

void TestWizard::testFinishReturnsConfig() {
    SetupWizard wizard;

    wizard.setCurrentId(0);
    QLineEdit *hostEdit = wizard.findChild<QLineEdit *>("hostEdit");
    hostEdit->setText("192.168.1.100");
    QLineEdit *apiKeyEdit = wizard.findChild<QLineEdit *>("apiKeyEdit");
    apiKeyEdit->setText("test-api-key");

    wizard.setCurrentId(1);
    QLineEdit *pinEdit = wizard.findChild<QLineEdit *>("pinEdit");
    pinEdit->setText("123456");
    QLineEdit *confirmEdit = wizard.findChild<QLineEdit *>("confirmPinEdit");
    confirmEdit->setText("123456");

    wizard.accept();

    QCOMPARE(wizard.getHost(), "192.168.1.100");
    QCOMPARE(wizard.getApiKey(), "test-api-key");
    QVERIFY(!wizard.getPinHash().isEmpty());
}

void TestWizard::testPinHashFormat() {
    SetupWizard wizard;

    wizard.setCurrentId(0);
    QLineEdit *hostEdit = wizard.findChild<QLineEdit *>("hostEdit");
    hostEdit->setText("192.168.1.100");
    QLineEdit *apiKeyEdit = wizard.findChild<QLineEdit *>("apiKeyEdit");
    apiKeyEdit->setText("test-api-key");

    wizard.setCurrentId(1);
    QLineEdit *pinEdit = wizard.findChild<QLineEdit *>("pinEdit");
    pinEdit->setText("123456");
    QLineEdit *confirmEdit = wizard.findChild<QLineEdit *>("confirmPinEdit");
    confirmEdit->setText("123456");

    wizard.accept();

    QString hash = wizard.getPinHash();
    QCOMPARE(hash.length(), 96);

    QByteArray salt = QByteArray::fromHex(hash.left(32).toUtf8());
    QCOMPARE(salt.size(), 16);

    QString hashPart = hash.mid(32);
    QCOMPARE(hashPart.length(), 64);

    QRegularExpression hexRe("^[0-9a-f]+$");
    QVERIFY(hexRe.match(hash).hasMatch());
}

void TestWizard::testPinHashIsVerifiable() {
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

    wizard.accept();

    QString hash = wizard.getPinHash();

    QByteArray salt = QByteArray::fromHex(hash.left(32).toUtf8());
    QString storedHash = hash.mid(32);

    QByteArray preImage = salt + testPin.toUtf8();
    QByteArray computedHash = QCryptographicHash::hash(preImage, QCryptographicHash::Sha256);

    QCOMPARE(computedHash.toHex(), storedHash.toUtf8());
}

QTEST_MAIN(TestWizard)
#include "test_wizard.moc"
