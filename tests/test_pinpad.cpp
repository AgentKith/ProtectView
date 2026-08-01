#include <QtTest>
#include <QApplication>
#include <QKeyEvent>
#include <QPushButton>
#include "ui/pinpad.h"

class TestPINPad : public QObject {
    Q_OBJECT
private slots:
    void testConstruction();
    void testHasTenDigitButtons();
    void testHasZeroButton();
    void testSetPin();
    void testEnteredPinInitiallyEmpty();
    void testIsValidWithMatchingPin();
    void testIsValidWithWrongPin();
    void testKeyboardDigitInput();
    void testKeyboardZeroInput();
    void testKeyboardEnterSubmits();
    void testKeyboardBackspace();
    void testKeyboardEscapeDismisses();
    void testKeyboardCClears();
    void testMaxSixDigits();
    void testButtonClickSubmitsDigit();
    void testClearButtonClears();
    void testEnterButtonSubmits();
    void testKeyboardEnterWhenButtonHasFocus();
    void testKeyboardReturnWhenButtonHasFocus();
    void testKeyboardNumericEnterWhenButtonHasFocus();
    void testBackspaceWhenButtonHasFocus();
    void testEscapeWhenButtonHasFocus();
};

void TestPINPad::testConstruction() {
    PINPad pad;
    QVERIFY(pad.enteredPin().isEmpty());
}

