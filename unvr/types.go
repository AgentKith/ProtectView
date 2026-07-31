package unvr

import (
	"encoding/json"
	"fmt"
)

// NVR represents the UniFi NVR system info.
type NVR struct {
	ID      string `json:"id"`
	Name    string `json:"name"`
	Version string `json:"version"`
	Model   string `json:"modelKey"`
}

// GetNVR returns NVR system information.
func (c *Client) GetNVR() (*NVR, error) {
	resp, err := c.doRequest("GET", "/v1/nvr", nil)
	if err != nil {
		return nil, fmt.Errorf("request nvr: %w", err)
	}
	defer resp.Body.Close()

	if resp.StatusCode != 200 {
		return nil, fmt.Errorf("get nvr: status %d", resp.StatusCode)
	}

	var nvr NVR
	if err := json.NewDecoder(resp.Body).Decode(&nvr); err != nil {
		return nil, fmt.Errorf("decode nvr: %w", err)
	}

	return &nvr, nil
}
