#!/bin/bash
set -eo pipefail

APP_NAME="ProtectView"
APP_USER="unvr-kiosk"
APP_BIN="${1:-/usr/local/bin/protectview}"

if [ "$(id -u)" -ne 0 ]; then
    echo "Error: This script must be run as root" >&2
    exit 1
fi

# Create kiosk user
if ! id "$APP_USER" &>/dev/null; then
    useradd -r -s /usr/sbin/nologin "$APP_USER"
    echo "Created user $APP_USER"
fi

# Install binary
if [ -f "$APP_BIN" ]; then
    chown root:root "$APP_BIN"
    chmod 755 "$APP_BIN"
fi

# Create systemd service
cat > /etc/systemd/system/${APP_NAME}.service <<EOF
[Unit]
Description=${APP_NAME} Kiosk
After=graphical.target

[Service]
Type=simple
User=${APP_USER}
ExecStart=${APP_BIN} --kiosk
Restart=always
RestartSec=5
Environment=DISPLAY=:0
Environment=XAUTHORITY=/home/${APP_USER}/.Xauthority

[Install]
WantedBy=graphical.target
EOF

systemctl daemon-reload
systemctl enable ${APP_NAME}.service

# Configure LightDM autologin
if [ -f /etc/lightdm/lightdm.conf ]; then
    cat > /etc/lightdm/lightdm.conf <<EOF
[Seat:*]
autologin-user=${APP_USER}
autologin-user-timeout=0
user-session=default
EOF
    echo "Configured LightDM autologin"
fi

# Disable TTY access for kiosk user
if [ -f /etc/securetty ]; then
    : > /etc/securetty
    echo "Disabled TTY access"
fi

echo "Kiosk setup complete"
