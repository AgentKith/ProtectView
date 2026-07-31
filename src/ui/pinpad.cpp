#include "pinpad.h"
#include <QVBoxLayout>
#include <QGridLayout>
#include <QRandomGenerator>
#include <QRegularExpression>

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

    QGridLayout *grid = new QGridLayout;
    int row = 0, col = 0;
    for (int i = 0; i < digitButtons_.size(); ++i) {
        grid->addWidget(digitButtons_[i], row, col);
        col++;
        if (col > 2) {
            col = 0;
            row++;
        }
    }
    grid->addWidget(clearButton_, row, 0);
    grid->addWidget(enterButton_, row, 2);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(display_);
    mainLayout->addLayout(grid);
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
        enteredPin_.append(btn->text());
        display_->setText(QString("●").repeated(enteredPin_.length()));
    }
}

void PINPad::onClearPressed() {
    enteredPin_.clear();
    display_->setText("");
}

void PINPad::onEnterPressed() {
    if (enteredPin_ == expectedPin_) {
        emit pinAccepted();
    } else {
        emit pinRejected();
        enteredPin_.clear();
        display_->setText("");
    }
}
