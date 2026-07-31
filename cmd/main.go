package main

import (
	"fyne.io/fyne/v2"
	"fyne.io/fyne/v2/app"
	"fyne.io/fyne/v2/widget"
)

func main() {
	a := app.New()
	w := a.NewWindow("UNVR Carousal")
	w.SetMaster()
	w.Resize(fyne.NewSize(1920, 1080))

	// TODO: Setup fullscreen, camera grid, settings panel
	w.SetContent(widget.NewLabel("UNVR Carousal - Coming Soon"))

	w.ShowAndRun()
}
