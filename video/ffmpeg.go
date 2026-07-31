package video

import (
	"bufio"
	"fmt"
	"image"
	"io"
	"os/exec"
	"sync"
)

// FFmpegPlayer plays video from an RTSPS URL using FFmpeg.
type FFmpegPlayer struct {
	url        string
	quality    string
	ffmpegPath string
	handler    FrameHandler
	cmd        *exec.Cmd
	cancel     chan struct{}
	mu         sync.Mutex
}

// NewFFmpegPlayer creates a new FFmpeg player.
func NewFFmpegPlayer(url, quality, ffmpegPath string) *FFmpegPlayer {
	if ffmpegPath == "" {
		ffmpegPath = "ffmpeg"
	}

	return &FFmpegPlayer{
		url:        url,
		quality:    quality,
		ffmpegPath: ffmpegPath,
		cancel:     make(chan struct{}),
	}
}

func (p *FFmpegPlayer) Start() error {
	p.mu.Lock()
	defer p.mu.Unlock()

	// Build FFmpeg command
	args := []string{
		"-hide_banner", "-loglevel", "error",
		"-i", p.url,
		"-f", "rawvideo", "-pix_fmt", "rgb24",
		"pipe:1",
	}

	cmd := exec.Command(p.ffmpegPath, args...)
	stdout, err := cmd.StdoutPipe()
	if err != nil {
		return fmt.Errorf("ffmpeg stdout pipe: %w", err)
	}

	if err := cmd.Start(); err != nil {
		return fmt.Errorf("ffmpeg start: %w", err)
	}

	p.cmd = cmd
	go p.readFrames(stdout)
	return nil
}

func (p *FFmpegPlayer) Stop() {
	p.mu.Lock()
	defer p.mu.Unlock()

	close(p.cancel)

	if p.cmd != nil && p.cmd.Process != nil {
		p.cmd.Process.Kill()
		p.cmd.Wait()
	}
}

func (p *FFmpegPlayer) SetFrameHandler(handler FrameHandler) {
	p.handler = handler
}

func (p *FFmpegPlayer) readFrames(stdout io.ReadCloser) {
	defer stdout.Close()

	reader := bufio.NewReader(stdout)
	for {
		select {
		case <-p.cancel:
			return
		default:
		}

		// Read a frame (width * height * 3 bytes for RGB24)
		// For now, we assume a fixed frame size.
		// In practice, we'd parse FFmpeg output to get dimensions.
		buf := make([]byte, 1920*1080*3) // 1080p placeholder
		n, err := io.ReadFull(reader, buf)
		if err != nil {
			return
		}

		frame := buf[:n]
		img := image.NewRGBA(image.Rect(0, 0, 1920, 1080))

		// Convert RGB24 to RGBA
		for i := 0; i < len(frame)/3 && i/3 < len(img.Pix)/4; i += 3 {
			pixIdx := (i / 3) * 4
			img.Pix[pixIdx] = frame[i]     // R
			img.Pix[pixIdx+1] = frame[i+1] // G
			img.Pix[pixIdx+2] = frame[i+2] // B
			img.Pix[pixIdx+3] = 255        // A
		}

		if p.handler != nil {
			p.handler(img)
		}
	}
}

// FindFFmpeg checks if ffmpeg is available in PATH.
func FindFFmpeg() (string, error) {
	path, err := exec.LookPath("ffmpeg")
	if err != nil {
		return "", fmt.Errorf("ffmpeg not found in PATH: %w", err)
	}
	return path, nil
}
