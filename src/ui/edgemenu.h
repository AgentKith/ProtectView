#pragma once

#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QTimer>

class EdgeMenu : public QWidget {
    Q_OBJECT
public:
    explicit EdgeMenu(QWidget *parent = nullptr);
    void showMenu();
    void hideMenu();
    bool isVisible() const;

signals:
    void settingsClicked();
    void fullscreenClicked();
    void exitClicked();

private:
    QPushButton *settingsButton_;
    QPushButton *fullscreenButton_;
    QPushButton *exitButton_;
    QTimer hideTimer_;
};
