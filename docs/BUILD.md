# Build

## Prerequisites

### Linux Desktop / Raspberry Pi

- **Go** 1.21+
- **FFmpeg** (installed and in PATH)
- **C compiler** (gcc or clang, for CGO if needed)

```bash
# Ubuntu/Debian
sudo apt install golang ffmpeg build-essential

# Raspberry Pi OS
sudo apt install golang ffmpeg build-essential
```

### Verify Installation

```bash
go version    # go1.21.0 or later
ffmpeg -version  # 4.2 or later
gcc --version  # any version
```

## Build

```bash
# Clone
git clone <repo-url>
cd unvr-carousal

# Download dependencies
go mod tidy

# Build
go build -o unvr-carousal .

# Run
./unvr-carousal
```

## Build Flags

```bash
# Release build (strip debug info, smaller binary)
go build -ldflags="-s -w" -o unvr-carousal .

# Raspberry Pi (cross-compile from x86_64)
GOARCH=arm GOARM=6 GOOS=linux go build -o unvr-carousal-pi .

# Raspberry Pi 64-bit
GOARCH=arm64 GOOS=linux go build -o unvr-carousal-pi64 .
```

## Run

```bash
# First run (setup wizard)
./unvr-carousal

# With custom config path
CONFIG_DIR=/custom/path ./unvr-carousal

# Windowed mode (for testing)
./unvr-carousal --windowed

# Fullscreen (default)
./unvr-carousal --fullscreen
```

## Pi Deployment

### One-Time Setup

```bash
# Install dependencies
sudo apt update
sudo apt install golang ffmpeg build-essential

# Clone and build
git clone <repo-url>
cd unvr-carousal
go build -ldflags="-s -w" -o unvr-carousal .

# Create config directory
mkdir -p ~/.config/unvr-carousal

# Run first time (setup wizard)
./unvr-carousal
```

### Auto-Start (Systemd)

```bash
sudo tee /etc/systemd/system/unvr-carousal.service << 'EOF'
[Unit]
Description=UNVR Carousal
After=graphical-session.target

[Service]
Type=simple
User=pi
ExecStart=/home/pi/unvr-carousal/unvr-carousal --fullscreen
Restart=on-failure
RestartSec=5

[Install]
WantedBy=graphical-session.target
EOF

sudo systemctl enable unvr-carousal
sudo systemctl start unvr-carousal
```

### Kiosk Mode

For headless Pi with no desktop environment:

```bash
# Install minimal X11 + window manager
sudo apt install xserver-xorg x11-xserver-utils openbox

# Create autostart
mkdir -p ~/.config/openbox
tee ~/.config/openbox/autostart << 'EOF'
# Fullscreen, no decorations
xrandr --output HDMI-1 --mode 1920x1080
unvr-carousal --fullscreen --kiosk
EOF

# Start X on boot
@xinit -- -nocursor
```

## Troubleshooting

### FFmpeg Not Found

```bash
# Check if FFmpeg is in PATH
which ffmpeg

# If not, specify path in settings or install:
sudo apt install ffmpeg
```

### RTSPS Connection Failed

- Verify UNVR host and port in settings
- Check TLS settings (skip verification or fingerprint)
- Verify API key has camera access

### High CPU Usage

- Switch to composite FFmpeg mode (one process)
- Reduce camera quality in settings
- Use hardware acceleration: ensure FFmpeg has VA-API/V4L2 support

### Black Tiles

- Check FFmpeg output: `ffmpeg -i rtsps://... -f null -`
- Verify stream URLs from UNVR API
- Check network connectivity to UNVR
