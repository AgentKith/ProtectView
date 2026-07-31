package video

import (
	"testing"
)

// mockPlayer implements Player for testing.
type mockPlayer struct {
	started bool
	stopped bool
	handler FrameHandler
}

func (m *mockPlayer) Start() error {
	m.started = true
	return nil
}

func (m *mockPlayer) Stop() {
	m.stopped = true
}

func (m *mockPlayer) SetFrameHandler(handler FrameHandler) {
	m.handler = handler
}

func TestManagerAddAndGet(t *testing.T) {
	m := NewManager()

	player := &mockPlayer{}
	m.Add("cam1", player)

	got := m.Get("cam1")
	if got != player {
		t.Errorf("Get(cam1) = %v, want %v", got, player)
	}
}

func TestManagerRemove(t *testing.T) {
	m := NewManager()

	player := &mockPlayer{}
	m.Add("cam1", player)
	m.Remove("cam1")

	got := m.Get("cam1")
	if got != nil {
		t.Errorf("Get(cam1) after Remove = %v, want nil", got)
	}
}

func TestManagerGetNonExistent(t *testing.T) {
	m := NewManager()

	got := m.Get("nonexistent")
	if got != nil {
		t.Errorf("Get(nonexistent) = %v, want nil", got)
	}
}

func TestManagerStartAll(t *testing.T) {
	m := NewManager()

	p1 := &mockPlayer{}
	p2 := &mockPlayer{}
	m.Add("cam1", p1)
	m.Add("cam2", p2)

	m.StartAll()

	if !p1.started {
		t.Error("p1 not started")
	}
	if !p2.started {
		t.Error("p2 not started")
	}
}

func TestManagerStopAll(t *testing.T) {
	m := NewManager()

	p1 := &mockPlayer{}
	p2 := &mockPlayer{}
	m.Add("cam1", p1)
	m.Add("cam2", p2)

	m.StopAll()

	if !p1.stopped {
		t.Error("p1 not stopped")
	}
	if !p2.stopped {
		t.Error("p2 not stopped")
	}
}

func TestManagerOverwrite(t *testing.T) {
	m := NewManager()

	p1 := &mockPlayer{}
	p2 := &mockPlayer{}
	m.Add("cam1", p1)
	m.Add("cam1", p2)

	got := m.Get("cam1")
	if got != p2 {
		t.Errorf("Get(cam1) = %v, want %v (should overwrite)", got, p2)
	}
}
