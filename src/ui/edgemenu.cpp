#include "edgemenu.h"

EdgeMenu::EdgeMenu(QWidget *parent)
    : QWidget(parent),
      settingsButton_(new QPushButton("Settings")),
      fullscreenButton_(new QPushButton("Fullscreen")),
      exitButton_(new QPushButton("Exit")) {
    setWindowFlags(Qt::FramelessWindowHint | Qt::Tool);
    setAttribute(Qt::WA_TranslucentBackground);
    setStyleSheet("background-color: rgba(26, 26, 26, 200);");

    settingsButton_->setStyleSheet("padding: 10px; color: #e0e0e0;");
    fullscreenButton_->setStyleSheet("padding: 10px; color: #e0e0e0;");
    exitButton_->setStyleSheet("padding: 10px; color: #e0e0e0;");

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(settingsButton_);
    layout->addWidget(fullscreenButton_);
    layout->addWidget(exitButton_);
    setLayout(layout);

    connect(settingsButton_, &QPushButton::clicked, this, &EdgeMenu::settingsClicked);
    connect(fullscreenButton_, &QPushButton::clicked, this, &EdgeMenu::fullscreenClicked);
    connect(exitButton_, &QPushButton::clicked, this, &EdgeMenu::exitClicked);

    hideTimer_.setInterval(3000);
    hideTimer_.setSingleShot(true);
    connect(&hideTimer_, &QTimer::timeout, this, &EdgeMenu::hideMenu);
}

void EdgeMenu::showMenu() {
    show();
    hideTimer_.start();
}

void EdgeMenu::hideMenu() {
    hideTimer_.stop();
    hide();
}

bool EdgeMenu::isVisible() const {
    return QWidget::isVisible();
}
