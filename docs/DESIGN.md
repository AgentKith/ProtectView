# Design

## Theme — Neon Touch

**Neon border** aesthetic on dark background. Optimized for 4K touch display (2x linear / 4x area scaling).

### Colors

| Role | Color | Hex | Usage |
|------|-------|-----|-------|
| **Primary** | Glowing orange | `#FF6B00` | Button borders, active states, accents, focus rings |
| **Primary hover** | Bright orange | `#FF8C33` | Hover state |
| **Primary press** | Light orange | `#FFB366` | Pressed state |
| **Secondary** | Electric cyan | `#00D4FF` | Links, secondary actions, edge menu buttons |
| **Secondary hover** | Bright cyan | `#33DDFF` | Hover state |
| **Information** | Cool white | `#E0E0E0` | Body text, labels |
| **Secondary text** | Muted gray | `#888888` | Subtitles, captions |
| **Disabled** | Dim ember | `#6B3A00` | Disabled buttons, inactive states |
| **Background** | Deep black | `#0A0A0A` | App background |
| **Surface** | Dark card | `#1A1A1A` | Dialogs, panels, tiles |
| **Border** | Subtle gray | `#2A2A2A` | Input borders, dividers |
| **Error** | Red | `#EF4444` | Error states, validation |
| **Success** | Green | `#22C55E` | Success states |

### Sizes (Touch-Optimized, 4K)

All sizes are **2x** the original (4x area) for comfortable touch interaction.

| Element | Value |
|---------|-------|
| Corner radius (cards/dialogs) | 24px |
| Corner radius (buttons) | 16px |
| Corner radius (inputs) | 16px |
| Corner radius (PIN buttons) | 24px |
| Padding (large) | 48px |
| Padding (medium) | 32px |
| Padding (small) | 16px |
| Font size (heading) | 48px |
| Font size (body) | 28px |
| Font size (button) | 40px |
| Font size (PIN digits) | 48px |
| Font size (caption) | 24px |
| Icon size (standard) | 40px |
| Icon size (large) | 48px |
| Button min size | 160×80px |
| PIN button size | 160×160px |
| Input min height | 60px |
| Grid spacing | 8px |
| Grid margins | 8px |
| Checkbox indicator | 48×48px |
| Scrollbar width/height | 32px |
| Menu edge trigger zone | 40px from screen edge |

### Dialog Sizes

| Dialog | Size |
|--------|------|
| Settings | min-width: 800px |
| PIN Verify | 640×800px |
| Setup Wizard | min-width: 800px |

## Font

**Plus Jakarta Sans** — SIL Open Font License (OFL), variable font embedded via Qt resources.

| Weight | Usage |
|--------|-------|
| Regular 400 | Body text, labels, inputs |
| Medium 500 | Button text, settings labels, camera names |
| SemiBold 600 | Section headers, camera names (error state) |
| Bold 700 | Headings, PIN pad numbers, PIN display |

Variable font (single file, all weights): ~173KB + 179KB (italic).

### Qt Registration

```cpp
QFontDatabase::addApplicationFont(":/PlusJakartaSans-Variable.ttf");
QFontDatabase::addApplicationFont(":/PlusJakartaSans-Italic-Variable.ttf");

QFont font("Plus Jakarta Sans", 28);
qApp->setFont(font);
```

## Style Sheets

**QSS file**: `src/ui/styles/app.qss` — loaded at startup via `qApp->setStyleSheet()`.

Covers: QWidget, QMainWindow, QDialog, QWizard, QPushButton, QLineEdit, QLabel, QComboBox, QSpinBox, QCheckBox, QFormLayout, QScrollBar.

Special objectNames for targeted styling:
- `#EdgeMenu` — edge overlay menu
- `#edgeButton` — edge menu buttons (cyan theme)
- `#pinDigit` — PIN pad digit buttons (orange, 160×160)
- `#pinAction` — PIN pad action buttons (cyan, 160×160)
- `#pinDisplay` — PIN dot display (48px bold)

Dynamic styles (inline, for runtime state changes):
- Error borders on QLineEdit (red border on validation error)
- Success/error label colors in wizard
- PIN mismatch label

## Icons

**Lucide** — ISC license, embedded as SVGs via Qt resources.

| Icon | Usage |
|------|-------|
| `settings` | Settings button |
| `fullscreen` | Fullscreen toggle |
| `exit` | Exit button |
| `retry` | Retry, reconnect |
| `camera` | Camera icon |
| `check` | Confirmation |
| `clear` | Clear action |
| `enter` | Submit action |
| `chevron-down` | Dropdown indicator |
| `chevron-up` | Dropdown indicator |

## UI Screens

### Main Feed

- Fullscreen grid of camera tiles (`QGridLayout`)
- Each tile: custom `paintEvent` with camera frame or error overlay
- Tile spacing: 8px gaps
- Error state: dark tile (`#1A1A1A`), camera name + error message centered, error color (`#EF4444`), font size 28px SemiBold
- Normal state: camera name centered, info color (`#E0E0E0`), font size 28px Medium
- Edge hover zone: 40px from screen edges, triggers menu

### Edge-Hover Menu

- Overlay panel, top-right corner
- Background: `rgba(10, 10, 10, 230)`, neon orange border, 24px border radius
- Buttons: cyan borders (`#00D4FF`), 36px font, 24px padding
- Auto-hide after 3s
- Items (vertical): settings, exit

### Settings Panel

- Modal dialog, centered, min-width: 800px
- Neon orange border, 24px border radius, 32px padding
- Sections (form layout):
  - **Connection**: UNVR host, API key, TLS option
  - **Video**: Backend mode, quality per camera
  - **Layout**: Auto/custom grid, camera selection
  - **Appearance**: Theme, fullscreen
  - **Security**: Change PIN
- Inputs: 28px font, 60px min height, 16px padding
- Buttons: 40px font, orange borders, 24px padding
- Close: ESC key, Cancel button

### Setup Wizard

- First-run only, modal, min-width: 800px
- Neon orange border, 24px border radius, 48px padding
- Steps (progress indicator at top):
  1. **Connection**: Host, API key, TLS, test connection
  2. **Set PIN**: PIN + confirm, mismatch validation
- Input validation: red border (`#EF4444`) on error, subtle gray (`#2A2A2A`) on valid
- Test result: green (`#22C55E`) for success, red (`#EF4444`) for error
- Back/Next buttons styled with global QPushButton rules

### PIN Pad

- Modal dialog, 640×800px
- 10 digit buttons (0-9) in 3×3 + bottom row grid
- Digit button size: 160×160px, corner radius 24px
- Digit button: `#1A1A1A` background, orange border (`#FF6B00`), 48px Bold font
- Action buttons (Clear, Enter): cyan border (`#00D4FF`), 40px SemiBold font
- Display: 48px Bold, 100px min height, centered dots
- Random positions: shuffled on each creation
- Shake animation on wrong PIN (200ms, 3 oscillations) — TBD
- Close: ESC key

### Kiosk Mode

- No window decorations (borderless)
- Fullscreen only, no toggle
- Window close event is ignored (prevents quit)
- Edge-hover menu disabled (no exit option)
- X11 key grabs: Alt+Tab, PrintScreen, Super key
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

Calculate optimal rows×cols from camera count and screen aspect ratio.

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
