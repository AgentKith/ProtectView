#pragma once

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QString>
#include <QVector>

class PINPad : public QWidget {
    Q_OBJECT
public:
    explicit PINPad(QWidget *parent = nullptr);
    void setPin(const QString &pin);
    bool isValid() const;
    QString enteredPin() const;

signals:
    void pinAccepted();
    void pinRejected();

private slots:
    void onDigitPressed();
    void onClearPressed();
    void onEnterPressed();

private:
    void setupUI();
    void shuffleButtons();

    QString expectedPin_;
    QString enteredPin_;
    QLabel *display_;
    QVector<QPushButton *> digitButtons_;
    QPushButton *clearButton_;
    QPushButton *enterButton_;
};
