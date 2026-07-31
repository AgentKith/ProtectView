package app

import (
	"crypto/aes"
	"crypto/cipher"
	"crypto/rand"
	"crypto/sha256"
	"encoding/hex"
	"encoding/json"
	"errors"
	"fmt"
	"os"
	"path/filepath"

	"golang.org/x/crypto/argon2"
	"golang.org/x/crypto/bcrypt"
)

const (
	configDir = ".config/unvr-carousal"
	configFile = "config.enc"
	saltFile   = ".salt"

	// Argon2id parameters
	argon2Iter      = 32768
	argon2Mem       = 16 * 1024 // 16 MB
	argon2Threads   = 4
	argon2KeyLength = 32
)

// TestArgon2Params are reduced parameters for testing.
var TestArgon2Params = struct {
	Iter, Mem, Threads, KeyLength uint32
}{1, 64, 1, 32}

// Storage handles encrypted config read/write.
type Storage struct {
	dir string
}

// NewStorage creates a new storage instance.
func NewStorage() (*Storage, error) {
	home, err := os.UserHomeDir()
	if err != nil {
		return nil, fmt.Errorf("get home dir: %w", err)
	}

	dir := filepath.Join(home, configDir)
	if err := os.MkdirAll(dir, 0700); err != nil {
		return nil, fmt.Errorf("create config dir: %w", err)
	}

	return &Storage{dir: dir}, nil
}

// Load reads and decrypts the config file.
func (s *Storage) Load() (*Config, error) {
	// Read salt
	salt, err := s.readSalt()
	if err != nil {
		return nil, fmt.Errorf("read salt: %w", err)
	}

	// Read encrypted config
	encData, err := os.ReadFile(filepath.Join(s.dir, configFile))
	if err != nil {
		if errors.Is(err, os.ErrNotExist) {
			return nil, nil // No config yet
		}
		return nil, fmt.Errorf("read config: %w", err)
	}

	// Derive key
	fingerprint, err := GetDeviceFingerprint()
	if err != nil {
		return nil, fmt.Errorf("get fingerprint: %w", err)
	}

	key := deriveKey(fingerprint, salt)

	// Decrypt
	plaintext, err := decrypt(key, encData)
	if err != nil {
		return nil, fmt.Errorf("decrypt: %w", err)
	}

	// Parse JSON
	var cfg Config
	if err := json.Unmarshal(plaintext, &cfg); err != nil {
		return nil, fmt.Errorf("parse json: %w", err)
	}

	return &cfg, nil
}

// Save encrypts and writes the config file.
func (s *Storage) Save(cfg *Config) error {
	// Read or generate salt
	salt, err := s.readSalt()
	if err != nil {
		salt, err = s.generateSalt()
		if err != nil {
			return fmt.Errorf("generate salt: %w", err)
		}
	}

	// Marshal JSON
	plaintext, err := json.MarshalIndent(cfg, "", "    ")
	if err != nil {
		return fmt.Errorf("marshal json: %w", err)
	}

	// Derive key
	fingerprint, err := GetDeviceFingerprint()
	if err != nil {
		return fmt.Errorf("get fingerprint: %w", err)
	}

	key := deriveKey(fingerprint, salt)

	// Encrypt
	encData, err := encrypt(key, plaintext)
	if err != nil {
		return fmt.Errorf("encrypt: %w", err)
	}

	// Write
	if err := os.WriteFile(filepath.Join(s.dir, configFile), encData, 0600); err != nil {
		return fmt.Errorf("write config: %w", err)
	}

	return nil
}

// HashPIN creates a bcrypt hash of the PIN.
func HashPIN(pin string) (string, error) {
	hash, err := bcrypt.GenerateFromPassword([]byte(pin), bcrypt.DefaultCost)
	return string(hash), err
}

// CheckPIN verifies a PIN against a bcrypt hash.
func CheckPIN(pin, hash string) error {
	return bcrypt.CompareHashAndPassword([]byte(hash), []byte(pin))
}

// HasPIN checks if a PIN is set.
func (cfg *Config) HasPIN() bool {
	return cfg.PIN.Hash != ""
}

// deriveKey derives a 32-byte key from device fingerprint and salt.
func deriveKey(fingerprint string, salt []byte) []byte {
	return argon2.IDKey(
		[]byte(fingerprint),
		salt,
		argon2Iter,
		argon2Mem,
		argon2Threads,
		argon2KeyLength,
	)
}

// encrypt encrypts plaintext with AES-256-GCM.
// Output: nonce (12 bytes) || ciphertext || tag (16 bytes, appended by GCM)
func encrypt(key, plaintext []byte) ([]byte, error) {
	block, err := aes.NewCipher(key)
	if err != nil {
		return nil, err
	}

	aead, err := cipher.NewGCM(block)
	if err != nil {
		return nil, err
	}

	nonce := make([]byte, aead.NonceSize())
	if _, err := rand.Read(nonce); err != nil {
		return nil, err
	}

	return aead.Seal(nonce, nonce, plaintext, nil), nil
}

// decrypt decrypts AES-256-GCM ciphertext.
func decrypt(key, ciphertext []byte) ([]byte, error) {
	block, err := aes.NewCipher(key)
	if err != nil {
		return nil, err
	}

	aead, err := cipher.NewGCM(block)
	if err != nil {
		return nil, err
	}

	nonceSize := aead.NonceSize()
	if len(ciphertext) < nonceSize {
		return nil, errors.New("ciphertext too short")
	}

	nonce, ciphertext := ciphertext[:nonceSize], ciphertext[nonceSize:]
	return aead.Open(nil, nonce, ciphertext, nil)
}

// readSalt reads the hex-encoded salt file.
func (s *Storage) readSalt() ([]byte, error) {
	data, err := os.ReadFile(filepath.Join(s.dir, saltFile))
	if err != nil {
		return nil, err
	}
	return hex.DecodeString(string(data))
}

// generateSalt creates a random 16-byte salt and saves it.
func (s *Storage) generateSalt() ([]byte, error) {
	salt := make([]byte, 16)
	if _, err := rand.Read(salt); err != nil {
		return nil, err
	}

	data := hex.EncodeToString(salt)
	if err := os.WriteFile(filepath.Join(s.dir, saltFile), []byte(data), 0600); err != nil {
		return nil, err
	}

	return salt, nil
}

// GetConfigDir returns the config directory path.
func (s *Storage) GetConfigDir() string {
	return s.dir
}

// Checksum is a helper to compute SHA-256 of a certificate for TLS fingerprint.
func Checksum(data []byte) string {
	hash := sha256.Sum256(data)
	return hex.EncodeToString(hash[:])
}
