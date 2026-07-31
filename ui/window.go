package ui

import (
	"fyne.io/fyne/v2"
	"fyne.io/fyne/v2/app"
	"fyne.io/fyne/v2/container"
	"fyne.io/fyne/v2/layout"
)

// Window represents the main application window.
type Window struct {
	window  fyne.Window
	grid    *fyne.Container
	cameras []CameraConfig
}

// CameraConfig holds configuration for a single camera tile.
type CameraConfig struct {
	ID   string
	Name string
}

// NewWindow creates a new main window.
func NewWindow(cameras []CameraConfig) *Window {
	a := app.New()
	w := a.NewWindow("UNVR Carousal")
	w.SetMaster()
	w.Resize(fyne.NewSize(1920, 1080))

	// Apply custom theme
	a.Settings().SetTheme(&CustomTheme{})

	// Create grid layout
	grid := createGrid(cameras)

	w.SetContent(container.New(layout.NewBorderLayout(nil, nil, nil, nil), grid))

	return &Window{
		window:  w,
		grid:    grid,
		cameras: cameras,
	}
}

// Show displays the window.
func (w *Window) Show() {
	w.window.ShowAndRun()
}

// SetFullscreen toggles fullscreen mode.
func (w *Window) SetFullscreen(fullscreen bool) {
	w.window.SetFullScreen(fullscreen)
}

// createGrid creates a grid of camera tiles.
func createGrid(cameras []CameraConfig) *fyne.Container {
	rows, _ := calculateGrid(len(cameras))
	gridLayout := layout.NewGridLayout(rows)

	var objects []fyne.CanvasObject
	for _, cam := range cameras {
		tile := NewCameraTile(cam.ID, cam.Name)
		objects = append(objects, tile.Object())
	}

	return container.New(gridLayout, objects...)
}

// calculateGrid calculates optimal rows and columns for the grid.
func calculateGrid(count int) (int, int) {
	if count <= 0 {
		return 1, 1
	}

	// Target aspect ratio 16:9
	aspect := 16.0 / 9.0

	bestRows := 1
	bestCols := count
	bestDiff := float64(bestCols) / float64(bestRows) - aspect

	for rows := 1; rows <= count; rows++ {
		cols := (count + rows - 1) / rows
		diff := float64(cols) / float64(rows) - aspect
		if diff < 0 {
			diff = -diff
		}
		if diff < bestDiff {
			bestDiff = diff
			bestRows = rows
			bestCols = cols
		}
	}

	return bestRows, bestCols
}
