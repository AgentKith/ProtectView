package video

import (
	"bytes"
	"fmt"
	"image/jpeg"
	"io"
	"net/http"
	"strings"
	"time"
)

// RelayClient connects to a Pi relay's MJPEG stream.
type RelayClient struct {
	url     string
	handler FrameHandler
	stop    chan struct{}
}

// NewRelayClient creates a new relay client.
func NewRelayClient(url string) *RelayClient {
	return &RelayClient{
		url:  url,
		stop: make(chan struct{}),
	}
}

func (p *RelayClient) Start() error {
	go p.connect()
	return nil
}

func (p *RelayClient) Stop() {
	close(p.stop)
}

func (p *RelayClient) SetFrameHandler(handler FrameHandler) {
	p.handler = handler
}

func (p *RelayClient) connect() {
	client := &http.Client{Timeout: 30 * time.Second}

	for {
		select {
		case <-p.stop:
			return
		default:
		}

		resp, err := client.Get(p.url)
		if err != nil {
			time.Sleep(5 * time.Second)
			continue
		}

		p.parseMJPEG(resp.Body)
		resp.Body.Close()
	}
}

func (p *RelayClient) parseMJPEG(r io.Reader) {
	buf := make([]byte, 4096)
	var frameBuf bytes.Buffer

	for {
		select {
		case <-p.stop:
			return
		default:
		}

		n, err := r.Read(buf)
		if err != nil {
			return
		}

		chunk := buf[:n]
		frameBuf.Write(chunk)

		// Look for JPEG end marker (FF D9)
		data := frameBuf.Bytes()
		for i := 1; i < len(data)-1; i++ {
			if data[i-1] == 0xFF && data[i] == 0xD9 {
				// Complete JPEG frame
				img, err := jpeg.Decode(bytes.NewReader(data[:i+1]))
				if err == nil && p.handler != nil {
					p.handler(img)
				}

				// Find next JPEG start (FF D8)
				rest := data[i+1:]
				nextStart := bytes.Index(rest, []byte{0xFF, 0xD8})
				if nextStart >= 0 {
					frameBuf.Reset()
					frameBuf.Write(rest[nextStart:])
				} else {
					frameBuf.Reset()
				}
				break
			}
		}
	}
}

// RelayServer serves MJPEG streams from FFmpeg.
type RelayServer struct {
	addr    string
	streams map[string]*RelayStream
}

// RelayStream holds a camera's MJPEG stream.
type RelayStream struct {
	// writers are the connected clients
	writers []io.WriteCloser
}

// NewRelayServer creates a new relay server.
func NewRelayServer(addr string) *RelayServer {
	return &RelayServer{
		addr:    addr,
		streams: make(map[string]*RelayStream),
	}
}

// Start begins serving MJPEG streams.
func (s *RelayServer) Start() error {
	mux := http.NewServeMux()
	mux.HandleFunc("/stream/", s.handleStream)

	server := &http.Server{
		Addr:    s.addr,
		Handler: mux,
	}

	go func() {
		if err := server.ListenAndServe(); err != nil && err != http.ErrServerClosed {
			fmt.Printf("relay server error: %v\n", err)
		}
	}()

	return nil
}

func (s *RelayServer) handleStream(w http.ResponseWriter, r *http.Request) {
	cameraID := strings.TrimPrefix(r.URL.Path, "/stream/")
	cameraID = strings.TrimSuffix(cameraID, ".mjpg")

	w.Header().Set("Content-Type", "multipart/x-mixed-replace; boundary=frame")
	// ... serve MJPEG frames
}
