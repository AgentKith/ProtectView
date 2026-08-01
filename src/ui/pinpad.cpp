#include "pinpad.h"
#include <QVBoxLayout>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QRandomGenerator>
#include <QRegularExpression>
#include <QKeyEvent>

PINPad::PINPad(QWidget *parent)
    : QWidget(parent),
      display_(new QLabel("")),
      clearButton_(new QPushButton("C")),
      enterButton_(new QPushButton("Enter")) {
    setupUI();
    shuffleButtons();
}

void PINPad::setupUI() {
    display_->setAlignment(Qt::AlignCenter);
    display_->setMinimumHeight(50);
    display_->setStyleSheet("font-size: 24px; color: #e0e0e0;");

    for (int i = 1; i <= 9; ++i) {
        QPushButton *btn = new QPushButton(QString::number(i));
        btn->setMinimumSize(80, 80);
        btn->setStyleSheet("font-size: 20px; padding: 10px;");
        connect(btn, &QPushButton::clicked, this, &PINPad::onDigitPressed);
        digitButtons_.append(btn);
    }

    QPushButton *zeroBtn = new QPushButton("0");
    zeroBtn->setMinimumSize(80, 80);
    zeroBtn->setStyleSheet("font-size: 20px; padding: 10px;");
    connect(zeroBtn, &QPushButton::clicked, this, &PINPad::onDigitPressed);
    digitButtons_.append(zeroBtn);

    clearButton_->setMinimumSize(80, 80);
    clearButton_->setStyleSheet("font-size: 20px; padding: 10px;");
    enterButton_->setMinimumSize(80, 80);
    enterButton_->setStyleSheet("font-size: 20px; padding: 10px;");

    connect(clearButton_, &QPushButton::clicked, this, &PINPad::onClearPressed);
    connect(enterButton_, &QPushButton::clicked, this, &PINPad::onEnterPressed);

    installEventFilter(this);
    for (QPushButton *btn : digitButtons_) btn->installEventFilter(this);
    clearButton_->installEventFilter(this);
    enterButton_->installEventFilter(this);

    QGridLayout *grid = new QGridLayout;
    for (int i = 0; i < 9; ++i) {
        int row = i / 3;
        int col = i % 3;
        grid->addWidget(digitButtons_[i], row, col);
    }

    QHBoxLayout *bottomRow = new QHBoxLayout;
    bottomRow->addWidget(clearButton_);
    bottomRow->addWidget(digitButtons_[9]);
    bottomRow->addWidget(enterButton_);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(display_);
    mainLayout->addLayout(grid);
    mainLayout->addLayout(bottomRow);
    setLayout(mainLayout);
}

void PINPad::shuffleButtons() {
    QRandomGenerator *rng = QRandomGenerator::system();
    for (int i = digitButtons_.size() - 1; i > 0; --i) {
        int j = rng->bounded(i + 1);
        std::swap(digitButtons_[i], digitButtons_[j]);
    }
}

void PINPad::setPin(const QString &pin) {
    expectedPin_ = pin;
}

bool PINPad::isValid() const {
    return enteredPin_ == expectedPin_;
}

QString PINPad::enteredPin() const {
    return enteredPin_;
}

void PINPad::onDigitPressed() {
    QPushButton *btn = qobject_cast<QPushButton *>(sender());
    if (btn) {
        appendDigit(btn->text().toInt());
    }
}

void PINPad::onClearPressed() {
    enteredPin_.clear();
    display_->setText("");
}

void PINPad::onEnterPressed() {
    emit pinSubmitted(enteredPin_);
    enteredPin_.clear();
    display_->setText("");
}

bool PINPad::eventFilter(QObject *obj, QEvent *event) {
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        handleKeyEvent(keyEvent);
        return true;
    }
    return QWidget::eventFilter(obj, event);
}

void PINPad::keyPressEvent(QKeyEvent *event) {
    handleKeyEvent(event);
}

void PINPad::handleKeyEvent(QKeyEvent *event) {
    if (event->key() >= Qt::Key_0 && event->key() <= Qt::Key_9) {
        int digit = event->key() - Qt::Key_0;
        appendDigit(digit);
    } else if (event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return) {
        onEnterPressed();
    } else if (event->key() == Qt::Key_Backspace || event->key() == Qt::Key_Delete) {
        if (!enteredPin_.isEmpty()) {
            enteredPin_.chop(1);
            display_->setText(QString("●").repeated(enteredPin_.length()));
        }
    } else if (event->key() == Qt::Key_Escape) {
        emit pinDismissed();
    } else if (event->key() == Qt::Key_C) {
        onClearPressed();
    } else {
        QWidget::keyPressEvent(event);
    }
}

void PINPad::appendDigit(int digit) {
    if (enteredPin_.length() >= 6) return;
    enteredPin_.append(QString::number(digit));
    display_->setText(QString("●").repeated(enteredPin_.length()));
}
