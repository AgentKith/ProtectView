# API — UniFi Protect

## Base URL

```
https://{unvr-host}:{port}/proxy/protect/integration
```

Default port: `443`

## Authentication

All requests require `X-API-Key` header:

```
X-API-Key: your-api-key-here
```

API keys are generated in UniFi Protect web interface: Settings > System > API Access.

## Endpoints

### List Cameras

```
GET /v1/cameras
```

**Response**: Array of camera objects (or `{"cameras": [...]}` wrapper).

```json
[
    {
        "id": "camera-uuid-here",
        "name": "Front Door",
        "type": "UVC G3 Flex",
        "state": {
            "recording": true,
            "motionEnabled": true
        },
        "features": ["rtsps", "snapshot"],
        "lastMotion": 1690000000
    }
]
```

**Relevant fields**:
- `id`: Camera UUID, used in other endpoints
- `name`: Display name
- `type`: Camera model
- `features`: Array of supported features (`rtsps`, `snapshot`, etc.)

### Create RTSPS Stream

```
POST /v1/cameras/{id}/rtsps-stream
```

**Request Body**:

```json
{
    "qualities": ["high"]
}
```

**Quality options**: `high`, `medium`, `low`

**Response**:

```json
{
    "high": "rtsps://...",
    "medium": "rtsps://...",
    "low": "rtsps://...",
    "package": "rtsps://..."
}
```

**Notes**:
- URLs are time-limited, typically expire after 24 hours
- URLs contain embedded credentials, use as-is with FFmpeg
- Re-create stream when URL expires

### Get Snapshot

```
GET /v1/cameras/{id}/snapshot
```

**Response**: JPEG image (200 OK)

**Content-Type**: `image/jpeg`

**Notes**:
- Returns latest snapshot from camera
- Suitable for polling (every 200-500ms)
- Resolution matches camera settings

### List NVR

```
GET /v1/nvr
```

**Response**: NVR system information.

```json
{
    "id": "nvr-uuid",
    "name": "UNVR",
    "version": "7.1.87",
    "modelKey": "unvr"
}
```

## Error Responses

| Status | Meaning |
|--------|---------|
| 401 | Invalid or missing API key |
| 403 | API key lacks permission |
| 404 | Camera not found |
| 429 | Rate limited |
| 500 | Internal server error |

## Rate Limiting

- No documented rate limits
- Observe `429` responses and back off
- Snapshot polling: 200-500ms interval recommended

## TLS

UNVR presents a self-signed certificate by default.

Options:
1. **Verify** (default): Standard TLS verification
2. **Skip verification**: Accept any certificate (less secure)
3. **Fingerprint verification**: Compare certificate SHA-256 fingerprint

Get fingerprint:

```bash
echo | openssl s_client -connect {unvr-host}:443 -servername {unvr-host} 2>/dev/null \
    | openssl x509 -outform DER 2>/dev/null \
    | openssl dgst -sha256 -binary \
    | xxd -p -c 256
```
