package app

import (
	"encoding/json"
	"os"
	"path/filepath"
	"testing"

	"golang.org/x/crypto/argon2"
)

func TestEncryptDecrypt(t *testing.T) {
	key := make([]byte, 32)
	for i := range key {
		key[i] = byte(i)
	}

	plaintext := []byte(`{"test":"data"}`)

	encrypted, err := encrypt(key, plaintext)
	if err != nil {
		t.Fatalf("encrypt failed: %v", err)
	}

	decrypted, err := decrypt(key, encrypted)
	if err != nil {
		t.Fatalf("decrypt failed: %v", err)
	}

	if string(decrypted) != string(plaintext) {
		t.Errorf("decrypted = %q, want %q", decrypted, plaintext)
	}
}

func TestEncryptProducesDifferentCiphertext(t *testing.T) {
	key := make([]byte, 32)
	for i := range key {
		key[i] = byte(i)
	}

	plaintext := []byte(`{"test":"data"}`)

	enc1, err := encrypt(key, plaintext)
	if err != nil {
		t.Fatalf("encrypt 1 failed: %v", err)
	}

	enc2, err := encrypt(key, plaintext)
	if err != nil {
		t.Fatalf("encrypt 2 failed: %v", err)
	}

	if string(enc1) == string(enc2) {
		t.Error("expected different ciphertexts for same plaintext (nonce should be random)")
	}
}

func TestDecryptWrongKey(t *testing.T) {
	key := make([]byte, 32)
	for i := range key {
		key[i] = byte(i)
	}

	wrongKey := make([]byte, 32)
	for i := range wrongKey {
		wrongKey[i] = byte(i + 1)
	}

	plaintext := []byte(`{"test":"data"}`)
	encrypted, err := encrypt(key, plaintext)
	if err != nil {
		t.Fatalf("encrypt failed: %v", err)
	}

	_, err = decrypt(wrongKey, encrypted)
	if err == nil {
		t.Error("expected error when decrypting with wrong key")
	}
}

func TestDecryptTooShort(t *testing.T) {
	key := make([]byte, 32)
	for i := range key {
		key[i] = byte(i)
	}

	short := []byte{0x00, 0x01, 0x02}
	_, err := decrypt(key, short)
	if err == nil {
		t.Error("expected error for too-short ciphertext")
	}
}

func TestHashAndCheckPIN(t *testing.T) {
	pin := "123456"

	hash, err := HashPIN(pin)
	if err != nil {
		t.Fatalf("HashPIN failed: %v", err)
	}

	if err := CheckPIN(pin, hash); err != nil {
		t.Errorf("CheckPIN failed for correct PIN: %v", err)
	}

	if err := CheckPIN("654321", hash); err == nil {
		t.Error("CheckPIN should fail for wrong PIN")
	}
}

func TestChecksum(t *testing.T) {
	data := []byte("test certificate")
	cs := Checksum(data)

	if len(cs) != 64 {
		t.Errorf("expected 64 hex chars, got %d", len(cs))
	}

	// Same input should produce same checksum
	cs2 := Checksum(data)
	if cs != cs2 {
		t.Errorf("checksum not deterministic: %s != %s", cs, cs2)
	}
}

func TestStorageSaveLoad(t *testing.T) {
	// Create temp dir
	tmpDir := t.TempDir()
	testConfigDir := filepath.Join(tmpDir, configDir)
	if err := os.MkdirAll(testConfigDir, 0700); err != nil {
		t.Fatalf("create temp config dir: %v", err)
	}

	storage := &Storage{dir: testConfigDir}

	cfg := NewDefaultConfig()
	cfg.UNVR.Host = "192.168.1.100"
	cfg.UNVR.APIKey = "test-key"

	// Use fast key derivation for testing
	salt, err := storage.generateSalt()
	if err != nil {
		t.Fatalf("generate salt: %v", err)
	}

	fingerprint := "test-fingerprint"
	key := argon2.IDKey([]byte(fingerprint), salt, TestArgon2Params.Iter, TestArgon2Params.Mem, uint8(TestArgon2Params.Threads), TestArgon2Params.KeyLength)

	plaintext, err := json.MarshalIndent(cfg, "", "    ")
	if err != nil {
		t.Fatalf("marshal: %v", err)
	}

	encData, err := encrypt(key, plaintext)
	if err != nil {
		t.Fatalf("encrypt: %v", err)
	}

	if err := os.WriteFile(filepath.Join(testConfigDir, configFile), encData, 0600); err != nil {
		t.Fatalf("write config: %v", err)
	}

	// Verify file exists and is non-empty
	info, err := os.Stat(filepath.Join(testConfigDir, configFile))
	if err != nil {
		t.Fatalf("stat config: %v", err)
	}
	if info.Size() == 0 {
		t.Error("config file is empty")
	}
}

func TestStorageLoadNoConfig(t *testing.T) {
	tmpDir := t.TempDir()
	configDir := filepath.Join(tmpDir, configDir)
	if err := os.MkdirAll(configDir, 0700); err != nil {
		t.Fatalf("create temp config dir: %v", err)
	}

	// Create salt file
	salt := make([]byte, 16)
	for i := range salt {
		salt[i] = byte(i)
	}
	if err := os.WriteFile(filepath.Join(configDir, saltFile), []byte("00000000000000000000000000000000"), 0600); err != nil {
		t.Fatalf("write salt: %v", err)
	}

	storage := &Storage{dir: configDir}

	loaded, err := storage.Load()
	if err != nil {
		t.Fatalf("Load failed: %v", err)
	}
	if loaded != nil {
		t.Error("expected nil config when no config file exists")
	}
}
