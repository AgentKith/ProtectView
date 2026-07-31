package main

import (
	"flag"
	"fmt"
	"os"

	"fyne.io/fyne/v2"
	"fyne.io/fyne/v2/app"
	"fyne.io/fyne/v2/widget"
)

func main() {
	setupKiosk := flag.Bool("setup-kiosk", false, "Setup kiosk mode")
	undoKiosk := flag.Bool("undo-kiosk", false, "Remove kiosk setup")
	kioskMode := flag.Bool("kiosk", false, "Run in kiosk mode")
	flag.Parse()

	if *setupKiosk {
		if err := SetupKiosk(); err != nil {
			fmt.Fprintf(os.Stderr, "Setup kiosk: %v\n", err)
			os.Exit(1)
		}
		return
	}

	if *undoKiosk {
		if err := UndoKiosk(); err != nil {
			fmt.Fprintf(os.Stderr, "Undo kiosk: %v\n", err)
			os.Exit(1)
		}
		return
	}

	a := app.New()
	w := a.NewWindow("UNVR Carousal")
	w.SetMaster()
	w.Resize(fyne.NewSize(1920, 1080))

	if *kioskMode {
		w.SetFullScreen(true)
	}

	// TODO: Setup fullscreen, camera grid, settings panel
	w.SetContent(widget.NewLabel("UNVR Carousal - Coming Soon"))

	w.ShowAndRun()
}
