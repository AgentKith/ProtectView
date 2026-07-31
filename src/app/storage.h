#pragma once

#include <QString>
#include <QByteArray>

class Storage {
public:
    explicit Storage(const QString &configDir, const QString &fingerprint);
    ~Storage();

    QByteArray encrypt(const QByteArray &plaintext) const;
    QByteArray decrypt(const QByteArray &ciphertext) const;
    bool save(const QString &filename, const QByteArray &data) const;
    QByteArray load(const QString &filename) const;
    QString configDir() const { return configDir_; }
    bool hasSalt() const;
    QByteArray getSalt() const;

private:
    QString configDir_;
    QByteArray key_;
    mutable QByteArray salt_;

    void deriveKey(const QString &fingerprint);
    void loadSalt();
    void saveSalt();
};