void TestPINPad::testHasTenDigitButtons() {
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

void TestPINPad::testHasZeroButton() {
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

void TestPINPad::testSetPin() {
    PINPad pad;
    pad.setPin("123456");
    QVERIFY(pad.enteredPin().isEmpty());
}

void TestPINPad::testEnteredPinInitiallyEmpty() {
    PINPad pad;
    QVERIFY(pad.enteredPin().isEmpty());
}

void TestPINPad::testIsValidWithMatchingPin() {
    PINPad pad;
    pad.setPin("123456");

    QKeyEvent press1(QEvent::KeyPress, Qt::Key_1, Qt::NoModifier);
    QKeyEvent press2(QEvent::KeyPress, Qt::Key_2, Qt::NoModifier);
    QKeyEvent press3(QEvent::KeyPress, Qt::Key_3, Qt::NoModifier);
    QKeyEvent press4(QEvent::KeyPress, Qt::Key_4, Qt::NoModifier);
    QKeyEvent press5(QEvent::KeyPress, Qt::Key_5, Qt::NoModifier);
    QKeyEvent press6(QEvent::KeyPress, Qt::Key_6, Qt::NoModifier);

    pad.setFocus();
    QApplication::sendEvent(&pad, &press1);
    QApplication::sendEvent(&pad, &press2);
    QApplication::sendEvent(&pad, &press3);
    QApplication::sendEvent(&pad, &press4);
    QApplication::sendEvent(&pad, &press5);
    QApplication::sendEvent(&pad, &press6);

    QVERIFY(pad.isValid());
}

void TestPINPad::testIsValidWithWrongPin() {
    PINPad pad;
    pad.setPin("654321");

    QKeyEvent press1(QEvent::KeyPress, Qt::Key_1, Qt::NoModifier);
    QKeyEvent press2(QEvent::KeyPress, Qt::Key_2, Qt::NoModifier);
    QKeyEvent press3(QEvent::KeyPress, Qt::Key_3, Qt::NoModifier);

    pad.setFocus();
    QApplication::sendEvent(&pad, &press1);
    QApplication::sendEvent(&pad, &press2);
    QApplication::sendEvent(&pad, &press3);

    QVERIFY(!pad.isValid());
}

void TestPINPad::testKeyboardDigitInput() {
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

void TestPINPad::testKeyboardZeroInput() {
    PINPad pad;
    pad.setPin("123456");
    pad.setFocus();

    QKeyEvent press(QEvent::KeyPress, Qt::Key_0, Qt::NoModifier);
    QApplication::sendEvent(&pad, &press);

    QCOMPARE(pad.enteredPin(), "0");
}

void TestPINPad::testKeyboardEnterSubmits() {
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

    QString submittedPin;
    connect(&pad, &PINPad::pinSubmitted, [&submittedPin](const QString &pin) {
        submittedPin = pin;
    });

    QApplication::sendEvent(&pad, &enter);

    QCOMPARE(submittedPin, "123");
    QVERIFY(pad.enteredPin().isEmpty());
}

void TestPINPad::testKeyboardBackspace() {
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

void TestPINPad::testKeyboardEscapeDismisses() {
    PINPad pad;
    pad.setPin("123456");
    pad.setFocus();

    QKeyEvent press1(QEvent::KeyPress, Qt::Key_1, Qt::NoModifier);
    QKeyEvent escape(QEvent::KeyPress, Qt::Key_Escape, Qt::NoModifier);

    QApplication::sendEvent(&pad, &press1);

    bool dismissed = false;
    connect(&pad, &PINPad::pinDismissed, [&]() { dismissed = true; });

    QApplication::sendEvent(&pad, &escape);

    QVERIFY(dismissed);
}

void TestPINPad::testKeyboardCClears() {
    PINPad pad;
    pad.setPin("123456");
    pad.setFocus();

    QKeyEvent press1(QEvent::KeyPress, Qt::Key_1, Qt::NoModifier);
    QKeyEvent press2(QEvent::KeyPress, Qt::Key_2, Qt::NoModifier);
    QKeyEvent press3(QEvent::KeyPress, Qt::Key_3, Qt::NoModifier);
    QKeyEvent clear(QEvent::KeyPress, Qt::Key_C, Qt::NoModifier);

    QApplication::sendEvent(&pad, &press1);
    QApplication::sendEvent(&pad, &press2);
    QApplication::sendEvent(&pad, &press3);

    QCOMPARE(pad.enteredPin(), "123");

    QApplication::sendEvent(&pad, &clear);

    QVERIFY(pad.enteredPin().isEmpty());
}

void TestPINPad::testMaxSixDigits() {
    PINPad pad;
    pad.setPin("123456");
    pad.setFocus();

    for (int i = 0; i < 10; ++i) {
        QKeyEvent press(QEvent::KeyPress, Qt::Key_1, Qt::NoModifier);
        QApplication::sendEvent(&pad, &press);
    }

    QCOMPARE(pad.enteredPin().length(), 6);
}

void TestPINPad::testButtonClickSubmitsDigit() {
    PINPad pad;
    pad.setPin("123456");

    QList<QPushButton *> buttons = pad.findChildren<QPushButton *>();
    QPushButton *oneBtn = nullptr;
    for (QPushButton *btn : buttons) {
        if (btn->text() == "1") {
            oneBtn = btn;
            break;
        }
    }
    QVERIFY(oneBtn != nullptr);

    oneBtn->click();
    QCOMPARE(pad.enteredPin(), "1");

    oneBtn->click();
    QCOMPARE(pad.enteredPin(), "11");
}

void TestPINPad::testClearButtonClears() {
    PINPad pad;
    pad.setPin("123456");
    pad.setFocus();

    QKeyEvent press1(QEvent::KeyPress, Qt::Key_1, Qt::NoModifier);
    QKeyEvent press2(QEvent::KeyPress, Qt::Key_2, Qt::NoModifier);
    QApplication::sendEvent(&pad, &press1);
    QApplication::sendEvent(&pad, &press2);

    QCOMPARE(pad.enteredPin(), "12");

    QPushButton *clearBtn = pad.findChild<QPushButton *>("");
    for (QPushButton *btn : pad.findChildren<QPushButton *>()) {
        if (btn->text() == "C") {
            btn->click();
            break;
        }
    }

    QVERIFY(pad.enteredPin().isEmpty());
}

void TestPINPad::testEnterButtonSubmits() {
    PINPad pad;
    pad.setPin("123456");
    pad.setFocus();

    QKeyEvent press1(QEvent::KeyPress, Qt::Key_1, Qt::NoModifier);
    QKeyEvent press2(QEvent::KeyPress, Qt::Key_2, Qt::NoModifier);
    QApplication::sendEvent(&pad, &press1);
    QApplication::sendEvent(&pad, &press2);

    QString submittedPin;
    connect(&pad, &PINPad::pinSubmitted, [&submittedPin](const QString &pin) {
        submittedPin = pin;
    });

    QPushButton *enterBtn = nullptr;
    for (QPushButton *btn : pad.findChildren<QPushButton *>()) {
        if (btn->text() == "Enter") {
            enterBtn = btn;
            break;
        }
    }
    QVERIFY(enterBtn != nullptr);

    enterBtn->click();

    QCOMPARE(submittedPin, "12");
    QVERIFY(pad.enteredPin().isEmpty());
}

void TestPINPad::testKeyboardEnterWhenButtonHasFocus() {
    PINPad pad;
    pad.setPin("123456");

    QPushButton *oneBtn = nullptr;
    for (QPushButton *btn : pad.findChildren<QPushButton *>()) {
        if (btn->text() == "1") {
            oneBtn = btn;
            break;
        }
    }
    QVERIFY(oneBtn != nullptr);

    QKeyEvent press1(QEvent::KeyPress, Qt::Key_1, Qt::NoModifier);
    QApplication::sendEvent(oneBtn, &press1);
    QCOMPARE(pad.enteredPin(), "1");
}

void TestPINPad::testKeyboardReturnWhenButtonHasFocus() {
    PINPad pad;
    pad.setPin("123456");

    QPushButton *oneBtn = nullptr;
    for (QPushButton *btn : pad.findChildren<QPushButton *>()) {
        if (btn->text() == "1") {
            oneBtn = btn;
            break;
        }
    }
    QVERIFY(oneBtn != nullptr);

    QKeyEvent press1(QEvent::KeyPress, Qt::Key_1, Qt::NoModifier);
    QApplication::sendEvent(&pad, &press1);
    QCOMPARE(pad.enteredPin(), "1");

    QString submittedPin;
    connect(&pad, &PINPad::pinSubmitted, [&submittedPin](const QString &pin) {
        submittedPin = pin;
    });

    QKeyEvent enterEvent(QEvent::KeyPress, Qt::Key_Return, Qt::NoModifier);
    QApplication::sendEvent(oneBtn, &enterEvent);
    QCOMPARE(submittedPin, "1");
}

void TestPINPad::testKeyboardNumericEnterWhenButtonHasFocus() {
    PINPad pad;
    pad.setPin("123456");

    QPushButton *oneBtn = nullptr;
    for (QPushButton *btn : pad.findChildren<QPushButton *>()) {
        if (btn->text() == "1") {
            oneBtn = btn;
            break;
        }
    }
    QVERIFY(oneBtn != nullptr);

    QKeyEvent press1(QEvent::KeyPress, Qt::Key_1, Qt::NoModifier);
    QApplication::sendEvent(&pad, &press1);
    QCOMPARE(pad.enteredPin(), "1");

    QString submittedPin;
    connect(&pad, &PINPad::pinSubmitted, [&submittedPin](const QString &pin) {
        submittedPin = pin;
    });

    QKeyEvent enterEvent(QEvent::KeyPress, Qt::Key_Enter, Qt::NoModifier);
    QApplication::sendEvent(oneBtn, &enterEvent);
    QCOMPARE(submittedPin, "1");
}

void TestPINPad::testBackspaceWhenButtonHasFocus() {
    PINPad pad;
    pad.setPin("123456");

    QPushButton *oneBtn = nullptr;
    for (QPushButton *btn : pad.findChildren<QPushButton *>()) {
        if (btn->text() == "1") {
            oneBtn = btn;
            break;
        }
    }
    QVERIFY(oneBtn != nullptr);

    QKeyEvent press1(QEvent::KeyPress, Qt::Key_1, Qt::NoModifier);
    QKeyEvent press2(QEvent::KeyPress, Qt::Key_2, Qt::NoModifier);
    QApplication::sendEvent(&pad, &press1);
    QApplication::sendEvent(&pad, &press2);
    QCOMPARE(pad.enteredPin(), "12");

    QKeyEvent backspace(QEvent::KeyPress, Qt::Key_Backspace, Qt::NoModifier);
    QApplication::sendEvent(oneBtn, &backspace);
    QCOMPARE(pad.enteredPin(), "1");
}

void TestPINPad::testEscapeWhenButtonHasFocus() {
    PINPad pad;
    pad.setPin("123456");

    QPushButton *oneBtn = nullptr;
    for (QPushButton *btn : pad.findChildren<QPushButton *>()) {
        if (btn->text() == "1") {
            oneBtn = btn;
            break;
        }
    }
    QVERIFY(oneBtn != nullptr);

    bool dismissed = false;
    connect(&pad, &PINPad::pinDismissed, [&dismissed]() {
        dismissed = true;
    });

    QKeyEvent escape(QEvent::KeyPress, Qt::Key_Escape, Qt::NoModifier);
    QApplication::sendEvent(oneBtn, &escape);
    QVERIFY(dismissed);
}

QTEST_MAIN(TestPINPad)
#include "test_pinpad.moc"
