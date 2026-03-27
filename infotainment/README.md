# 🚗 InfoDrive HMI — Qt Infotainment Simulator

A full **center-console HMI infotainment system** built in C++17 + Qt Widgets.  
1280×720 frameless window, dark automotive theme, animated transitions, live notifications.

---

## Screens

| Screen        | Features |
|---------------|----------|
| 🏠 **Home**       | Greeting, live clock, quick-access tiles (Media/Nav/Climate/Phone/Battery), weather widget with 4-day forecast, Now Playing strip, Recent Destinations |
| 🎵 **Media**      | Source tabs (Library / Radio / Podcast / BT), animated album art with glow shadow, seek slider, play/pause/skip, volume control, scrollable queue |
| 🗺️ **Navigation** | Procedurally drawn animated city map, bezier route with cyan glow, animated car icon, POI markers, ETA panel, turn card, speed-limit sign, zoom/compass overlay |
| ❄️ **Climate**    | Custom arc-dial per zone (driver + passenger), scroll-wheel temp adjust, fan speed selector (1–5), airflow mode buttons, animated airflow bars, sync/heated-seats toggles |
| 📱 **Phone**      | BT status card, contacts list, full dialpad, call/end-call with live timer, recent calls panel |
| ⚡ **Apps**       | 4×4 app launcher grid with color-coded cards, recently-used chips strip |
| ⚙️ **Settings**   | Left-nav sub-pages: Display (brightness/theme/fonts), Sound (5 volume sliders + profile), Connectivity (BT/Wi-Fi/CarPlay), Vehicle (drive mode/safety), About (VIN/versions/update) |

**Notifications:** toast overlay auto-shows for BT connect, traffic, now-playing events.

---

## Build Requirements

- **Qt 5.15+** or **Qt 6.x** — Widgets module only
- **C++17** compiler (GCC 9+, Clang 11+, MSVC 2019+)
- **CMake 3.16+** or **qmake**

---

## Build

### Qt Creator (easiest — 30 seconds)
1. Open `InfoDrive.pro` (or `CMakeLists.txt`)
2. Select your Kit
3. Press **Ctrl+R**

### qmake CLI
```bash
cd infotainment
qmake InfoDrive.pro
make -j$(nproc)
./InfoDrive
```

### CMake CLI
```bash
cd infotainment
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j$(nproc)
./build/InfoDrive
```

---

## Architecture

```
infotainment/
├── InfoDrive.pro / CMakeLists.txt
└── src/
    ├── main.cpp
    ├── HMIWindow          ← App shell, screen routing, notification dispatch
    ├── TopBar             ← Clock, status icons, close/home buttons
    ├── SideBar            ← Vertical icon nav with active indicator bar
    ├── NotificationOverlay← Animated toast popup (fade in/out)
    ├── HomeScreen         ← Dashboard hub with quick tiles & widgets
    ├── MediaScreen        ← Full music player with source tabs
    ├── NavigationScreen   ← Animated procedural map + route
    ├── ClimateScreen      ← Custom TempDial widget + controls
    ├── PhoneScreen        ← Dialpad + contacts + recent calls
    ├── AppsScreen         ← App launcher grid + recently used
    └── SettingsScreen     ← Multi-panel settings with left nav
```

### Key Technical Notes

- **TempDial** is a custom `QWidget` subclass using `QPainter` arcs, animated tip glow, and `QWheelEvent` for scroll-to-adjust temperature. Uses `Q_PROPERTY` for future animation support.
- **NavigationScreen** runs a 25fps `QTimer`, draws a full city grid, buildings, parks, bezier-smoothed route with layered glow, and animates a car polygon along the route. All geometry is procedural — no image assets.
- **NotificationOverlay** uses `QPropertyAnimation` on a custom `opacity` `Q_PROPERTY` for smooth fade-in/fade-out, and auto-dismisses after 3.5 seconds.
- **SettingsScreen** builds reusable `makeToggleRow`, `makeSliderRow`, `makeSelectRow` helpers to assemble each sub-panel cleanly with `QScrollArea` for overflow.
- The entire UI runs at **1280×720** (standard automotive HMI resolution) with no external assets — pure Qt Widgets + QPainter.

---

*No HTML. No QML. No external assets. Pure C++ Qt Widgets.*
