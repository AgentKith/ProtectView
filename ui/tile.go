package ui

import (
	"image"
	"time"

	"fyne.io/fyne/v2"
	"fyne.io/fyne/v2/canvas"
	"fyne.io/fyne/v2/container"
	"fyne.io/fyne/v2/layout"
	"fyne.io/fyne/v2/widget"
)

// CameraTile represents a single camera tile in the grid.
type CameraTile struct {
	container *fyne.Container
	image     *canvas.Image
	label     *widget.Label
	cameraID  string
	cameraName string
	frameHandler func(*image.RGBA)
}

// NewCameraTile creates a new camera tile.
func NewCameraTile(cameraID, cameraName string) *CameraTile {
	tile := &CameraTile{
		cameraID:   cameraID,
		cameraName: cameraName,
	}

	tile.image = canvas.NewImageFromResource(nil)
	tile.image.FillMode = canvas.ImageFillOriginal
	tile.image.SetMinSize(fyne.NewSize(320, 180))

	tile.label = widget.NewLabel(cameraName)
	tile.label.Alignment = fyne.TextAlignCenter

	// Overlay container for camera name
	overlay := container.New(layout.NewGridLayout(1), tile.label)
	overlay.Move(fyne.NewPos(0, 0))

	// Main container with image and overlay
	tile.container = container.New(layout.NewMaxLayout(), tile.image, overlay)

	return tile
}

// SetFrameHandler sets the callback for frame updates.
func (t *CameraTile) SetFrameHandler(handler func(*image.RGBA)) {
	t.frameHandler = handler
}

// UpdateFrame updates the tile with a new frame.
func (t *CameraTile) UpdateFrame(img *image.RGBA) {
	if t.frameHandler != nil {
		t.frameHandler(img)
	}

	t.image.Refresh()
}

// ShowError displays an error state.
func (t *CameraTile) ShowError() {
	bg := canvas.NewRectangle(colorFromHex(ColorCardDark))
	bg.SetMinSize(fyne.NewSize(320, 180))

	errorLabel := widget.NewLabelWithStyle("Connection Lost", fyne.TextAlignCenter, fyne.TextStyle{Bold: true})

	retryLabel := widget.NewLabel("Retrying...")
	retryLabel.Alignment = fyne.TextAlignCenter

	content := container.New(layout.NewVBoxLayout(),
		errorLabel,
		retryLabel,
	)

	t.container.Objects = []fyne.CanvasObject{bg, content}
	t.container.Refresh()
}

// StartRetryAnimation starts a pulsing retry animation.
func (t *CameraTile) StartRetryAnimation() {
	go func() {
		for {
			time.Sleep(1 * time.Second)
			t.container.Refresh()
		}
	}()
}

// Object returns the underlying fyne.CanvasObject.
func (t *CameraTile) Object() fyne.CanvasObject {
	return t.container
}
