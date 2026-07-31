package app

import (
	"bufio"
	"os"
	"strings"
)

// GetDeviceFingerprint returns a unique device identifier.
//
// On Raspberry Pi, reads /proc/cpuinfo Serial.
// On Linux desktop, reads /etc/machine-id.
func GetDeviceFingerprint() (string, error) {
	// Try Pi first
	if serial, err := readPiSerial(); err == nil && serial != "" {
		return serial, nil
	}

	// Fallback to machine-id
	return readMachineID()
}

// readPiSerial reads the Serial field from /proc/cpuinfo.
func readPiSerial() (string, error) {
	f, err := os.Open("/proc/cpuinfo")
	if err != nil {
		return "", err
	}
	defer f.Close()

	scanner := bufio.NewScanner(f)
	for scanner.Scan() {
		line := scanner.Text()
		if strings.HasPrefix(line, "Serial") {
			parts := strings.SplitN(line, ":", 2)
			if len(parts) == 2 {
				return strings.TrimSpace(parts[1]), nil
			}
		}
	}
	return "", scanner.Err()
}

// readMachineID reads the machine-id from /etc/machine-id.
func readMachineID() (string, error) {
	data, err := os.ReadFile("/etc/machine-id")
	if err != nil {
		return "", err
	}
	return strings.TrimSpace(string(data)), nil
}
