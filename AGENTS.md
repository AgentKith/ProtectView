# AGENTS.md — Agent Reference

## Project Overview

**unvr-carousal** is a multi-camera security monitor for UniFi UNVR. Displays live camera feeds in a fullscreen grid layout with PIN-protected settings.

**Platforms**: Linux only (desktop + Raspberry Pi). No Windows support.
**GUI**: Fyne (`fyne.io/fyne/v2`)
**Language**: Go 1.21+

## Tech Stack

| Component | Choice | Rationale |
|-----------|--------|-----------|
| GUI | Fyne v2 | Cross-platform, dark/light themes, mobile packaging |
| Video | FFmpeg subprocess | Reliable, hardware acceleration, handles RTSPS |
| Icons | Lucide SVGs | Modern, consistent, embeddable |
| Font | Inter | OFL-1.1, screen-optimized, used by Figma/GitLab/NASA |
| Config | Encrypted JSON | AES-256-GCM, device-bound via argon2id |
| PIN | bcrypt hash | 6-char, stored in encrypted config |

## Key Decisions

### Video Architecture
- **Per-camera FFmpeg**: One subprocess per camera, independent streams, resilient to individual failures
- **Composite FFmpeg**: One subprocess, `filter_complex` grid, efficient but less resilient
- **User picks** in settings. Pi defaults to per-camera.

### Frame Rendering
- FFmpeg outputs rawvideo to stdout
- Goroutine reads into `image.RGBA`, calls `canvas.Refresh()` per frame
- Simple, works, no OpenGL complexity

### Config Storage
- `~/.config/unvr-carousal/config.enc`
- AES-256-GCM encrypted, key derived from device fingerprint via `argon2id`
- Pi fingerprint: `/proc/cpuinfo` Serial
- Android fingerprint: `Settings.Secure.ANDROID_ID` (future)

### Camera Layout
- Auto grid: calculate optimal rows×cols from camera count + screen aspect ratio
- User override: custom grid layout in settings
- Default: show all cameras. User can customize selection and order.

### Error Handling
- Dark tile + camera name + pulsing retry icon
- Auto-retry every 5s, tap to force reconnect

### Touch Interactions (Android, future)
- Double-tap: quick menu
- Long-press: hold to reveal menu, release to dismiss

### Font
- Inter: Regular 400, Medium 500, Semi Bold 600, Bold 700
- Embedded via `go:embed`, ~400KB total
- No internet needed at runtime

### TLS
- Skip verification or SSL fingerprint, user choice in settings

### FFmpeg Path
- Check PATH first, fallback to user-specified path in settings

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
- [ ] Init Go module
- [ ] Config storage (AES-256-GCM, argon2id, device fingerprint)
- [ ] UNVR client (TLS, auth, camera listing, stream creation)
- [ ] Video interface + FFmpeg backends (per-camera, composite)
- [ ] Snapshot backend
- [ ] Pi relay backend

### Phase 2: UI Core
- [ ] Custom theme (dark/light, Inter font, colors, sizes)
- [ ] Lucide icons (embed, render)
- [ ] Main window (fullscreen, auto grid)
- [ ] Camera tiles (canvas.Image, error states)

### Phase 3: Interactive UI
- [ ] Edge-hover menu (mouse + touch)
- [ ] Setup wizard (first-run flow)
- [ ] Settings panel (all options)
- [ ] PIN pad (random buttons, shake animation)

### Phase 4: Polish
- [ ] Android scaffolding (build tags)
- [ ] Error handling (retries, crash recovery)
- [ ] Settings hot-reload
