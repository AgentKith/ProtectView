#include "pinpad.h"
#include <QVBoxLayout>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QRandomGenerator>
#include <QRegularExpression>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QResizeEvent>
#include <QGraphicsDropShadowEffect>
#include <QSvgRenderer>
#include <QPainter>
#include <QIcon>
#include <QPixmap>
#include <QFile>
#include <QByteArray>

PINPad::PINPad(QWidget *parent)
    : QWidget(parent),
      display_(new QLabel("")),
      clearButton_(new QPushButton("")),
       enterButton_(new QPushButton("")) {
    setupUI();
    shuffleButtons();
}

void PINPad::setupUI() {
    display_->setAlignment(Qt::AlignCenter);
    display_->setMinimumHeight(72);
    display_->setObjectName("pinDisplay");

    for (int i = 1; i <= 9; ++i) {
        QPushButton *btn = new QPushButton(QString::number(i));
        btn->setObjectName("pinDigit");
        connect(btn, &QPushButton::clicked, this, &PINPad::onDigitPressed);
        {
            QGraphicsDropShadowEffect *glow = new QGraphicsDropShadowEffect(btn);
            glow->setColor(QColor(255, 107, 0));
            glow->setBlurRadius(20);
            glow->setOffset(0, 0);
            btn->setGraphicsEffect(glow);
        }
        digitButtons_.append(btn);
    }

    QPushButton *zeroBtn = new QPushButton("0");
    zeroBtn->setObjectName("pinDigit");
    connect(zeroBtn, &QPushButton::clicked, this, &PINPad::onDigitPressed);
    {
        QGraphicsDropShadowEffect *glow = new QGraphicsDropShadowEffect(zeroBtn);
        glow->setColor(QColor(255, 107, 0));
        glow->setBlurRadius(20);
        glow->setOffset(0, 0);
        zeroBtn->setGraphicsEffect(glow);
    }
    digitButtons_.append(zeroBtn);

    clearButton_->setObjectName("pinClear");
    clearButton_->setToolTip("Clear");
    {
        QGraphicsDropShadowEffect *glow = new QGraphicsDropShadowEffect(clearButton_);
        glow->setColor(QColor(255, 107, 0));
        glow->setBlurRadius(20);
        glow->setOffset(0, 0);
        clearButton_->setGraphicsEffect(glow);
    }
    enterButton_->setObjectName("pinEnter");
    enterButton_->setToolTip("Enter");
    {
        QGraphicsDropShadowEffect *glow = new QGraphicsDropShadowEffect(enterButton_);
        glow->setColor(QColor(255, 107, 0));
        glow->setBlurRadius(20);
        glow->setOffset(0, 0);
        enterButton_->setGraphicsEffect(glow);
    }

    connect(clearButton_, &QPushButton::clicked, this, &PINPad::onClearPressed);
    connect(enterButton_, &QPushButton::clicked, this, &PINPad::onEnterPressed);

    installEventFilter(this);
    for (QPushButton *btn : digitButtons_) btn->installEventFilter(this);
    clearButton_->installEventFilter(this);
    enterButton_->installEventFilter(this);

    QGridLayout *grid = new QGridLayout;
    grid->setSpacing(24);
    for (int i = 0; i < 9; ++i) {
        int row = i / 3;
        int col = i % 3;
        grid->addWidget(digitButtons_[i], row, col);
    }

    QHBoxLayout *bottomRow = new QHBoxLayout;
    bottomRow->setSpacing(24);
    bottomRow->addWidget(clearButton_);
    bottomRow->addWidget(digitButtons_[9]);
    bottomRow->addWidget(enterButton_);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->setSpacing(24);
    mainLayout->setContentsMargins(48, 48, 48, 48);
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

void PINPad::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);
    recalculateButtonSizes();
}

void PINPad::recalculateButtonSizes() {
    QLayout *mainLayout = layout();
    if (!mainLayout) return;

    int margin = mainLayout->contentsMargins().left();
    int spacing = mainLayout->spacing();

    int availW = width() - 2 * margin;
    int availH = height() - 2 * margin - display_->sizeHint().height() - spacing;

    int buttonW = (availW - 2 * spacing) / 3;
    int buttonH = (availH - 3 * spacing) / 4;
    int buttonSize = qBound(40, qMin(buttonW, buttonH), 500);

    for (QPushButton *btn : digitButtons_) {
        btn->setFixedSize(buttonSize, buttonSize);
    }
    clearButton_->setFixedSize(buttonSize, buttonSize);
    enterButton_->setFixedSize(buttonSize, buttonSize);

    int iconSize = qBound(24, buttonSize / 2, 200);
    updateButtonIcon(clearButton_, QString(":/clear.svg"), iconSize);
    updateButtonIcon(enterButton_, QString(":/enter.svg"), iconSize);
}

QPixmap PINPad::renderSvgIcon(const QString &resourcePath, int size, const QColor &color) {
    QFile svgFile(resourcePath);
    svgFile.open(QIODevice::ReadOnly | QIODevice::Text);
    QByteArray content = svgFile.readAll();
    svgFile.close();

    QString colorHex = QString("#%1").arg(color.name().mid(1));
    content.replace("currentColor", colorHex.toUtf8());

    QSvgRenderer renderer(content);
    QPixmap pixmap(size, size);
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing);
    renderer.render(&painter);
    painter.end();
    return pixmap;
}

void PINPad::updateButtonIcon(QPushButton *button, const QString &resourcePath, int size) {
    QPixmap pixmap = renderSvgIcon(resourcePath, size, QColor(255, 107, 0));
    button->setIcon(QIcon(pixmap));
    button->setIconSize(QSize(size, size));
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
            display_->setText(QString("\u25cf").repeated(enteredPin_.length()));
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
    display_->setText(QString("\u25cf").repeated(enteredPin_.length()));
}
