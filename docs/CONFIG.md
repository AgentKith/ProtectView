# Config

## File Location

```
~/.config/ProtectView/
└── config.enc    # AES-256-GCM encrypted JSON
```

## Encryption

### Algorithm

- **Cipher**: AES-256-GCM (OpenSSL)
- **Key derivation**: argon2id
- **Key length**: 32 bytes (256 bits)
- **Nonce**: 12 bytes, random per encryption

### Key Derivation

```cpp
key = argon2id(
    password: device_fingerprint,
    salt: file_salt,
    iter: 32768,
    mem: 16384 KB,
    threads: 4,
    keyLen: 32
)
```

### Device Fingerprint

| Platform | Source | Example |
|----------|--------|---------|
| Raspberry Pi | `/proc/cpuinfo` Serial | `10000000e1234567` |
| Linux Desktop | `/etc/machine-id` | `abcd1234efgh5678ijkl9012mnop3456` |

### Salt Generation

On first run, `Storage` generates 16 random bytes, saves as hex-encoded string in `.salt` file within the config directory.

## Config Schema

### Plaintext JSON (before encryption)

```json
{
    "version": 1,
    "unvr": {
        "host": "192.168.1.100",
        "port": 443,
        "tlsMode": "Verify",
        "tlsFingerprint": "",
        "apiKey": "your-api-key-here"
    },
    "pinHash": "$2a$12$...",
    "video": {
        "mode": "PerCamera",
        "ffmpegPath": "",
        "quality": "medium"
    },
    "layout": {
        "mode": "Auto",
        "rows": 0,
        "cols": 0,
        "cameras": []
    },
    "appearance": {
        "theme": "System",
        "fullscreen": true,
        "kiosk": false
    }
}
```

## Settings Reference

### UNVR Connection

| Field | Type | Default | Description |
|-------|------|---------|-------------|
| `unvr.host` | QString | — | UNVR IP address or hostname |
| `unvr.port` | int | `443` | UNVR port |
| `unvr.tlsMode` | TLSMode | `Verify` | `Verify`, `Skip`, or `Fingerprint` |
| `unvr.tlsFingerprint` | QString | — | SHA-256 fingerprint of UNVR cert |
| `unvr.apiKey` | QString | — | UniFi Protect API key |

### PIN

| Field | Type | Description |
|-------|------|-------------|
| `pinHash` | QString | bcrypt hash of 6-digit PIN |

### Video

| Field | Type | Default | Description |
|-------|------|---------|-------------|
| `video.mode` | VideoMode | `PerCamera` | `PerCamera`, `Composite`, or `Snapshot` |
| `video.ffmpegPath` | QString | `` | Custom FFmpeg path (empty = use PATH) |
| `video.quality` | QString | `medium` | Default quality: `high`, `medium`, `low` |

### Layout

| Field | Type | Default | Description |
|-------|------|---------|-------------|
| `layout.mode` | LayoutMode | `Auto` | `Auto` or `Custom` |
| `layout.rows` | int | `0` | Custom rows (ignored if mode=Auto) |
| `layout.cols` | int | `0` | Custom cols (ignored if mode=Auto) |
| `layout.cameras` | QStringList | — | Ordered list of camera IDs to display |

### Appearance

| Field | Type | Default | Description |
|-------|------|---------|-------------|
| `appearance.theme` | ThemeMode | `System` | `System`, `Light`, or `Dark` |
| `appearance.fullscreen` | bool | `true` | Start in fullscreen mode |
| `appearance.kiosk` | bool | `false` | Kiosk mode (no decorations, prevent close) |

## Enum Values

### VideoMode
- `PerCamera` — one FFmpeg subprocess per camera
- `Composite` — single FFmpeg with filter_complex grid
- `Snapshot` — HTTP snapshot polling

### TLSMode
- `Verify` — standard TLS verification (default)
- `Skip` — skip certificate verification
- `Fingerprint` — verify against stored SHA-256 fingerprint

### LayoutMode
- `Auto` — calculate optimal grid from camera count + screen aspect
- `Custom` — use user-specified rows/cols

### ThemeMode
- `System` — follow system theme
- `Light` — force light theme
- `Dark` — force dark theme

## Migration

Config version field (`version`) enables future migrations. `AppConfig::fromJson()` checks version and applies transformations as needed:

```cpp
AppConfig AppConfig::fromJson(const QJsonObject &obj) {
    AppConfig config;
    config.version = obj.value("version", 0).toInt();

    // Apply version-specific migrations
    switch (config.version) {
    case 1:
        // Current schema
        break;
    case 0:
        // Legacy schema migration
        config.version = 1;
        // ... migration logic ...
        break;
    default:
        qWarning() << "Unsupported config version:" << config.version;
    }

    // Parse fields
    // ...
    return config;
}
```
