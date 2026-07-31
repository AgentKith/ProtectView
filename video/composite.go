package video

import (
	"fmt"
	"strings"
)

// CompositePlayer uses a single FFmpeg process with filter_complex to combine cameras.
type CompositePlayer struct {
	urls       []string
	ffmpegPath string
	rows       int
	cols       int
	handler    FrameHandler
	cmd        interface{} // *exec.Cmd
	cancel     chan struct{}
}

// NewCompositePlayer creates a new composite player.
func NewCompositePlayer(urls []string, rows, cols int, ffmpegPath string) *CompositePlayer {
	if ffmpegPath == "" {
		ffmpegPath = "ffmpeg"
	}

	return &CompositePlayer{
		urls:       urls,
		ffmpegPath: ffmpegPath,
		rows:       rows,
		cols:       cols,
		cancel:     make(chan struct{}),
	}
}

func (p *CompositePlayer) Start() error {
	// Build filter_complex
	filter := p.buildFilter()

	args := []string{
		"-hide_banner", "-loglevel", "error",
	}

	// Add inputs
	for _, url := range p.urls {
		args = append(args, "-i", url)
	}

	args = append(args,
		"-filter_complex", filter,
		"-f", "rawvideo", "-pix_fmt", "rgb24",
		"pipe:1",
	)

	// ... start FFmpeg subprocess
	_ = args
	return nil
}

func (p *CompositePlayer) Stop() {
	close(p.cancel)
}

func (p *CompositePlayer) SetFrameHandler(handler FrameHandler) {
	p.handler = handler
}

// buildFilter generates the FFmpeg filter_complex string for a grid layout.
func (p *CompositePlayer) buildFilter() string {
	var sb strings.Builder

	// Scale each input
	for i := range p.urls {
		fmt.Fprintf(&sb, "[%d:v]scale=640:360[out%d]; ", i, i)
	}

	// Stack rows
	for r := 0; r < p.rows; r++ {
		rowInputs := ""
		for c := 0; c < p.cols; c++ {
			idx := r*p.cols + c
			if idx < len(p.urls) {
				rowInputs += fmt.Sprintf("[out%d]", idx)
			}
		}
		fmt.Fprintf(&sb, "%shstack=%d[row%d]; ", rowInputs, p.cols, r)
	}

	// Stack columns
	colInputs := ""
	for r := 0; r < p.rows; r++ {
		colInputs += fmt.Sprintf("[row%d]", r)
	}
	fmt.Fprintf(&sb, "%svstack=%d", colInputs, p.rows)

	return sb.String()
}
