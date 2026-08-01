#!/bin/bash
set -eo pipefail

APP_NAME="ProtectView"
APP_USER="unvr-kiosk"

if [ "$(id -u)" -ne 0 ]; then
    echo "Error: This script must be run as root" >&2
    exit 1
fi

# Disable and remove systemd service
systemctl disable ${APP_NAME}.service 2>/dev/null || true
rm -f /etc/systemd/system/${APP_NAME}.service
systemctl daemon-reload
echo "Removed systemd service"

# Restore LightDM config
if [ -f /etc/lightdm/lightdm.conf ]; then
    rm -f /etc/lightdm/lightdm.conf
    echo "Removed LightDM autologin"
fi

# Restore TTY access
echo "tty1" > /etc/securetty
echo "Restored TTY access"

# Remove kiosk user
if id "$APP_USER" &>/dev/null; then
    userdel -r "$APP_USER" 2>/dev/null || userdel "$APP_USER"
    echo "Removed user $APP_USER"
fi

echo "Kiosk mode undone"
