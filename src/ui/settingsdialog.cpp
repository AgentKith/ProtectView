#include "settingsdialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QGraphicsDropShadowEffect>
#include <QShowEvent>
#include <QHideEvent>

SettingsDialog::SettingsDialog(QWidget *parent)
    : QDialog(parent),
      hostEdit_(new QLineEdit),
      apiKeyEdit_(new QLineEdit),
      ffmpegPathEdit_(new QLineEdit),
      qualityCombo_(new QComboBox),
      skipTlsCheck_(new QCheckBox),
      retryIntervalEdit_(new QSpinBox),
    saveButton_(new QPushButton("Save")),
       cancelButton_(new QPushButton("Cancel")) {
    setObjectName("SettingsDialog");
    setWindowTitle("Settings");

    {
        QGraphicsDropShadowEffect *glow = new QGraphicsDropShadowEffect(this);
        glow->setColor(QColor(255, 107, 0));
        glow->setBlurRadius(40);
        glow->setOffset(0, 0);
        setGraphicsEffect(glow);
    }

    apiKeyEdit_->setEchoMode(QLineEdit::Password);
    qualityCombo_->addItems({"Low", "Medium", "High"});
    qualityCombo_->setCurrentIndex(1);
    retryIntervalEdit_->setRange(1, 60);
    retryIntervalEdit_->setValue(5);
    retryIntervalEdit_->setSuffix("s");

    saveButton_->setObjectName("saveButton");
    cancelButton_->setObjectName("cancelButton");

    QFormLayout *form = new QFormLayout;
    form->addRow("UNVR Host:", hostEdit_);
    form->addRow("API Key:", apiKeyEdit_);
    form->addRow("FFmpeg Path:", ffmpegPathEdit_);
    form->addRow("Video Quality:", qualityCombo_);
    form->addRow("Skip TLS Verify:", skipTlsCheck_);
    form->addRow("Retry Interval:", retryIntervalEdit_);

    QHBoxLayout *buttons = new QHBoxLayout;
    buttons->addWidget(cancelButton_);
    buttons->addWidget(saveButton_);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addLayout(form);
    mainLayout->addLayout(buttons);
    setLayout(mainLayout);

    connect(saveButton_, &QPushButton::clicked, this, [this]() {
        emit settingsChanged();
        accept();
    });
    connect(cancelButton_, &QPushButton::clicked, this, &QDialog::reject);
}

void SettingsDialog::showEvent(QShowEvent *event) {
    if (parentWidget()) parentWidget()->clearFocus();
    QDialog::showEvent(event);
}

void SettingsDialog::hideEvent(QHideEvent *event) {
    clearFocus();
    QDialog::hideEvent(event);
}

void SettingsDialog::loadFrom(const QString &host, const QString &apiKey, const QString &ffmpegPath,
                              int quality, bool skipTls, int retryInterval) {
    hostEdit_->setText(host);
    apiKeyEdit_->setText(apiKey);
    ffmpegPathEdit_->setText(ffmpegPath);
    qualityCombo_->setCurrentIndex(quality);
    skipTlsCheck_->setChecked(skipTls);
    retryIntervalEdit_->setValue(retryInterval);
}

QString SettingsDialog::unvrHost() const {
    return hostEdit_->text();
}

QString SettingsDialog::apiKey() const {
    return apiKeyEdit_->text();
}

QString SettingsDialog::ffmpegPath() const {
    return ffmpegPathEdit_->text();
}

int SettingsDialog::videoQuality() const {
    return qualityCombo_->currentIndex();
}

bool SettingsDialog::skipTlsVerify() const {
    return skipTlsCheck_->isChecked();
}

int SettingsDialog::retryInterval() const {
    return retryIntervalEdit_->value();
}
