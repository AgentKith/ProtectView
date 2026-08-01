#include <QtTest>
#include <QApplication>
#include <QKeyEvent>
#include <QPushButton>
#include "ui/pinpad.h"

class TestPINPad : public QObject {
    Q_OBJECT
private slots:
    void testConstruction() {
        PINPad pad;
        QVERIFY(pad.enteredPin().isEmpty());
    }

    void testHasTenDigitButtons() {
        PINPad pad;
        pad.setPin("123456");
        QList<QPushButton *> buttons = pad.findChildren<QPushButton *>();
        int digitCount = 0;
        for (QPushButton *btn : buttons) {
            QString text = btn->text();
            if (text == "0" || text == "1" || text == "2" || text == "3" ||
                text == "4" || text == "5" || text == "6" || text == "7" ||
                text == "8" || text == "9") {
                digitCount++;
            }
        }
        QCOMPARE(digitCount, 10);
    }

    void testHasZeroButton() {
        PINPad pad;
        QList<QPushButton *> buttons = pad.findChildren<QPushButton *>();
        bool hasZero = false;
        for (QPushButton *btn : buttons) {
            if (btn->text() == "0") {
                hasZero = true;
                break;
            }
        }
        QVERIFY(hasZero);
    }

    void testSetPin() {
        PINPad pad;
        pad.setPin("123456");
        QVERIFY(pad.enteredPin().isEmpty());
    }

    void testEnteredPin() {
        PINPad pad;
        pad.setPin("123456");
        QVERIFY(pad.enteredPin().isEmpty());
    }

    void testKeyboardDigitInput() {
        PINPad pad;
        pad.setPin("123456");
        pad.setFocus();

        QKeyEvent press1(QEvent::KeyPress, Qt::Key_1, Qt::NoModifier);
        QKeyEvent press2(QEvent::KeyPress, Qt::Key_2, Qt::NoModifier);
        QKeyEvent press3(QEvent::KeyPress, Qt::Key_3, Qt::NoModifier);

        QApplication::sendEvent(&pad, &press1);
        QApplication::sendEvent(&pad, &press2);
        QApplication::sendEvent(&pad, &press3);

        QCOMPARE(pad.enteredPin(), "123");
    }

    void testKeyboardZeroInput() {
        PINPad pad;
        pad.setPin("123456");
        pad.setFocus();

        QKeyEvent press(QEvent::KeyPress, Qt::Key_0, Qt::NoModifier);
        QApplication::sendEvent(&pad, &press);

        QCOMPARE(pad.enteredPin(), "0");
    }

    void testKeyboardEnter() {
        PINPad pad;
        pad.setPin("123");
        pad.setFocus();

        QKeyEvent press1(QEvent::KeyPress, Qt::Key_1, Qt::NoModifier);
        QKeyEvent press2(QEvent::KeyPress, Qt::Key_2, Qt::NoModifier);
        QKeyEvent press3(QEvent::KeyPress, Qt::Key_3, Qt::NoModifier);
        QKeyEvent enter(QEvent::KeyPress, Qt::Key_Return, Qt::NoModifier);

        QApplication::sendEvent(&pad, &press1);
        QApplication::sendEvent(&pad, &press2);
        QApplication::sendEvent(&pad, &press3);
        QApplication::sendEvent(&pad, &enter);

        QVERIFY(pad.enteredPin().isEmpty());
    }

    void testKeyboardBackspace() {
        PINPad pad;
        pad.setPin("123456");
        pad.setFocus();

        QKeyEvent press1(QEvent::KeyPress, Qt::Key_1, Qt::NoModifier);
        QKeyEvent press2(QEvent::KeyPress, Qt::Key_2, Qt::NoModifier);
        QKeyEvent backspace(QEvent::KeyPress, Qt::Key_Backspace, Qt::NoModifier);

        QApplication::sendEvent(&pad, &press1);
        QApplication::sendEvent(&pad, &press2);
        QApplication::sendEvent(&pad, &backspace);

        QCOMPARE(pad.enteredPin(), "1");
    }

    void testKeyboardEscapeClears() {
        PINPad pad;
        pad.setPin("123456");
        pad.setFocus();

        QKeyEvent press1(QEvent::KeyPress, Qt::Key_1, Qt::NoModifier);
        QKeyEvent escape(QEvent::KeyPress, Qt::Key_Escape, Qt::NoModifier);

        QApplication::sendEvent(&pad, &press1);
        QApplication::sendEvent(&pad, &escape);

        QVERIFY(pad.enteredPin().isEmpty());
    }

    void testMaxSixDigits() {
        PINPad pad;
        pad.setPin("123456");
        pad.setFocus();

        for (int i = 0; i < 10; ++i) {
            QKeyEvent press(QEvent::KeyPress, Qt::Key_1, Qt::NoModifier);
            QApplication::sendEvent(&pad, &press);
        }

        QCOMPARE(pad.enteredPin().length(), 6);
    }
};

QTEST_MAIN(TestPINPad)
#include "test_pinpad.moc"
