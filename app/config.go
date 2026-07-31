package app

// Config is the top-level configuration structure.
type Config struct {
	Version    int      `json:"version"`
	UNVR       UNVR     `json:"unvr"`
	PIN        PIN      `json:"pin"`
	Video      Video    `json:"video"`
	Layout     Layout   `json:"layout"`
	Appearance Appearance `json:"appearance"`
}

// UNVR holds UniFi UNVR connection settings.
type UNVR struct {
	Host            string `json:"host"`
	Port            int    `json:"port"`
	TLSMode         string `json:"tls_mode"`
	TLSFingerprint  string `json:"tls_fingerprint"`
	APIKey          string `json:"api_key"`
}

// PIN holds the hashed PIN.
type PIN struct {
	Hash string `json:"hash"`
}

// Video holds video backend settings.
type Video struct {
	Mode       string `json:"mode"`
	FFmpegPath string `json:"ffmpeg_path"`
	Quality    string `json:"quality"`
}

// Layout holds camera layout settings.
type Layout struct {
	Mode    string     `json:"mode"`
	Rows    int        `json:"rows"`
	Cols    int        `json:"cols"`
	Cameras []Camera   `json:"cameras"`
}

// Camera holds per-camera settings.
type Camera struct {
	ID      string `json:"id"`
	Name    string `json:"name"`
	Enabled bool   `json:"enabled"`
	Quality string `json:"quality"`
}

// Appearance holds UI appearance settings.
type Appearance struct {
	Theme     string `json:"theme"`
	Fullscreen bool  `json:"fullscreen"`
	Kiosk     bool   `json:"kiosk"`
}

// NewDefaultConfig returns a config with sensible defaults.
func NewDefaultConfig() *Config {
	return &Config{
		Version: 1,
		UNVR: UNVR{
			Port:    443,
			TLSMode: "fingerprint",
		},
		Video: Video{
			Mode:    "per-camera",
			Quality: "medium",
		},
		Layout: Layout{
			Mode: "auto",
		},
		Appearance: Appearance{
			Theme:      "dark",
			Fullscreen: true,
		},
	}
}
