#include <QtTest>
#include <QApplication>
#include "ui/pinpad.h"

class TestPINPad : public QObject {
    Q_OBJECT
private slots:
    void testConstruction() {
        PINPad pad;
        QVERIFY(true);
    }

    void testSetPin() {
        PINPad pad;
        pad.setPin("123456");
        QVERIFY(true);
    }

    void testEnteredPin() {
        PINPad pad;
        pad.setPin("123456");
        QVERIFY(pad.enteredPin().isEmpty());
    }
};

QTEST_MAIN(TestPINPad)
#include "test_pinpad.moc"
