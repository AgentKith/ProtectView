# ProtectView

Multi-camera security monitor for UniFi UNVR. Displays live camera feeds in a fullscreen grid layout with PIN-protected settings.

## Features

- Live camera feeds from UniFi UNVR via RTSPS
- Auto-grid layout (adapts to camera count and screen size)
- Composite or per-camera FFmpeg playback
- PIN-protected settings with decoy buttons
- Encrypted configuration (AES-256-GCM, device-bound)
- Dark/light theme with Plus Jakarta Sans font
- Kiosk mode (full lockdown, auto-restart, no TTY)
- Linux desktop and Raspberry Pi support

## Prerequisites

- **CMake** 3.24+
- **Qt6** (Core, Gui, Widgets, Network, Multimedia, Test, OpenGLWidgets, Svg)
- **OpenSSL** (AES-256-GCM, argon2id)
- **FFmpeg** (for video playback)
- **libX11** (for kiosk key grab)
- **C++20 compiler** (GCC 11+ or Clang 14+)
- UniFi UNVR with Protect API access

## Quick Start

```bash
git clone <repo-url>
cd ProtectView
cmake -B build
cmake --build build
./build/src/protectview
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
