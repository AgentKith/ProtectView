# unvr-carousal

Multi-camera security monitor for UniFi UNVR. Displays live camera feeds in a fullscreen grid layout with PIN-protected settings.

## Features

- Live camera feeds from UniFi UNVR via RTSPS
- Auto-grid layout (adapts to camera count and screen size)
- Composite or per-camera FFmpeg playback
- PIN-protected settings with decoy buttons
- Encrypted configuration (AES-256-GCM, device-bound)
- Dark/light theme with Inter font
- Linux desktop and Raspberry Pi support

## Prerequisites

- Go 1.21+
- FFmpeg (for video playback)
- UniFi UNVR with Protect API access

## Quick Start

```bash
git clone <repo-url>
cd unvr-carousal
go build -o unvr-carousal .
./unvr-carousal
```

First run launches the setup wizard: UNVR host → API key → set PIN → done.

## Documentation

- [Architecture](docs/ARCHITECTURE.md) — Components, data flow, video backends
- [Design](docs/DESIGN.md) — Theme, fonts, icons, UI screens, animations
- [Build](docs/BUILD.md) — Build instructions, Pi deployment
- [Config](docs/CONFIG.md) — Encrypted config format, settings reference
- [API](docs/API.md) — UNVR Protect API reference

## License

MIT
