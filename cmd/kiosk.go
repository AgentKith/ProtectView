package main

import (
	"fmt"
	"os"
	"os/exec"
	"strings"
)

const (
	kioskUser = "unvr-kiosk"
	kioskGroup = "unvr-kiosk"
	systemdService = "[Unit]\nDescription=UNVR Carousal Kiosk\nAfter=graphical.target\n\n[Service]\nType=simple\nUser=" + kioskUser + "\nExecStart=/usr/local/bin/unvr-carousal --kiosk\nRestart=always\nRestartSec=5\n\n[Install]\nWantedBy=graphical.target"
)

// SetupKiosk sets up the kiosk environment.
func SetupKiosk() error {
	// Check for root
	if os.Geteuid() != 0 {
		return fmt.Errorf("kiosk setup requires root (use sudo)")
	}

	// Create kiosk user
	if err := createUser(); err != nil {
		return fmt.Errorf("create user: %w", err)
	}

	// Configure LightDM autologin
	if err := configureAutologin(); err != nil {
		return fmt.Errorf("configure autologin: %w", err)
	}

	// Create systemd service
	if err := createSystemdService(); err != nil {
		return fmt.Errorf("create systemd service: %w", err)
	}

	// Enable service
	if err := enableService(); err != nil {
		return fmt.Errorf("enable service: %w", err)
	}

	fmt.Println("Kiosk setup complete. Reboot to apply.")
	return nil
}

// UndoKiosk removes the kiosk setup.
func UndoKiosk() error {
	// Check for root
	if os.Geteuid() != 0 {
		return fmt.Errorf("kiosk undo requires root (use sudo)")
	}

	// Disable and remove systemd service
	if err := removeSystemdService(); err != nil {
		return fmt.Errorf("remove systemd service: %w", err)
	}

	// Remove autologin
	if err := removeAutologin(); err != nil {
		return fmt.Errorf("remove autologin: %w", err)
	}

	// Remove kiosk user
	if err := removeUser(); err != nil {
		return fmt.Errorf("remove user: %w", err)
	}

	fmt.Println("Kiosk setup removed. Reboot to apply.")
	return nil
}

func createUser() error {
	cmd := exec.Command("useradd", "-m", "-s", "/bin/bash", kioskUser)
	output, err := cmd.CombinedOutput()
	if err != nil {
		if strings.Contains(string(output), "already exists") {
			return nil
		}
		return fmt.Errorf("%s: %s", err, output)
	}
	return nil
}

func configureAutologin() error {
	// TODO: Configure LightDM autologin
	return nil
}

func createSystemdService() error {
	return os.WriteFile("/etc/systemd/system/unvr-kiosk.service", []byte(systemdService), 0644)
}

func enableService() error {
	cmd := exec.Command("systemctl", "enable", "unvr-kiosk.service")
	return cmd.Run()
}

func removeSystemdService() error {
	cmd := exec.Command("systemctl", "disable", "--now", "unvr-kiosk.service")
	_ = cmd.Run()

	return os.Remove("/etc/systemd/system/unvr-kiosk.service")
}

func removeAutologin() error {
	// TODO: Remove LightDM autologin
	return nil
}

func removeUser() error {
	cmd := exec.Command("userdel", "-r", kioskUser)
	output, err := cmd.CombinedOutput()
	if err != nil {
		if strings.Contains(string(output), "does not exist") {
			return nil
		}
		return fmt.Errorf("%s: %s", err, output)
	}
	return nil
}
