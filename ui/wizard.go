package ui

import (
	"fyne.io/fyne/v2"
	"fyne.io/fyne/v2/container"
	"fyne.io/fyne/v2/layout"
	"fyne.io/fyne/v2/widget"
)

// Wizard represents the setup wizard.
type Wizard struct {
	container *fyne.Container
	currentStep int
	steps     []*widget.Entry
	onNext    func()
	onBack    func()
	onFinish  func()
}

// NewWizard creates a new setup wizard.
func NewWizard(onNext, onBack, onFinish func()) *Wizard {
	wizard := &Wizard{
		onNext:   onNext,
		onBack:   onBack,
		onFinish: onFinish,
	}

	// Step 1: UNVR host
	hostEntry := widget.NewEntry()
	hostEntry.PlaceHolder = "UNVR Host (IP or hostname)"

	// Step 2: API key
	apiKeyEntry := widget.NewEntry()
	apiKeyEntry.PlaceHolder = "API Key"
	apiKeyEntry.Password = true

	// Step 3: Set PIN
	pinEntry := widget.NewEntry()
	pinEntry.PlaceHolder = "Set 6-digit PIN"
	pinEntry.Password = true

	// Step 4: Confirm PIN
	pinConfirmEntry := widget.NewEntry()
	pinConfirmEntry.PlaceHolder = "Confirm PIN"
	pinConfirmEntry.Password = true

	wizard.steps = []*widget.Entry{hostEntry, apiKeyEntry, pinEntry, pinConfirmEntry}
	wizard.currentStep = 0

	// Navigation buttons
	nextBtn := widget.NewButton("Next", wizard.onNext)
	backBtn := widget.NewButton("Back", wizard.onBack)
	finishBtn := widget.NewButton("Finish", wizard.onFinish)

	// Progress indicator
	progress := widget.NewLabel("Step 1 of 4")

	wizard.container = container.New(layout.NewVBoxLayout(),
		progress,
		hostEntry,
		container.New(layout.NewHBoxLayout(), backBtn, nextBtn, finishBtn),
	)

	return wizard
}

// Next moves to the next step.
func (w *Wizard) Next() {
	if w.currentStep < len(w.steps)-1 {
		w.currentStep++
		w.updateStep()
	}
}

// Back moves to the previous step.
func (w *Wizard) Back() {
	if w.currentStep > 0 {
		w.currentStep--
		w.updateStep()
	}
}

// updateStep updates the wizard display.
func (w *Wizard) updateStep() {
	// TODO: Update UI based on current step
	w.container.Refresh()
}

// Object returns the underlying fyne.CanvasObject.
func (w *Wizard) Object() fyne.CanvasObject {
	return w.container
}
