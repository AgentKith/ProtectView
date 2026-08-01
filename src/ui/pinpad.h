#pragma once

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QString>
#include <QVector>
#include <QPixmap>
#include <QColor>

class PINPad : public QWidget {
    Q_OBJECT
public:
    explicit PINPad(QWidget *parent = nullptr);
    void setPin(const QString &pin);
    bool isValid() const;
    QString enteredPin() const;

signals:
    void pinSubmitted(const QString &pin);
    void pinDismissed();

private slots:
    void onDigitPressed();
    void onClearPressed();
    void onEnterPressed();

 private:
    void keyPressEvent(QKeyEvent *event) override;
    bool eventFilter(QObject *obj, QEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void setupUI();
    void shuffleButtons();
    void recalculateButtonSizes();
    void appendDigit(int digit);
    void handleKeyEvent(QKeyEvent *event);
    QPixmap renderSvgIcon(const QString &resourcePath, int size, const QColor &color);
    void updateButtonIcon(QPushButton *button, const QString &resourcePath, int size);

    QString expectedPin_;
    QString enteredPin_;
    QLabel *display_;
    QVector<QPushButton *> digitButtons_;
    QPushButton *clearButton_;
    QPushButton *enterButton_;
};
