# Architecture

## Component Diagram

```
┌─────────────────────────────────────────────────────────────┐
│                        Main Window                          │
│  ┌───────────────────────────────────────────────────────┐  │
│  │                   Camera Grid                          │  │
│  │  ┌─────────┐ ┌─────────┐ ┌─────────┐ ┌─────────┐    │  │
│  │  │ Tile 1  │ │ Tile 2  │ │ Tile 3  │ │ Tile 4  │    │  │
│  │  │canvas.Image│ │canvas.Image│ │canvas.Image│ │canvas.Image│    │  │
│  │  └────┬────┘ └────┬────┘ └────┬────┘ └────┬────┘    │  │
│  └───────┼───────────┼───────────┼───────────┼──────────┘  │
│          │           │           │           │              │
│  ┌───────▼───────────▼───────────▼───────────▼──────────┐  │
│  │                  Video Manager                        │  │
│  │  ┌─────────────┐ ┌─────────────┐ ┌─────────────┐    │  │
│  │  │FFmpeg Proc 1│ │FFmpeg Proc 2│ │FFmpeg Proc N│    │  │
│  │  │rawvideo→img │ │rawvideo→img │ │rawvideo→img │    │  │
│  │  └─────────────┘ └─────────────┘ └─────────────┘    │  │
│  └──────────────────────────────────────────────────────┘  │
│                                                             │
│  ┌──────────────────────────────────────────────────────┐  │
│  │              UNVR Client                              │  │
│  │  GET /v1/cameras  │  POST /v1/cameras/{id}/rtsps-stream│  │
│  └──────────────────────────────────────────────────────┘  │
└─────────────────────────────────────────────────────────────┘
```

## Data Flow

```
UNVR ──RTSPS──▶ FFmpeg ──rawvideo──▶ Goroutine ──image.RGBA──▶ canvas.Image ──▶ Fyne Window
```

1. **UNVR Client** requests RTSPS stream URLs via `POST /v1/cameras/{id}/rtsps-stream`
2. **FFmpeg** connects to RTSPS URL, decodes video, outputs raw RGB24 to stdout
3. **Goroutine** reads raw bytes from FFmpeg stdout, constructs `image.RGBA`
4. **Frame handler** updates `canvas.Image` and calls `canvas.Refresh()`
5. **Fyne** renders frame to window

## Video Backends

### Per-Camera FFmpeg (Default)
- One FFmpeg subprocess per camera
- Each camera has independent `canvas.Image`
- **Pros**: Resilient (one camera fails, others continue), easy to resize/rearrange
- **Cons**: More CPU, loose frame sync between cameras
- **Command**: `ffmpeg -i rtsps://... -f rawvideo -pixfmt rgb24 pipe:1`

### Composite FFmpeg
- One FFmpeg subprocess, `filter_complex` combines all cameras into grid
- Single `canvas.Image` for entire grid
- **Pros**: One process, hardware compositing, efficient
- **Cons**: Complex filter graph, one stream dies = whole layout breaks
- **Filter example** (2x2): `[0:v]scale=640:360[topleft]; [1:v]scale=640:360[topright]; ... [topleft][topright][bottomleft][bottomright]hstack=2`

### Snapshot Polling
- Poll `GET /v1/cameras/{id}/snapshot` every 200ms
- Decode JPEG, update `canvas.Image`
- **Pros**: No FFmpeg needed, works on Android
- **Cons**: 5fps, choppy, higher bandwidth

### Pi Relay
- Pi runs FFmpeg, serves MJPEG stream over HTTP on local network
- Android connects to Pi's stream instead of UNVR directly
- **Pros**: Offloads decoding to Pi, low Android CPU, single point of UNVR connection
- **Cons**: Requires Pi running, network dependency
- **Server**: HTTP endpoint `/stream/{camera-id}.mjpg`, MJPEG frames from FFmpeg
- **Client**: HTTP GET, parse multipart JPEG, decode each frame

## Platform Matrix

| Feature | Linux Desktop | Raspberry Pi | Android (future) |
|---------|--------------|--------------|------------------|
| Per-camera FFmpeg | Yes | Yes | No |
| Composite FFmpeg | Yes | Yes | No |
| Snapshot polling | Yes | Yes | Yes |
| Pi relay | Server | Server | Client |
| HLS WebView | No | No | Yes |
| Pure-Go RTSP | Yes | Yes | Yes |
| Edge-hover menu | Yes | Yes | No |
| Touch menu | No | No | Yes |

## Config Storage

```
~/.config/ProtectView/
├── config.enc          # AES-256-GCM encrypted JSON
└── .salt               # 16-byte random salt (per-device)
```

- **Encryption**: AES-256-GCM
- **Key derivation**: `argon2id(device_fingerprint, salt, iter=32768, mem=16384, threads=4, keyLen=32)`
- **Device fingerprint**:
  - Pi: `/proc/cpuinfo` Serial field
  - Desktop: Machine ID from `/etc/machine-id`
  - Android: `Settings.Secure.ANDROID_ID`
- **PIN**: bcrypt hash stored in encrypted config

## Error Handling

- **Camera stream failure**: Dark tile + camera name + pulsing retry icon
- **Auto-retry**: Every 5 seconds
- **Manual retry**: Tap tile to force reconnect
- **FFmpeg crash**: Restart subprocess, preserve state
- **UNVR unreachable**: Show error in all tiles, retry loop

## Kiosk Mode

### Setup Flow (`--setup-kiosk`, requires sudo)

1. Create `unvr-kiosk` user (no shell, no login password)
2. Configure LightDM autologin for `unvr-kiosk`
3. Disable TTY access (`NAutoVTs=0`, `ReserveVT=0` in `logind.conf`)
4. Create systemd service (`Restart=always`, `RestartSec=1`)
5. Install openbox autostart (only launches `ProtectView --kiosk`)
6. Drop privileges, exit

### Undo Flow (`--undo-kiosk`, requires sudo)

1. Stop systemd service
2. Restore LightDM config (remove autologin)
3. Re-enable TTY access
4. Remove openbox autostart
5. Remove `unvr-kiosk` user
6. Remove systemd service

### Kiosk Runtime (`--kiosk`)

- No window decorations
- Catch window close events, ignore them
- Fullscreen only, no toggle
- Systemd restarts on any exit
