#include "storage.h"

#include <QFile>
#include <QDir>

#include <openssl/evp.h>
#include <openssl/rand.h>

constexpr int SALT_LENGTH = 16;
constexpr int KEY_LENGTH = 32;
constexpr int IV_LENGTH = 12;
constexpr int TAG_LENGTH = 16;
constexpr int PBKDF2_ITERATIONS = 100000;

Storage::Storage(const QString &configDir, const QString &fingerprint)
    : configDir_(configDir) {
    QDir().mkpath(configDir_);
    loadSalt();
    if (salt_.isEmpty()) {
        QByteArray random(SALT_LENGTH, 0);
        RAND_bytes(reinterpret_cast<unsigned char *>(random.data()), SALT_LENGTH);
        salt_ = random;
        saveSalt();
    }
    deriveKey(fingerprint);
}

Storage::~Storage() = default;

void Storage::loadSalt() {
    QFile saltFile(configDir_ + "/salt");
    if (saltFile.open(QIODevice::ReadOnly)) {
        QByteArray data = saltFile.readAll();
        if (data.size() == SALT_LENGTH) {
            salt_ = data;
        }
    }
}

void Storage::saveSalt() {
    QFile saltFile(configDir_ + "/salt");
    if (saltFile.open(QIODevice::WriteOnly)) {
        saltFile.write(salt_);
    }
}

bool Storage::hasSalt() const {
    return !salt_.isEmpty();
}

QByteArray Storage::getSalt() const {
    return salt_;
}

void Storage::deriveKey(const QString &fingerprint) {
    unsigned char key[KEY_LENGTH];
    if (PKCS5_PBKDF2_HMAC(fingerprint.toUtf8().constData(), fingerprint.toUtf8().size(),
                          reinterpret_cast<const unsigned char *>(salt_.constData()), salt_.size(),
                          PBKDF2_ITERATIONS, EVP_sha256(), KEY_LENGTH, key) == 1) {
        key_ = QByteArray(reinterpret_cast<char *>(key), KEY_LENGTH);
    }
    OPENSSL_cleanse(key, KEY_LENGTH);
}

QByteArray Storage::encrypt(const QByteArray &plaintext) const {
    if (key_.isEmpty()) return {};

    unsigned char iv[IV_LENGTH];
    RAND_bytes(iv, IV_LENGTH);

    QByteArray ciphertext(plaintext.size() + TAG_LENGTH, 0);

    int ciphertextLen = 0;
    unsigned char tag[TAG_LENGTH];

    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if (!ctx) return {};

    QByteArray result;
    if (EVP_EncryptInit_ex(ctx, EVP_aes_256_gcm(), nullptr, nullptr, nullptr) == 1 &&
        EVP_EncryptInit_ex(ctx, nullptr, nullptr,
                          reinterpret_cast<const unsigned char *>(key_.constData()), iv) == 1 &&
        EVP_EncryptUpdate(ctx, reinterpret_cast<unsigned char *>(ciphertext.data()),
                         &ciphertextLen, reinterpret_cast<const unsigned char *>(plaintext.constData()),
                         plaintext.size()) == 1 &&
        EVP_EncryptFinal_ex(ctx, reinterpret_cast<unsigned char *>(ciphertext.data() + ciphertextLen),
                           &ciphertextLen) == 1 &&
        EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_GET_TAG, TAG_LENGTH, tag) == 1) {
        result.resize(IV_LENGTH + plaintext.size() + TAG_LENGTH);
        memcpy(result.data(), iv, IV_LENGTH);
        memcpy(result.data() + IV_LENGTH, ciphertext.data(), plaintext.size());
        memcpy(result.data() + IV_LENGTH + plaintext.size(), tag, TAG_LENGTH);
    }

    EVP_CIPHER_CTX_free(ctx);
    return result;
}

QByteArray Storage::decrypt(const QByteArray &ciphertext) const {
    if (key_.isEmpty() || ciphertext.size() < IV_LENGTH + TAG_LENGTH) return {};

    unsigned char iv[IV_LENGTH];
    unsigned char tag[TAG_LENGTH];
    memcpy(iv, ciphertext.constData(), IV_LENGTH);
    memcpy(tag, ciphertext.constData() + ciphertext.size() - TAG_LENGTH, TAG_LENGTH);

    int plaintextLen = ciphertext.size() - IV_LENGTH - TAG_LENGTH;
    QByteArray plaintext(plaintextLen, 0);

    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if (!ctx) return {};

    QByteArray result;
    int plaintextPos = 0;

    if (EVP_DecryptInit_ex(ctx, EVP_aes_256_gcm(), nullptr, nullptr, nullptr) == 1 &&
        EVP_DecryptInit_ex(ctx, nullptr, nullptr,
                          reinterpret_cast<const unsigned char *>(key_.constData()), iv) == 1 &&
        EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_TAG, TAG_LENGTH, tag) == 1 &&
        EVP_DecryptUpdate(ctx, reinterpret_cast<unsigned char *>(plaintext.data()),
                         &plaintextPos,
                         reinterpret_cast<const unsigned char *>(ciphertext.constData() + IV_LENGTH),
                         plaintextLen) == 1) {
        int finalLen = 0;
        if (EVP_DecryptFinal_ex(ctx, reinterpret_cast<unsigned char *>(plaintext.data() + plaintextPos),
                               &finalLen) == 1) {
            result = plaintext;
        }
    }

    EVP_CIPHER_CTX_free(ctx);
    return result;
}

bool Storage::save(const QString &filename, const QByteArray &data) const {
    QFile file(configDir_ + "/" + filename);
    if (file.open(QIODevice::WriteOnly)) {
        file.write(data);
        return true;
    }
    return false;
}

QByteArray Storage::load(const QString &filename) const {
    QFile file(configDir_ + "/" + filename);
    if (file.open(QIODevice::ReadOnly)) {
        return file.readAll();
    }
    return {};
}
