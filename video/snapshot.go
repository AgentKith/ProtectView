package video

import (
	"bytes"
	"image/jpeg"
	"io"
	"net/http"
	"time"
)

// SnapshotPlayer polls a UNVR endpoint for JPEG snapshots.
type SnapshotPlayer struct {
	url     string
	handler FrameHandler
	stop    chan struct{}
}

// NewSnapshotPlayer creates a snapshot polling player.
func NewSnapshotPlayer(url string) *SnapshotPlayer {
	return &SnapshotPlayer{
		url:  url,
		stop: make(chan struct{}),
	}
}

func (p *SnapshotPlayer) Start() error {
	go p.poll()
	return nil
}

func (p *SnapshotPlayer) Stop() {
	close(p.stop)
}

func (p *SnapshotPlayer) SetFrameHandler(handler FrameHandler) {
	p.handler = handler
}

func (p *SnapshotPlayer) poll() {
	ticker := time.NewTicker(200 * time.Millisecond)
	defer ticker.Stop()

	client := &http.Client{Timeout: 5 * time.Second}

	for {
		select {
		case <-p.stop:
			return
		case <-ticker.C:
			p.fetch(client)
		}
	}
}

func (p *SnapshotPlayer) fetch(client *http.Client) {
	resp, err := client.Get(p.url)
	if err != nil {
		return
	}
	defer resp.Body.Close()

	if resp.StatusCode != http.StatusOK {
		return
	}

	data, err := io.ReadAll(resp.Body)
	if err != nil {
		return
	}

	img, err := jpeg.Decode(bytes.NewReader(data))
	if err != nil {
		return
	}

	if p.handler != nil {
		p.handler(img)
	}
}
