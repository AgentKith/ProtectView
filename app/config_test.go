package app

import (
	"testing"
)

func TestNewDefaultConfig(t *testing.T) {
	cfg := NewDefaultConfig()

	if cfg.Version != 1 {
		t.Errorf("expected version 1, got %d", cfg.Version)
	}
	if cfg.UNVR.Port != 443 {
		t.Errorf("expected port 443, got %d", cfg.UNVR.Port)
	}
	if cfg.UNVR.TLSMode != "fingerprint" {
		t.Errorf("expected TLS mode fingerprint, got %s", cfg.UNVR.TLSMode)
	}
	if cfg.Video.Mode != "per-camera" {
		t.Errorf("expected video mode per-camera, got %s", cfg.Video.Mode)
	}
	if cfg.Video.Quality != "medium" {
		t.Errorf("expected quality medium, got %s", cfg.Video.Quality)
	}
	if cfg.Layout.Mode != "auto" {
		t.Errorf("expected layout mode auto, got %s", cfg.Layout.Mode)
	}
	if cfg.Appearance.Theme != "dark" {
		t.Errorf("expected theme dark, got %s", cfg.Appearance.Theme)
	}
	if !cfg.Appearance.Fullscreen {
		t.Error("expected fullscreen to be true")
	}
}

func TestConfigHasPIN(t *testing.T) {
	tests := []struct {
		name string
		cfg  *Config
		want bool
	}{
		{"empty hash", NewDefaultConfig(), false},
		{"with hash", &Config{PIN: PIN{Hash: "test"}}, true},
	}

	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			if got := tt.cfg.HasPIN(); got != tt.want {
				t.Errorf("HasPIN() = %v, want %v", got, tt.want)
			}
		})
	}
}
