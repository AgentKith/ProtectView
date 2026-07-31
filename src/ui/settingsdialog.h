#pragma once

#include <QDialog>
#include <QLineEdit>
#include <QComboBox>
#include <QSpinBox>
#include <QCheckBox>
#include <QPushButton>
#include <QString>

class SettingsDialog : public QDialog {
    Q_OBJECT
public:
    explicit SettingsDialog(QWidget *parent = nullptr);
    QString unvrHost() const;
    QString apiKey() const;
    QString ffmpegPath() const;
    int videoQuality() const;
    bool skipTlsVerify() const;
    int retryInterval() const;

signals:
    void settingsChanged();

private:
    QLineEdit *hostEdit_;
    QLineEdit *apiKeyEdit_;
    QLineEdit *ffmpegPathEdit_;
    QComboBox *qualityCombo_;
    QCheckBox *skipTlsCheck_;
    QSpinBox *retryIntervalEdit_;
    QPushButton *saveButton_;
    QPushButton *cancelButton_;
};
