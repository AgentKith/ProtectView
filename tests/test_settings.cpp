#include <QtTest>
#include <QApplication>
#include "ui/settingsdialog.h"

class TestSettings : public QObject {
    Q_OBJECT
private slots:
    void testConstruction() {
        SettingsDialog dialog;
        QCOMPARE(dialog.windowTitle(), "Settings");
    }

    void testDefaultValues() {
        SettingsDialog dialog;
        QVERIFY(dialog.unvrHost().isEmpty());
        QVERIFY(dialog.apiKey().isEmpty());
        QCOMPARE(dialog.videoQuality(), 1);
        QVERIFY(!dialog.skipTlsVerify());
        QCOMPARE(dialog.retryInterval(), 5);
    }

    void testSetValue() {
        SettingsDialog dialog;
        dialog.findChildren<QLineEdit *>().first()->setText("192.168.1.100");
        QCOMPARE(dialog.unvrHost(), "192.168.1.100");
    }
};

QTEST_MAIN(TestSettings)
#include "test_settings.moc"
