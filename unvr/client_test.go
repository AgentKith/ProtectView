package unvr

import (
	"encoding/hex"
	"net/http"
	"testing"
)

func TestNewClient(t *testing.T) {
	tests := []struct {
		name       string
		host       string
		port       int
		tlsMode    string
		tlsFp      string
		apiKey     string
		wantBase   string
		wantSkip   bool
	}{
		{
			name:     "skip TLS",
			host:     "192.168.1.100",
			port:     443,
			tlsMode:  "skip",
			apiKey:   "test-key",
			wantBase: "https://192.168.1.100:443/proxy/protect/integration",
			wantSkip: true,
		},
		{
			name:     "fingerprint TLS",
			host:     "unvr.local",
			port:     443,
			tlsMode:  "fingerprint",
			tlsFp:    "aa:bb:cc",
			apiKey:   "test-key",
			wantBase: "https://unvr.local:443/proxy/protect/integration",
			wantSkip: false,
		},
		{
			name:     "custom port",
			host:     "10.0.0.1",
			port:     8443,
			tlsMode:  "skip",
			apiKey:   "test-key",
			wantBase: "https://10.0.0.1:8443/proxy/protect/integration",
			wantSkip: true,
		},
	}

	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			client, err := New(tt.host, tt.port, tt.tlsMode, tt.tlsFp, tt.apiKey)
			if err != nil {
				t.Fatalf("New failed: %v", err)
			}

			if client.baseURL != tt.wantBase {
				t.Errorf("baseURL = %q, want %q", client.baseURL, tt.wantBase)
			}

			if client.apiKey != tt.apiKey {
				t.Errorf("apiKey = %q, want %q", client.apiKey, tt.apiKey)
			}

			skipVerify := client.http.Transport.(*http.Transport).TLSClientConfig.InsecureSkipVerify
			if skipVerify != tt.wantSkip {
				t.Errorf("InsecureSkipVerify = %v, want %v", skipVerify, tt.wantSkip)
			}
		})
	}
}

func TestNewClientTimeout(t *testing.T) {
	client, err := New("192.168.1.100", 443, "skip", "", "test-key")
	if err != nil {
		t.Fatalf("New failed: %v", err)
	}

	if client.http.Timeout != 30*1000000000 { // 30s in nanoseconds
		t.Errorf("timeout = %v, want 30s", client.http.Timeout)
	}
}

func TestSha256Hash(t *testing.T) {
	data := []byte("test")
	hash := sha256Hash(data)

	if len(hash) != 32 {
		t.Errorf("expected 32 bytes, got %d", len(hash))
	}

	// Known SHA-256 of "test"
	expected := "9f86d081884c7d659a2feaa0c55ad015a3bf4f1b2b0b822cd15d6c15b0f00a08"
	actual := hex.EncodeToString(hash)
	if actual != expected {
		t.Errorf("sha256(test) = %s, want %s", actual, expected)
	}
}
