#include "edgemenu.h"
#include <QGraphicsDropShadowEffect>

EdgeMenu::EdgeMenu(QWidget *parent)
    : QWidget(parent),
      settingsButton_(new QPushButton("Settings")),
      exitButton_(new QPushButton("Exit")) {
    setObjectName("EdgeMenu");
    setWindowFlags(Qt::FramelessWindowHint | Qt::Tool);
    setAttribute(Qt::WA_TranslucentBackground);

    settingsButton_->setObjectName("edgeButton");
    exitButton_->setObjectName("edgeButton");

    QWidget *buttonPanel = new QWidget;
    buttonPanel->setObjectName("edgeButtonPanel");
    {
        QGraphicsDropShadowEffect *panelGlow = new QGraphicsDropShadowEffect(buttonPanel);
        panelGlow->setColor(QColor(255, 107, 0));
        panelGlow->setBlurRadius(30);
        panelGlow->setOffset(0, 0);
        buttonPanel->setGraphicsEffect(panelGlow);
    }

    {
        QGraphicsDropShadowEffect *glow = new QGraphicsDropShadowEffect(settingsButton_);
        glow->setColor(QColor(0, 212, 255));
        glow->setBlurRadius(20);
        glow->setOffset(0, 0);
        settingsButton_->setGraphicsEffect(glow);
    }
    {
        QGraphicsDropShadowEffect *glow = new QGraphicsDropShadowEffect(exitButton_);
        glow->setColor(QColor(0, 212, 255));
        glow->setBlurRadius(20);
        glow->setOffset(0, 0);
        exitButton_->setGraphicsEffect(glow);
    }

    QVBoxLayout *buttonLayout = new QVBoxLayout(buttonPanel);
    buttonLayout->setSpacing(48);
    buttonLayout->addWidget(settingsButton_);
    buttonLayout->addWidget(exitButton_);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(buttonPanel);
    setLayout(layout);

    connect(settingsButton_, &QPushButton::clicked, this, &EdgeMenu::settingsClicked);
    connect(exitButton_, &QPushButton::clicked, this, &EdgeMenu::exitClicked);

    hideTimer_.setInterval(3000);
    hideTimer_.setSingleShot(true);
    connect(&hideTimer_, &QTimer::timeout, this, &EdgeMenu::hideMenu);
}

void EdgeMenu::setKioskMode(bool kiosk) {
    exitButton_->setVisible(!kiosk);
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
