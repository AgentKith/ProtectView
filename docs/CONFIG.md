# Config

## File Location

```
~/.config/unvr-carousal/
├── config.enc    # AES-256-GCM encrypted JSON
└── .salt         # 16-byte random salt (hex-encoded)
```

## Encryption

### Algorithm

- **Cipher**: AES-256-GCM
- **Key derivation**: argon2id
- **Key length**: 32 bytes (256 bits)
- **Nonce**: 12 bytes, random per encryption

### Key Derivation

```
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
| Android | `Settings.Secure.ANDROID_ID` | `9774d56d682e549c` |

### Salt Generation

On first run, generate 16 random bytes, save as hex-encoded string in `.salt` file.

## Config Schema

### Plaintext JSON (before encryption)

```json
{
    "version": 1,
    "unvr": {
        "host": "192.168.1.100",
        "port": 443,
        "tls_mode": "fingerprint",
        "tls_fingerprint": "aa:bb:cc:dd:ee:ff:00:11:22:33:44:55:66:77:88:99:aa:bb:cc:dd:ee:ff",
        "api_key": "your-api-key-here"
    },
    "pin": {
        "hash": "$2a$12$..."
    },
    "video": {
        "mode": "per-camera",
        "ffmpeg_path": "",
        "quality": "medium"
    },
    "layout": {
        "mode": "auto",
        "rows": 0,
        "cols": 0,
        "cameras": [
            {
                "id": "camera-uuid-1",
                "name": "Front Door",
                "enabled": true,
                "quality": "high"
            },
            {
                "id": "camera-uuid-2",
                "name": "Back Yard",
                "enabled": true,
                "quality": "medium"
            }
        ]
    },
    "appearance": {
        "theme": "dark",
        "fullscreen": true
    }
}
```

## Settings Reference

### UNVR Connection

| Field | Type | Default | Description |
|-------|------|---------|-------------|
| `unvr.host` | string | — | UNVR IP address or hostname |
| `unvr.port` | int | `443` | UNVR port |
| `unvr.tls_mode` | string | `fingerprint` | `skip` or `fingerprint` |
| `unvr.tls_fingerprint` | string | — | SHA-256 fingerprint of UNVR cert (22 hex bytes, colon-separated) |
| `unvr.api_key` | string | — | UniFi Protect API key |

### PIN

| Field | Type | Description |
|-------|------|-------------|
| `pin.hash` | string | bcrypt hash of 6-digit PIN |

### Video

| Field | Type | Default | Description |
|-------|------|---------|-------------|
| `video.mode` | string | `per-camera` | `per-camera`, `composite`, `snapshot`, `relay` |
| `video.ffmpeg_path` | string | `` | Custom FFmpeg path (empty = use PATH) |
| `video.quality` | string | `medium` | Default quality: `high`, `medium`, `low` |

### Layout

| Field | Type | Default | Description |
|-------|------|---------|-------------|
| `layout.mode` | string | `auto` | `auto` or `custom` |
| `layout.rows` | int | `0` | Custom rows (ignored if mode=auto) |
| `layout.cols` | int | `0` | Custom cols (ignored if mode=auto) |
| `layout.cameras` | array | — | Ordered list of camera configs |

### Camera Entry

| Field | Type | Default | Description |
|-------|------|---------|-------------|
| `id` | string | — | UNVR camera UUID |
| `name` | string | — | Camera display name |
| `enabled` | bool | `true` | Whether to show this camera |
| `quality` | string | `medium` | Per-camera quality: `high`, `medium`, `low` |

### Appearance

| Field | Type | Default | Description |
|-------|------|---------|-------------|
| `appearance.theme` | string | `dark` | `dark` or `light` |
| `appearance.fullscreen` | bool | `true` | Start in fullscreen mode |

## Migration

Config version field (`version`) enables future migrations:

```go
func loadConfig() (*Config, error) {
    cfg, err := decryptAndLoad()
    if err != nil {
        return nil, err
    }

    switch cfg.Version {
    case 1:
        return cfg, nil
    case 0:
        // Migrate from v0 to v1
        cfg.Version = 1
        // ... migration logic ...
        return cfg, saveConfig(cfg)
    default:
        return nil, fmt.Errorf("unsupported config version: %d", cfg.Version)
    }
}
```
