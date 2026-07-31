package unvr

import (
	"encoding/json"
	"fmt"
	"io"
	"net/http"
)

// Camera represents a UniFi Protect camera.
type Camera struct {
	ID       string     `json:"id"`
	Name     string     `json:"name"`
	Type     string     `json:"type"`
	State    CameraState `json:"state"`
	Features []string   `json:"features"`
}

// CameraState holds camera state information.
type CameraState struct {
	Recording    bool `json:"recording"`
	MotionEnabled bool `json:"motionEnabled"`
}

// StreamURLs holds RTSPS stream URLs.
type StreamURLs struct {
	URLs      map[string]string `json:"urls"`
	ExpiresAt int64             `json:"expiresAt"`
}

// ListCameras returns all cameras from the UNVR.
func (c *Client) ListCameras() ([]Camera, error) {
	resp, err := c.doRequest(http.MethodGet, "/v1/cameras", nil)
	if err != nil {
		return nil, fmt.Errorf("request cameras: %w", err)
	}
	defer resp.Body.Close()

	if resp.StatusCode != http.StatusOK {
		return nil, fmt.Errorf("list cameras: status %d", resp.StatusCode)
	}

	var cameras []Camera
	if err := json.NewDecoder(resp.Body).Decode(&cameras); err != nil {
		return nil, fmt.Errorf("decode cameras: %w", err)
	}

	return cameras, nil
}

// CreateStream creates an RTSPS stream for a camera.
func (c *Client) CreateStream(cameraID, quality string) (*StreamURLs, error) {
	resp, err := c.doRequest(http.MethodPost, fmt.Sprintf("/v1/cameras/%s/rtsps-stream", cameraID), nil)
	if err != nil {
		return nil, fmt.Errorf("request stream: %w", err)
	}
	defer resp.Body.Close()

	if resp.StatusCode != http.StatusOK {
		return nil, fmt.Errorf("create stream: status %d", resp.StatusCode)
	}

	var urls StreamURLs
	if err := json.NewDecoder(resp.Body).Decode(&urls); err != nil {
		return nil, fmt.Errorf("decode stream urls: %w", err)
	}

	return &urls, nil
}

// GetSnapshot returns a JPEG snapshot of a camera.
func (c *Client) GetSnapshot(cameraID string) ([]byte, error) {
	resp, err := c.doRequest(http.MethodGet, fmt.Sprintf("/v1/cameras/%s/snapshot", cameraID), nil)
	if err != nil {
		return nil, fmt.Errorf("request snapshot: %w", err)
	}
	defer resp.Body.Close()

	if resp.StatusCode != http.StatusOK {
		return nil, fmt.Errorf("get snapshot: status %d", resp.StatusCode)
	}

	return io.ReadAll(resp.Body)
}
