package ui

import (
	"image/color"
	"fmt"

	"fyne.io/fyne/v2"
	"fyne.io/fyne/v2/theme"
)

// Theme colors
const (
	ColorBgDark       = "#0a0a0a"
	ColorCardDark     = "#1a1a1a"
	ColorTextPrimary  = "#e0e0e0"
	ColorTextSecondary = "#888888"
	ColorAccent       = "#3b82f6"
	ColorError        = "#ef4444"
	ColorSuccess      = "#22c55e"
	ColorWarning      = "#f59e0b"
)

// CustomTheme implements fyne.Theme
type CustomTheme struct{}

func (t *CustomTheme) Color(name fyne.ThemeColorName, variant fyne.ThemeVariant) color.Color {
	switch name {
	case theme.ColorNameBackground:
		return colorFromHex(ColorBgDark)
	case theme.ColorNameButton:
		return colorFromHex(ColorCardDark)
	case theme.ColorNamePlaceHolder:
		return colorFromHex(ColorTextSecondary)
	case theme.ColorNamePrimary:
		return colorFromHex(ColorAccent)
	case theme.ColorNameError:
		return colorFromHex(ColorError)
	default:
		return theme.DefaultTheme().Color(name, variant)
	}
}

func (t *CustomTheme) Font(style fyne.TextStyle) fyne.Resource {
	return theme.DefaultTheme().Font(style)
}

func (t *CustomTheme) Icon(name fyne.ThemeIconName) fyne.Resource {
	return theme.DefaultTheme().Icon(name)
}

func (t *CustomTheme) Size(name fyne.ThemeSizeName) float32 {
	switch name {
	case theme.SizeNameInlineIcon:
		return 24
	case theme.SizeNamePadding:
		return 16
	default:
		return theme.DefaultTheme().Size(name)
	}
}

func colorFromHex(hex string) color.Color {
	r, g, b := parseHex(hex)
	return color.RGBA{R: r, G: g, B: b, A: 255}
}

func parseHex(hex string) (byte, byte, byte) {
	if len(hex) != 7 || hex[0] != '#' {
		return 0, 0, 0
	}
	var r, g, b uint64
	fmt.Sscanf(hex[1:], "%02x%02x%02x", &r, &g, &b)
	return byte(r), byte(g), byte(b)
}
