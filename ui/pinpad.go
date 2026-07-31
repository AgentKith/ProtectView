package ui

import (
	"fyne.io/fyne/v2"
	"fyne.io/fyne/v2/container"
	"fyne.io/fyne/v2/layout"
	"fyne.io/fyne/v2/widget"
)

// PINPad represents the PIN entry pad.
type PINPad struct {
	container *fyne.Container
	buttons   []*widget.Button
	dots      []*widget.Label
	current   string
	onSubmit  func(string)
	onCancel  func()
}

// NewPINPad creates a new PIN pad.
func NewPINPad(onSubmit func(string), onCancel func()) *PINPad {
	pad := &PINPad{
		onSubmit: onSubmit,
		onCancel: onCancel,
	}

	// Create 10 buttons (0-9)
	for i := 0; i < 10; i++ {
		num := i
		btn := widget.NewButton(string(rune('0'+num)), func() {
			pad.addDigit(string(rune('0'+num)))
		})
		pad.buttons = append(pad.buttons, btn)
	}

	// Create 6 dots for PIN display
	for i := 0; i < 6; i++ {
		dot := widget.NewLabel("○")
		dot.Alignment = fyne.TextAlignCenter
		pad.dots = append(pad.dots, dot)
	}

	// Submit and Cancel buttons
	submitBtn := widget.NewButton("Submit", func() {
		pad.onSubmit(pad.current)
	})
	cancelBtn := widget.NewButton("Cancel", pad.onCancel)

	// Layout
	var buttonObjects []fyne.CanvasObject
	for _, btn := range pad.buttons {
		buttonObjects = append(buttonObjects, btn)
	}
	buttonGrid := container.New(layout.NewGridLayout(5), buttonObjects...)

	var dotObjects []fyne.CanvasObject
	for _, dot := range pad.dots {
		dotObjects = append(dotObjects, dot)
	}
	dotRow := container.New(layout.NewHBoxLayout(), dotObjects...)

	pad.container = container.New(layout.NewVBoxLayout(),
		widget.NewLabel("Enter PIN"),
		dotRow,
		buttonGrid,
		container.New(layout.NewHBoxLayout(), cancelBtn, submitBtn),
	)

	return pad
}

// addDigit adds a digit to the current PIN.
func (p *PINPad) addDigit(digit string) {
	if len(p.current) < 6 {
		p.current += digit
		p.updateDots()
	}
}

// updateDots updates the PIN display.
func (p *PINPad) updateDots() {
	for i, dot := range p.dots {
		if i < len(p.current) {
			dot.SetText("●")
		} else {
			dot.SetText("○")
		}
	}
	p.container.Refresh()
}

// Reset clears the current PIN.
func (p *PINPad) Reset() {
	p.current = ""
	p.updateDots()
}

// Object returns the underlying fyne.CanvasObject.
func (p *PINPad) Object() fyne.CanvasObject {
	return p.container
}
