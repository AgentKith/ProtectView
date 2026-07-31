package video

import "image"

// FrameHandler is called with each decoded frame.
type FrameHandler func(img image.Image)

// Player is the interface for video playback backends.
type Player interface {
	// Start begins video playback.
	Start() error

	// Stop terminates video playback.
	Stop()

	// SetFrameHandler sets the callback for decoded frames.
	SetFrameHandler(handler FrameHandler)
}

// Manager manages multiple video players.
type Manager struct {
	players map[string]Player
}

// NewManager creates a new player manager.
func NewManager() *Manager {
	return &Manager{
		players: make(map[string]Player),
	}
}

// Add registers a player for a camera.
func (m *Manager) Add(cameraID string, player Player) {
	m.players[cameraID] = player
}

// Remove removes a player for a camera.
func (m *Manager) Remove(cameraID string) {
	delete(m.players, cameraID)
}

// StartAll starts all registered players.
func (m *Manager) StartAll() {
	for _, p := range m.players {
		_ = p.Start()
	}
}

// StopAll stops all registered players.
func (m *Manager) StopAll() {
	for _, p := range m.players {
		p.Stop()
	}
}

// Get returns the player for a camera.
func (m *Manager) Get(cameraID string) Player {
	return m.players[cameraID]
}
