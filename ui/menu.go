package ui

import (
	"fyne.io/fyne/v2"
	"fyne.io/fyne/v2/container"
	"fyne.io/fyne/v2/layout"
	"fyne.io/fyne/v2/widget"
)

// Menu represents the edge-hover menu.
type Menu struct {
	container *fyne.Container
	visible   bool
	items     []*widget.Button
	onSettings func()
	onFullscreen func()
	onExit func()
}

// NewMenu creates a new edge-hover menu.
func NewMenu(onSettings, onFullscreen, onExit func()) *Menu {
	menu := &Menu{
		onSettings:   onSettings,
		onFullscreen: onFullscreen,
		onExit:       onExit,
	}

	menu.items = []*widget.Button{
		widget.NewButton("Settings", menu.onSettings),
		widget.NewButton("Fullscreen", menu.onFullscreen),
		widget.NewButton("Exit", menu.onExit),
	}

	var objects []fyne.CanvasObject
	for _, item := range menu.items {
		objects = append(objects, item)
	}
	menu.container = container.New(layout.NewVBoxLayout(), objects...)
	menu.container.Hide()

	return menu
}

// Show displays the menu.
func (m *Menu) Show() {
	m.container.Show()
	m.visible = true
}

// Hide hides the menu.
func (m *Menu) Hide() {
	m.container.Hide()
	m.visible = false
}

// Toggle toggles the menu visibility.
func (m *Menu) Toggle() {
	if m.visible {
		m.Hide()
	} else {
		m.Show()
	}
}

// Object returns the underlying fyne.CanvasObject.
func (m *Menu) Object() fyne.CanvasObject {
	return m.container
}
