# Design

## Theme

### Colors

| Element | Dark | Light |
|---------|------|-------|
| Background | `#0a0a0a` | `#f5f5f5` |
| Card surface | `#1a1a1a` | `#ffffff` |
| Foreground text | `#e0e0e0` | `#1a1a1a` |
| Secondary text | `#888888` | `#666666` |
| Accent | `#3b82f6` | `#2563eb` |
| Error | `#ef4444` | `#dc2626` |
| Success | `#22c55e` | `#16a34a` |
| Border | `#2a2a2a` | `#e0e0e0` |
| Hover overlay | `rgba(255,255,255,0.05)` | `rgba(0,0,0,0.03)` |

### Sizes

| Element | Value |
|---------|-------|
| Corner radius (cards) | 12px |
| Corner radius (buttons) | 8px |
| Corner radius (inputs) | 8px |
| Padding (large) | 24px |
| Padding (medium) | 16px |
| Padding (small) | 8px |
| Font size (heading) | 24px |
| Font size (body) | 14px |
| Font size (caption) | 12px |
| Icon size (standard) | 20px |
| Icon size (large) | 24px |
| Menu edge trigger zone | 40px from screen edge |

### Shadows

| Element | Shadow |
|---------|--------|
| Popup menu | `0 4px 24px rgba(0,0,0,0.4)` |
| Settings panel | `0 8px 32px rgba(0,0,0,0.5)` |
| PIN pad | `0 4px 16px rgba(0,0,0,0.3)` |
| Camera tile (error) | None |

## Font

**Inter** — OFL-1.1 license, embedded via `go:embed`.

| Weight | File | Usage |
|--------|------|-------|
| Regular 400 | `Inter-Regular.ttf` | Body text, labels |
| Medium 500 | `Inter-Medium.ttf` | Button text, settings labels |
| Semi Bold 600 | `Inter-SemiBold.ttf` | Section headers, camera names |
| Bold 700 | `Inter-Bold.ttf` | Headings, PIN pad numbers |

Total embedded size: ~400KB.

### Fyne Theme Integration

```go
func (t *theme) Font(style fyne.TextStyle) fyne.Resource {
    switch {
    case style.Bold:
        return interBold
    case style.Italic:
        return interRegular // fallback, no italic weights
    case style.Monospace:
        return interRegular // fallback
    default:
        return interRegular
    }
}
```

## Icons

**Lucide** — ISC license, embedded as SVGs via `go:embed`.

| Icon | Usage |
|------|-------|
| `settings` | Settings button |
| `lock` | PIN pad, security |
| `monitor` | Camera feed, fullscreen |
| `x` | Close, dismiss |
| `check` | Success, confirm |
| `eye` | Show PIN |
| `eye-off` | Hide PIN |
| `layout-grid` | Grid layout |
| `sliders-horizontal` | Video settings |
| `wifi` | Connection status |
| `shield` | TLS/security |
| `refresh-cw` | Retry, reconnect |
| `triangle-alert` | Error, warning |

### SVG Color Injection

Lucide SVGs use `currentColor` for stroke/fill. Inject theme-aware color:

```go
func renderIcon(name string, color color.Color) *canvas.Image {
    svg := embeddedIcons[name]
    colored := strings.ReplaceAll(svg, "currentColor", colorToHex(color))
    img, _ := svgimg.Unmarshal([]byte(colored))
    pic := picture.NewImage(img)
    return canvas.NewImageFromResource(pic)
}
```

## UI Screens

### Main Feed

- Fullscreen grid of camera tiles
- Each tile: `canvas.Image` with camera name overlay (top-left, semi-transparent background)
- Tile spacing: 4px gaps
- Error state: dark tile (`#1a1a1a`), camera name centered, pulsing `refresh-cw` icon below
- Edge hover zone: 40px from screen edges, triggers menu

### Edge-Hover Menu

- Slide-in from screen edge (200ms ease-out)
- Width: 60px, height: full screen
- Items (vertical): settings, fullscreen toggle, layout, exit
- Icons only, no text labels
- Hover highlight: `rgba(255,255,255,0.1)` background
- Dismiss: hover away from edge

### Settings Panel

- Modal popup, centered, 600x500px
- Sections (scrollable):
  - **Connection**: UNVR host, API key, TLS option
  - **Video**: Backend mode, quality per camera
  - **Layout**: Auto/custom grid, camera selection
  - **Appearance**: Theme, fullscreen
  - **Security**: Change PIN
- Close: `x` button (top-right), ESC key

### Setup Wizard

- First-run only, modal, 500x400px
- Steps (progress indicator at top):
  1. **UNVR Host**: Entry field, "Next" button
  2. **API Key**: Entry field, "Next" button
  3. **Set PIN**: Two entry fields (PIN + confirm), "Next" button
  4. **Test Connection**: Loading spinner, success/error result, "Done" button
- Back button on each step

### PIN Pad

- Modal popup, centered, 320x400px
- 16 buttons in 4x4 grid (0-9 + 6 decoy numbers)
- Button size: 64x64px, corner radius 12px
- Button color: `#2a2a2a`, text: `#e0e0e0`
- Hover: `#3a3a3a`
- Press: `#4a4a4a`
- Random positions: decoy buttons mixed with real digits
- Display: 6 dots (filled/empty) above grid
- Shake animation on wrong PIN (200ms, 3 oscillations)
- Close: `x` button, ESC key

### Kiosk Mode

- No window decorations (borderless)
- Fullscreen only, no toggle
- Window close event is ignored (prevents quit)
- Edge-hover menu disabled (no exit option)
- Systemd guard restarts on any exit

## Animations

### Menu Slide-In
- Duration: 200ms
- Easing: ease-out
- Property: X position from -60px to 0px

### PIN Pad Shake
- Duration: 200ms
- Pattern: 3 oscillations, ±8px horizontal
- Trigger: Wrong PIN entered

### Settings Fade-In
- Duration: 150ms
- Easing: ease-out
- Property: Opacity from 0.0 to 1.0

### Retry Icon Pulse
- Duration: 1000ms
- Easing: ease-in-out
- Property: Opacity from 0.3 to 1.0 to 0.3
- Loop: Continuous

### Tile Error Transition
- Duration: 300ms
- Easing: ease-in
- Property: Fade from last frame to dark tile

## Layout Algorithm

### Auto Grid

Calculate optimal rows×cols from camera count and screen aspect ratio:

```go
func calculateGrid(cameras int, screenAspect float64) (rows, cols int) {
    // Target: tiles should be roughly square (16:9 aspect)
    // Screen aspect: width/height
    // Grid aspect: cols/rows should match screen aspect / 16:9

    targetAspect := screenAspect / (16.0 / 9.0)

    for cols := 1; cols <= cameras; cols++ {
        rows = (cameras + cols - 1) / cols // ceiling division
        gridAspect := float64(cols) / float64(rows)
        if math.Abs(gridAspect-targetAspect) < 0.3 {
            return rows, cols
        }
    }
    // Fallback: sqrt
    cols = int(math.Sqrt(float64(cameras)))
    rows = (cameras + cols - 1) / cols
    return rows, cols
}
```

### Common Layouts

| Cameras | Screen 16:9 | Screen 21:9 |
|---------|-------------|-------------|
| 1 | 1x1 | 1x1 |
| 2 | 1x2 | 1x2 |
| 3 | 2x2 | 1x3 |
| 4 | 2x2 | 2x2 |
| 5 | 3x2 | 2x3 |
| 6 | 2x3 | 2x3 |
| 8 | 3x3 | 2x4 |
| 9 | 3x3 | 3x3 |
