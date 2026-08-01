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
    void loadFrom(const QString &host, const QString &apiKey, const QString &ffmpegPath,
                  int quality, bool skipTls, int retryInterval);
    QString unvrHost() const;
    QString apiKey() const;
    QString ffmpegPath() const;
    int videoQuality() const;
    bool skipTlsVerify() const;
    int retryInterval() const;

signals:
    void settingsChanged();

protected:
    void showEvent(QShowEvent *event) override;
    void hideEvent(QHideEvent *event) override;

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
