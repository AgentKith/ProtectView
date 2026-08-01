#include <QtTest>
#include <QApplication>
#include <QKeyEvent>
#include <QPushButton>
#include <QDialog>
#include <QVBoxLayout>
#include <QFontMetrics>
#include <QLabel>
#include <QFile>
#include <QIODevice>
#include <QColor>
#include <QPixmap>
#include <QImage>
#include "ui/pinpad.h"

class TestPINPad : public QObject {
    Q_OBJECT
private slots:
    void initTestCase() {
        Q_INIT_RESOURCE(styles);
        Q_INIT_RESOURCE(icons);
    }

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
    void testLayoutFitsInDialog();
    void testButtonCount();
    void testIconResourcesExist();
    void testActionButtonsHaveIcons();
    void testIconRendersAsPixmap();
    void testIconSizeScalesWithButton();
    void testIconColorIsOrange();
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

    QPushButton *clearBtn = nullptr;
    for (QPushButton *btn : pad.findChildren<QPushButton *>()) {
        if (btn->objectName() == "pinClear") {
            clearBtn = btn;
            break;
        }
    }
    QVERIFY(clearBtn != nullptr);
    clearBtn->click();

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
        if (btn->objectName() == "pinEnter") {
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

void TestPINPad::testButtonCount() {
    PINPad pad;

    QList<QPushButton *> allBtns = pad.findChildren<QPushButton *>();
    QCOMPARE(allBtns.size(), 12);

    int digitCount = 0;
    int actionCount = 0;
    for (QPushButton *btn : allBtns) {
        if (btn->objectName() == "pinClear" || btn->objectName() == "pinEnter") {
            actionCount++;
        } else {
            digitCount++;
        }
    }
    QCOMPARE(digitCount, 10);
    QCOMPARE(actionCount, 2);
}

void TestPINPad::testLayoutFitsInDialog() {
    struct TestCase { int w; int h; };
    QList<TestCase> cases = {
        {400, 500}, {640, 800}, {800, 1000}, {1000, 600}, {500, 500}
    };

    for (const TestCase &tc : cases) {
        QDialog dialog;
        dialog.setFixedSize(tc.w, tc.h);

        PINPad pad;
        QVBoxLayout *layout = new QVBoxLayout(&dialog);
        layout->setContentsMargins(0, 0, 0, 0);
        layout->addWidget(&pad);
        dialog.setLayout(layout);
        dialog.show();
        QApplication::processEvents();

        QPushButton *btns[10] = {};
        int found = 0;
        for (QPushButton *btn : pad.findChildren<QPushButton *>()) {
            for (int d = 0; d < 10; ++d) {
                if (btn->text() == QString::number(d) && !btns[d]) {
                    btns[d] = btn;
                    found++;
                    break;
                }
            }
        }
        QCOMPARE(found, 10);

        QPushButton *clearBtn = nullptr;
        QPushButton *enterBtn = nullptr;
        for (QPushButton *btn : pad.findChildren<QPushButton *>()) {
            if (btn->objectName() == "pinClear") clearBtn = btn;
            if (btn->objectName() == "pinEnter") enterBtn = btn;
        }
        QVERIFY(clearBtn != nullptr);
        QVERIFY(enterBtn != nullptr);

        // Uniformity: all digit buttons same size
        for (int i = 1; i < 10; ++i) {
            QCOMPARE(btns[i]->width(), btns[0]->width());
            QCOMPARE(btns[i]->height(), btns[0]->height());
        }
        QCOMPARE(clearBtn->width(), btns[0]->width());
        QCOMPARE(clearBtn->height(), btns[0]->height());
        QCOMPARE(enterBtn->width(), btns[0]->width());
        QCOMPARE(enterBtn->height(), btns[0]->height());

        // Horizontal: buttons 1, 2, 3 must not overlap
        // Find positions in global coords
        QPoint p1 = btns[1]->mapTo(&dialog, QPoint(0, 0));
        QPoint p2 = btns[2]->mapTo(&dialog, QPoint(0, 0));
        QPoint p3 = btns[3]->mapTo(&dialog, QPoint(0, 0));
        int bw = btns[1]->width();
        QVERIFY(p2.x() >= p1.x());
        QVERIFY(p3.x() >= p2.x());
        QVERIFY(p1.x() + bw <= tc.w);
        QVERIFY(p2.x() + bw <= tc.w);
        QVERIFY(p3.x() + bw <= tc.w);

        // Vertical: display + buttons 2, 5, 8, 0 must not overlap
        QPoint d2 = btns[2]->mapTo(&dialog, QPoint(0, 0));
        QPoint d5 = btns[5]->mapTo(&dialog, QPoint(0, 0));
        QPoint d8 = btns[8]->mapTo(&dialog, QPoint(0, 0));
        QPoint d0 = btns[0]->mapTo(&dialog, QPoint(0, 0));
        int bh = btns[2]->height();
        QVERIFY(d5.y() >= d2.y());
        QVERIFY(d8.y() >= d5.y());
        QVERIFY(d0.y() >= d8.y());
        QVERIFY(d0.y() + bh <= tc.h);

        // Font fitting: digit text must fit within button
        QFontMetrics fm(btns[1]->font());
        QRect br = fm.boundingRect("1");
        QVERIFY(br.width() < btns[1]->width());
        QVERIFY(br.height() < btns[1]->height());

        // Font fitting: action button text must fit
        QFontMetrics fmClear(clearBtn->font());
        QRect brClear = fmClear.boundingRect("Clear");
        QVERIFY(brClear.width() < clearBtn->width());

        QFontMetrics fmEnter(enterBtn->font());
        QRect brEnter = fmEnter.boundingRect("Enter");
        QVERIFY(brEnter.width() < enterBtn->width());
    }
}

void TestPINPad::testIconResourcesExist() {
    QFile clearIcon(":/clear.svg");
    QVERIFY(clearIcon.exists());
    clearIcon.open(QIODevice::ReadOnly);
    QByteArray clearData = clearIcon.readAll();
    QVERIFY(!clearData.isEmpty());
    clearIcon.close();

    QFile enterIcon(":/enter.svg");
    QVERIFY(enterIcon.exists());
    enterIcon.open(QIODevice::ReadOnly);
    QByteArray enterData = enterIcon.readAll();
    QVERIFY(!enterData.isEmpty());
    enterIcon.close();
}

void TestPINPad::testActionButtonsHaveIcons() {
    QDialog dialog;
    dialog.setFixedSize(448, 800);

    PINPad pad;
    QVBoxLayout *layout = new QVBoxLayout(&dialog);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(&pad);
    dialog.setLayout(layout);
    dialog.show();
    QApplication::processEvents();

    QPushButton *clearBtn = nullptr;
    QPushButton *enterBtn = nullptr;
    for (QPushButton *btn : pad.findChildren<QPushButton *>()) {
        if (btn->objectName() == "pinClear") clearBtn = btn;
        if (btn->objectName() == "pinEnter") enterBtn = btn;
    }
    QVERIFY(clearBtn != nullptr);
    QVERIFY(enterBtn != nullptr);

    QVERIFY(!clearBtn->icon().isNull());
    QVERIFY(!enterBtn->icon().isNull());

    int buttonSize = clearBtn->width();
    int expectedIconSize = qBound(24, buttonSize / 2, 200);
    QCOMPARE(clearBtn->iconSize().width(), expectedIconSize);
    QCOMPARE(clearBtn->iconSize().height(), expectedIconSize);
    QCOMPARE(enterBtn->iconSize().width(), expectedIconSize);
    QCOMPARE(enterBtn->iconSize().height(), expectedIconSize);
}

void TestPINPad::testIconRendersAsPixmap() {
    QDialog dialog;
    dialog.setFixedSize(448, 800);

    PINPad pad;
    QVBoxLayout *layout = new QVBoxLayout(&dialog);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(&pad);
    dialog.setLayout(layout);
    dialog.show();
    QApplication::processEvents();

    QPushButton *clearBtn = nullptr;
    QPushButton *enterBtn = nullptr;
    for (QPushButton *btn : pad.findChildren<QPushButton *>()) {
        if (btn->objectName() == "pinClear") clearBtn = btn;
        if (btn->objectName() == "pinEnter") enterBtn = btn;
    }
    QVERIFY(clearBtn != nullptr);
    QVERIFY(enterBtn != nullptr);

    int iconSize = clearBtn->iconSize().width();
    QPixmap clearPixmap = clearBtn->icon().pixmap(iconSize, iconSize);
    QVERIFY(!clearPixmap.isNull());
    QCOMPARE(clearPixmap.width(), iconSize);
    QCOMPARE(clearPixmap.height(), iconSize);

    {
        QImage img = clearPixmap.toImage();
        int visiblePixels = 0;
        for (int y = 0; y < img.height(); ++y) {
            for (int x = 0; x < img.width(); ++x) {
                if (img.pixelColor(x, y).alpha() >= 10) visiblePixels++;
            }
        }
        QVERIFY(visiblePixels > iconSize * iconSize / 100);
    }

    QPixmap enterPixmap = enterBtn->icon().pixmap(iconSize, iconSize);
    QVERIFY(!enterPixmap.isNull());
    QCOMPARE(enterPixmap.width(), iconSize);
    QCOMPARE(enterPixmap.height(), iconSize);

    {
        QImage img = enterPixmap.toImage();
        int visiblePixels = 0;
        for (int y = 0; y < img.height(); ++y) {
            for (int x = 0; x < img.width(); ++x) {
                if (img.pixelColor(x, y).alpha() >= 10) visiblePixels++;
            }
        }
        QVERIFY(visiblePixels > iconSize * iconSize / 100);
    }
}

void TestPINPad::testIconSizeScalesWithButton() {
    struct TestCase { int w; int h; };
    QList<TestCase> cases = {
        {300, 400}, {448, 800}, {800, 600}
    };

    int prevIconSize = -1;
    for (const TestCase &tc : cases) {
        QDialog dialog;
        dialog.setFixedSize(tc.w, tc.h);

        PINPad pad;
        QVBoxLayout *layout = new QVBoxLayout(&dialog);
        layout->setContentsMargins(0, 0, 0, 0);
        layout->addWidget(&pad);
        dialog.setLayout(layout);
        dialog.show();
        QApplication::processEvents();

        QPushButton *clearBtn = nullptr;
        for (QPushButton *btn : pad.findChildren<QPushButton *>()) {
            if (btn->objectName() == "pinClear") { clearBtn = btn; break; }
        }
        QVERIFY(clearBtn != nullptr);

        int buttonSize = clearBtn->width();
        int iconSize = clearBtn->iconSize().width();

        int expectedIconSize = qBound(24, buttonSize / 2, 200);
        QCOMPARE(iconSize, expectedIconSize);

        QVERIFY(iconSize <= buttonSize);
        QVERIFY(iconSize >= 24);
        QVERIFY(iconSize <= 200);

        if (prevIconSize >= 0) {
            QVERIFY(iconSize != prevIconSize || tc.w == cases[0].w);
        }
        prevIconSize = iconSize;
    }
}

void TestPINPad::testIconColorIsOrange() {
    QDialog dialog;
    dialog.setFixedSize(448, 800);

    PINPad pad;
    QVBoxLayout *layout = new QVBoxLayout(&dialog);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(&pad);
    dialog.setLayout(layout);
    dialog.show();
    QApplication::processEvents();

    QPushButton *clearBtn = nullptr;
    QPushButton *enterBtn = nullptr;
    for (QPushButton *btn : pad.findChildren<QPushButton *>()) {
        if (btn->objectName() == "pinClear") clearBtn = btn;
        if (btn->objectName() == "pinEnter") enterBtn = btn;
    }
    QVERIFY(clearBtn != nullptr);
    QVERIFY(enterBtn != nullptr);

    for (QPushButton *btn : {clearBtn, enterBtn}) {
        int iconSize = btn->iconSize().width();
        QPixmap pixmap = btn->icon().pixmap(iconSize, iconSize);
        QVERIFY(!pixmap.isNull());

        int orangePixels = 0;
        int totalNonTransparent = 0;
        QImage img = pixmap.toImage();
        for (int y = 0; y < img.height(); ++y) {
            for (int x = 0; x < img.width(); ++x) {
                QColor c = img.pixelColor(x, y);
                if (c.alpha() < 10) continue;
                totalNonTransparent++;

                int r = c.red(), g = c.green(), b = c.blue();
                if (r > 150 && g < 120 && b < 50) {
                    orangePixels++;
                }
            }
        }
        QVERIFY(totalNonTransparent > 0);
        QVERIFY(orangePixels > totalNonTransparent / 2);
    }
}

QTEST_MAIN(TestPINPad)
#include "test_pinpad.moc"
