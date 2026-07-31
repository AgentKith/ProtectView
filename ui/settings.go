package ui

import (
	"fyne.io/fyne/v2"
	"fyne.io/fyne/v2/container"
	"fyne.io/fyne/v2/layout"
	"fyne.io/fyne/v2/widget"
)

// Settings represents the settings panel.
type Settings struct {
	container *fyne.Container
	hostEntry *widget.Entry
	apiKeyEntry *widget.Entry
	tlsSelect *widget.Select
	qualitySelect *widget.Select
	ffmpegPathEntry *widget.Entry
	onSave func()
}

// NewSettings creates a new settings panel.
func NewSettings(onSave func()) *Settings {
	settings := &Settings{
		onSave: onSave,
	}

	// Connection section
	hostEntry := widget.NewEntry()
	hostEntry.PlaceHolder = "UNVR Host"

	apiKeyEntry := widget.NewEntry()
	apiKeyEntry.PlaceHolder = "API Key"
	apiKeyEntry.Password = true

	tlsSelect := widget.NewSelect([]string{"skip", "fingerprint"}, nil)

	// Video section
	qualitySelect := widget.NewSelect([]string{"high", "medium", "low"}, nil)

	ffmpegPathEntry := widget.NewEntry()
	ffmpegPathEntry.PlaceHolder = "FFmpeg Path"

	// Save button
	saveBtn := widget.NewButton("Save", settings.onSave)

	settings.container = container.New(layout.NewVBoxLayout(),
		widget.NewLabel("Connection"),
		hostEntry,
		apiKeyEntry,
		tlsSelect,
		widget.NewLabel("Video"),
		qualitySelect,
		ffmpegPathEntry,
		saveBtn,
	)

	settings.hostEntry = hostEntry
	settings.apiKeyEntry = apiKeyEntry
	settings.tlsSelect = tlsSelect
	settings.qualitySelect = qualitySelect
	settings.ffmpegPathEntry = ffmpegPathEntry

	return settings
}

// Object returns the underlying fyne.CanvasObject.
func (s *Settings) Object() fyne.CanvasObject {
	return s.container
}
