# AGENTS.md — Agent Reference

## Project Overview

**ProtectView** is a multi-camera security monitor for UniFi UNVR. Displays live camera feeds in a fullscreen grid layout with PIN-protected settings.

**Platforms**: Linux only (desktop + Raspberry Pi). No Windows support.
**GUI**: Qt6 Widgets
**Language**: C++20
**Build**: CMake 3.24+

## Tech Stack

| Component | Choice | Rationale |
|-----------|--------|-----------|
| GUI | Qt6 Widgets | Native, QSS styling, resource system |
| Video | FFmpeg subprocess | Reliable, hardware acceleration, handles RTSPS |
| Icons | Lucide SVGs | Embedded via Qt resources, renderable with QSvg |
| Font | Plus Jakarta Sans | SIL OFL, variable font, embedded via Qt resources |
| Config | Encrypted JSON | AES-256-GCM, device-bound via argon2id |
| PIN | bcrypt hash | 6-char, stored in encrypted config |
| Build | CMake | AUTOMOC/AUTORCC, Qt6 integration, test support |
| Crypto | OpenSSL | AES-256-GCM encryption, argon2id key derivation |
| X11 | libX11 | Key grab for kiosk mode (Alt+Tab, PrintScreen, Super) |

## Key Decisions

### Video Architecture
- **Per-camera FFmpeg**: One subprocess per camera, independent streams, resilient to individual failures
- **Composite FFmpeg**: One subprocess, `filter_complex` grid, efficient but less resilient
- **Snapshot polling**: HTTP GET snapshot endpoint every 200ms, no FFmpeg needed
- **User picks** in settings. Pi defaults to per-camera.

### Frame Rendering
- FFmpeg outputs rawvideo to stdout
- `QProcess` reads raw bytes into `QImage`
- `CameraTileWidget::setFrame()` stores `QImage`, calls `update()` to trigger `paintEvent`
- `paintEvent` draws scaled frame with `QPainter`

### Config Storage
- `~/.config/ProtectView/config.enc`
- AES-256-GCM encrypted, key derived from device fingerprint via `argon2id`
- Pi fingerprint: `/proc/cpuinfo` Serial
- Desktop fingerprint: `/etc/machine-id`

### Camera Layout
- Auto grid: calculate optimal rows×cols from camera count + screen aspect ratio
- User override: custom grid layout in settings
- Default: show all cameras. User can customize selection and order.
- **Current**: hard-coded 2-column grid in `MainWindow::arrangeGrid()`

### Error Handling
- Dark tile + camera name + pulsing retry icon
- Auto-retry every 5s, double-click to force reconnect

### Font
- Plus Jakarta Sans: Variable font (all weights in one file)
- Embedded via Qt resources (`:PlusJakartaSans-Variable.ttf`)
- Registered with `QFontDatabase::addApplicationFont()`
- No internet needed at runtime

### TLS
- Skip verification or SSL fingerprint, user choice in settings
- Three modes: `Verify` (default), `Skip`, `Fingerprint`

### FFmpeg Path
- Check PATH first, fallback to user-specified path in settings

### Kiosk Mode
- `--setup-kiosk` (sudo): creates `unvr-kiosk` user, configures LightDM autologin, disables TTY, creates systemd guard
- `--undo-kiosk` (sudo): reverses all changes, removes user, restores normal boot
- `--kiosk`: runs in kiosk mode (no decorations, prevents close, X11 key grabs)
- Root only for setup, app runs as unprivileged kiosk user

### Pi Relay
- Pi runs FFmpeg, serves MJPEG stream over HTTP
- Android connects to Pi's stream instead of UNVR directly
- Offloads decoding to Pi, low Android CPU

## Source Structure

```
src/
├── main.cpp                    # Entry point, CLI parsing, font/style loading
├── app/
│   ├── appcontroller.h/cpp     # Central orchestrator, state machine
│   ├── cameramanager.h/cpp     # Camera lifecycle, player management
│   ├── config.h/cpp            # Config structs, JSON serialization
│   ├── fingerprint.h/cpp       # Device fingerprint extraction
│   ├── kiosk.h/cpp             # Kiosk setup/undo
│   └── storage.h/cpp           # Encrypted file storage
├── ui/
│   ├── mainwindow.h/cpp        # Main window, grid layout, edge menu
│   ├── cameratilewidget.h/cpp  # Single camera tile, paintEvent
│   ├── edgemenu.h/cpp          # Edge-hover overlay menu
│   ├── settingsdialog.h/cpp    # Settings panel
│   ├── setupwizard.h/cpp       # First-run setup wizard
│   ├── pinpad.h/cpp            # PIN pad widget
│   ├── pinverifydialog.h/cpp   # PIN verification dialog
│   └── styles/app.qss          # QSS stylesheet
├── unvr/
│   ├── client.h/cpp            # HTTP client for UniFi Protect API
│   └── types.h/cpp             # CameraInfo, StreamResponse structs
└── video/
    ├── player.h/cpp            # VideoPlayer base class
    ├── ffmpegplayer.h/cpp      # Per-camera FFmpeg player
    ├── compositeplayer.h/cpp   # Composite grid player
    ├── snapshotplayer.h/cpp    # Snapshot polling player
    └── relayplayer.h/cpp       # Pi relay player
```

## Testing

Tests use Qt6 Test with offscreen platform (no display server needed). `tests/CMakeLists.txt` sets `QT_QPA_PLATFORM=offscreen` automatically via test properties.

```bash
# From build directory
ctest --output-on-failure
```

## Doc Index

| Doc | Purpose |
|-----|---------|
| [README.md](README.md) | Project overview, quick start |
| [docs/ARCHITECTURE.md](docs/ARCHITECTURE.md) | Components, data flow, video backends |
| [docs/DESIGN.md](docs/DESIGN.md) | Theme, fonts, icons, UI screens, animations |
| [docs/BUILD.md](docs/BUILD.md) | Build instructions, Pi deployment |
| [docs/CONFIG.md](docs/CONFIG.md) | Encrypted config format, settings reference |
| [docs/API.md](docs/API.md) | UNVR Protect API reference |

## Implementation Checklist

### Phase 1: Foundation
- [x] CMake project setup (C++20, Qt6, AUTOMOC/AUTORCC)
- [x] Config storage (AES-256-GCM, argon2id, device fingerprint)
- [x] UNVR client (TLS, auth, camera listing, stream creation)
- [x] Video interface + FFmpeg backends (per-camera, composite)
- [x] Snapshot backend
- [x] Pi relay backend

### Phase 2: UI Core
- [x] QSS stylesheet (Neon Touch theme, dark/light)
- [x] Plus Jakarta Sans font (variable, Qt resources)
- [x] Lucide icons (embedded, QSvg render)
- [x] Main window (fullscreen, grid layout)
- [x] Camera tiles (custom paintEvent, error states)

### Phase 3: Interactive UI
- [x] Edge-hover menu (mouse + touch)
- [x] Setup wizard (first-run flow, QWizard)
- [x] Settings panel (all options)
- [x] PIN pad (random buttons, shake animation)

### Phase 4: Polish
- [x] Kiosk mode (--setup-kiosk, --undo-kiosk, --kiosk)
- [ ] Android scaffolding
- [x] Error handling (retries, crash recovery)
- [ ] Settings hot-reload
