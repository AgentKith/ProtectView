# Build

## Prerequisites

### Linux Desktop / Raspberry Pi

- **CMake** 3.24+
- **Qt6** (Core, Gui, Widgets, Network, Multimedia, Test, OpenGLWidgets, Svg)
- **OpenSSL** (development headers)
- **FFmpeg** (installed and in PATH)
- **libX11** (development headers)
- **C++20 compiler** (GCC 11+ or Clang 14+)

```bash
# Ubuntu/Debian
sudo apt install cmake g++ build-essential \
  qt6-base-dev qt6-multimedia-dev qt6-svg-dev \
  libssl-dev libx11-dev \
  ffmpeg

# Raspberry Pi OS
sudo apt install cmake g++ build-essential \
  qt6-base-dev qt6-multimedia-dev qt6-svg-dev \
  libssl-dev libx11-dev \
  ffmpeg
```

### Verify Installation

```bash
cmake --version    # 3.24 or later
g++ --version      # 11+ or clang 14+
ffmpeg -version    # 4.2 or later
```

## Build

```bash
# Clone
git clone <repo-url>
cd ProtectView

# Configure
cmake -B build

# Build
cmake --build build

# Run
./build/src/protectview
```

## Build Flags

```bash
# Release build
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build

# Debug build
cmake -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build

# With code coverage
cmake -B build -DCMAKE_BUILD_TYPE=Debug -DENABLE_COVERAGE=ON
cmake --build build
```

## Run

```bash
# First run (setup wizard)
./build/src/protectview

# Kiosk mode
./build/src/protectview --kiosk

# Setup kiosk (requires sudo)
sudo ./build/src/protectview --setup-kiosk

# Undo kiosk (requires sudo)
sudo ./build/src/protectview --undo-kiosk
```

## Pi Deployment

### One-Time Setup

```bash
# Install dependencies
sudo apt update
sudo apt install cmake g++ build-essential \
  qt6-base-dev qt6-multimedia-dev qt6-svg-dev \
  libssl-dev libx11-dev ffmpeg

# Clone and build
git clone <repo-url>
cd ProtectView
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build

# Run first time (setup wizard)
./build/src/protectview
```

### Auto-Start (Systemd)

```bash
sudo tee /etc/systemd/system/ProtectView.service << 'EOF'
[Unit]
Description=UNVR Camera Monitor
After=graphical-session.target

[Service]
Type=simple
User=pi
ExecStart=/home/pi/ProtectView/build/src/protectview
Restart=on-failure
RestartSec=5

[Install]
WantedBy=graphical-session.target
EOF

sudo systemctl enable ProtectView
sudo systemctl start ProtectView
```

### Kiosk Mode

Full lockdown: app is the only thing running, no TTY access, auto-restarts on exit.

**Setup** (one-time, requires sudo):

```bash
sudo ./build/src/protectview --setup-kiosk
```

This creates a `unvr-kiosk` user, configures autologin, disables TTY, and sets up a systemd guard. Reboot to enter kiosk mode.

**Undo** (requires sudo):

```bash
sudo ./build/src/protectview --undo-kiosk
```

Reverses all changes, removes kiosk user, restores normal boot.

**Manual setup** (equivalent to --setup-kiosk):

```bash
# Install dependencies
sudo apt install lightdm openbox x11-xserver-utils

# Create kiosk user
sudo useradd -m -s /usr/sbin/nologin unvr-kiosk

# Configure LightDM autologin
sudo tee /etc/lightdm/lightdm.conf.d/99-kiosk.conf << 'EOF'
[Seat:*]
autologin-user=unvr-kiosk
autologin-user-timeout=0
user-session=openbox
EOF

# Disable TTY
sudo sed -i 's/NAutoVTs=.*/NAutoVTs=0/' /etc/systemd/logind.conf
sudo sed -i 's/ReserveVT=.*/ReserveVT=0/' /etc/systemd/logind.conf

# Create openbox autostart
sudo mkdir -p /home/unvr-kiosk/.config/openbox
sudo tee /home/unvr-kiosk/.config/openbox/autostart << 'EOF'
xrandr --output HDMI-1 --mode 1920x1080
ProtectView --kiosk
EOF
sudo chown -R unvr-kiosk:unvr-kiosk /home/unvr-kiosk/.config

# Create systemd service
sudo tee /etc/systemd/system/ProtectView.service << 'EOF'
[Unit]
Description=UNVR Camera Monitor Kiosk
After=graphical.target

[Service]
Type=simple
User=unvr-kiosk
ExecStart=/usr/local/bin/protectview --kiosk
Restart=always
RestartSec=1

[Install]
WantedBy=graphical.target
EOF

sudo systemctl enable ProtectView
sudo systemctl enable lightdm
```

## Testing

```bash
cd build
ctest --output-on-failure
```

Tests use Qt6 Test with offscreen platform (`QT_QPA_PLATFORM=offscreen`), set automatically via test properties in `tests/CMakeLists.txt`. No display server needed.

### Code Coverage

Build with coverage enabled, then run tests:

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Debug -DENABLE_COVERAGE=ON
cmake --build build
cd build && ctest --output-on-failure
```

## Troubleshooting

### FFmpeg Not Found

```bash
# Check if FFmpeg is in PATH
which ffmpeg

# If not, specify path in settings or install:
sudo apt install ffmpeg
```

### Qt6 Not Found

```bash
# Install Qt6 development packages
sudo apt install qt6-base-dev qt6-multimedia-dev qt6-svg-dev

# If CMake can't find Qt6, set CMAKE_PREFIX_PATH:
cmake -B build -DCMAKE_PREFIX_PATH=/usr/lib/x86_64-linux-gnu/cmake/Qt6
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
