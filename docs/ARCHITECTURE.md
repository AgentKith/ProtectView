# Architecture

## Component Diagram

```
┌─────────────────────────────────────────────────────────────┐
│                      AppController                           │
│  ┌───────────────────────────────────────────────────────┐  │
│  │                      MainWindow                        │  │
│  │  ┌──────────────────────────────────────────────────┐ │  │
│  │  │                 Camera Grid (QGridLayout)         │ │  │
│  │  │  ┌─────────┐ ┌─────────┐ ┌─────────┐ ┌─────────┐ │ │  │
│  │  │  │ Tile 1  │ │ Tile 2  │ │ Tile 3  │ │ Tile 4  │ │ │  │
│  │  │  │QImage+  │ │QImage+  │ │QImage+  │ │QImage+  │ │ │  │
│  │  │  │paintEvent│ │paintEvent│ │paintEvent│ │paintEvent│ │ │  │
│  │  │  └────┬────┘ └────┬────┘ └────┬────┘ └────┬────┘ │ │  │
│  │  └───────┼───────────┼───────────┼───────────┼───────┘ │  │
│  │          │           │           │           │         │  │
│  │  ┌───────▼───────────▼───────────▼───────────▼───────┐ │  │
│  │  │                  EdgeMenu                          │ │  │
│  │  └───────────────────────────────────────────────────┘ │  │
│  └───────────────────────────────────────────────────────┘  │
│                                                             │
│  ┌───────────────────────────────────────────────────────┐  │
│  │                  CameraManager                         │  │
│  │  ┌─────────────┐ ┌─────────────┐ ┌─────────────┐    │  │
│  │  │FFmpegPlayer1│ │FFmpegPlayer2│ │FFmpegPlayerN│    │  │
│  │  │QProcess→    │ │QProcess→    │ │QProcess→    │    │  │
│  │  │QImage       │ │QImage       │ │QImage       │    │  │
│  │  └─────────────┘ └─────────────┘ └─────────────┘    │  │
│  └──────────────────────────────────────────────────────┘  │
│                                                             │
│  ┌───────────────────────────────────────────────────────┐  │
│  │                    UNVRClient                          │  │
│  │  GET /v1/cameras  │  POST /v1/cameras/{id}/rtsps-stream│  │
│  └───────────────────────────────────────────────────────┘  │
└─────────────────────────────────────────────────────────────┘
```

## Data Flow

```
UNVR ──RTSPS──▶ FFmpeg ──rawvideo──▶ QProcess ──QImage──▶ CameraTileWidget::setFrame() ──▶ QPainter
```

1. **UNVRClient** requests RTSPS stream URLs via `POST /v1/cameras/{id}/rtsps-stream`
2. **FFmpegPlayer** spawns FFmpeg subprocess, connects to RTSPS URL, decodes video, outputs raw RGB24 to stdout
3. **QProcess** reads raw bytes from FFmpeg stdout, constructs `QImage`
4. **CameraManager** emits `frameReady(index, QImage)` signal
5. **MainWindow** forwards to `CameraTileWidget::setFrame()` which stores the image and calls `update()`
6. **CameraTileWidget::paintEvent** draws scaled frame with `QPainter`

## Video Backends

### Per-Camera FFmpeg (Default)
- One FFmpeg subprocess per camera (`FFmpegPlayer`)
- Each camera has independent `QImage` in its `CameraTileWidget`
- **Pros**: Resilient (one camera fails, others continue), easy to resize/rearrange
- **Cons**: More CPU, loose frame sync between cameras
- **Command**: `ffmpeg -i rtsps://... -f rawvideo -pixfmt rgb24 pipe:1`

### Composite FFmpeg
- One FFmpeg subprocess, `filter_complex` combines all cameras into grid (`CompositePlayer`)
- Single `QImage` for entire grid
- **Pros**: One process, hardware compositing, efficient
- **Cons**: Complex filter graph, one stream dies = whole layout breaks
- **Filter example** (2x2): `[0:v]scale=640:360[topleft]; [1:v]scale=640:360[topright]; ... [topleft][topright][bottomleft][bottomright]hstack=2`

### Snapshot Polling
- Poll `GET /v1/cameras/{id}/snapshot` every 200ms (`SnapshotPlayer`)
- Decode JPEG, update `QImage`
- **Pros**: No FFmpeg needed, works on low-power devices
- **Cons**: 5fps, choppy, higher bandwidth

### Pi Relay
- Pi runs FFmpeg, serves MJPEG stream over HTTP on local network (`RelayPlayer`)
- Client connects to Pi's stream instead of UNVR directly
- **Pros**: Offloads decoding to Pi, low client CPU, single point of UNVR connection
- **Cons**: Requires Pi running, network dependency
- **Server**: HTTP endpoint `/stream/{camera-id}.mjpg`, MJPEG frames from FFmpeg
- **Client**: HTTP GET, parse multipart JPEG, decode each frame

## Platform Matrix

| Feature | Linux Desktop | Raspberry Pi |
|---------|--------------|--------------|
| Per-camera FFmpeg | Yes | Yes |
| Composite FFmpeg | Yes | Yes |
| Snapshot polling | Yes | Yes |
| Pi relay | Server | Server |
| Edge-hover menu | Yes | Yes |
| Kiosk mode | Yes | Yes |

## Config Storage

```
~/.config/ProtectView/
└── config.enc          # AES-256-GCM encrypted JSON
```

- **Encryption**: AES-256-GCM (OpenSSL)
- **Key derivation**: `argon2id(device_fingerprint, salt, iter=32768, mem=16384, threads=4, keyLen=32)`
- **Device fingerprint**:
  - Pi: `/proc/cpuinfo` Serial field
  - Desktop: Machine ID from `/etc/machine-id`
- **PIN**: bcrypt hash stored in encrypted config

## Error Handling

- **Camera stream failure**: Dark tile + camera name + pulsing retry icon
- **Auto-retry**: Every 5 seconds (QTimer in CameraTileWidget)
- **Manual retry**: Double-click tile to force reconnect
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
- X11 key grabs: Alt+Tab, PrintScreen, Super key
- Systemd restarts on any exit

## State Machine

```
NoConfig ──▶ Wizard ──(accepted)──▶ Connecting ──▶ Connected
   │              │                      │
   │         (cancelled)              (error)
   │              │                      │
   └──────────────┴──────────────────────┴──▶ Settings
```

- `AppController` manages state transitions
- `stateChanged` signal emitted on each transition
- `camerasLoaded` signal emitted when MainWindow is ready
