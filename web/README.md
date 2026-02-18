# Blades of Exile - WebAssembly Port

The WASM port compiles the full Blades of Exile game to run in a web browser using Emscripten.

## Status

All core systems are working:
- **Rendering** - Canvas 2D via SFML compatibility layer
- **Input** - Mouse and keyboard via browser event bridge
- **Audio** - Web Audio API sound effects
- **Save/Load** - IndexedDB persistent storage
- **Dialogs** - Full dialog system with ASYNCIFY blocking loops
- **Scenarios** - All base scenarios playable

See [TODO.md](./TODO.md) for remaining limitations and future work.

## How to Build

See [BUILD_WEB.md](../BUILD_WEB.md) in the repo root.

Quick start:
```bash
# Dev build (fast compile)
bash build_wasm_dev.sh

# Prod build (optimized, for GitHub Pages)
bash build_wasm_prod.sh
```

## Architecture

### Compatibility Layer
Rather than a full platform abstraction, the port uses compat headers that stub out SFML
and Boost APIs using C++17 stdlib and Emscripten APIs:

| Header | Replaces |
|--------|---------|
| `src/compat/graphics.hpp` | `SFML/Graphics.hpp` - Canvas 2D rendering |
| `src/compat/audio.hpp` | `SFML/Audio.hpp` - Web Audio API |
| `src/compat/event.hpp` | `SFML/Window.hpp` events |
| `src/compat/time.hpp` | `sf::Time`, `sf::Clock` |
| `src/compat/texture.hpp` | `sf::Texture` - JS Image loading |
| `src/compat/boost_compat.hpp` | Boost string/optional/lexical_cast |
| `src/compat/fmt_compat.hpp` | `fmt::format` |

### Event Bridge
Browser events → C++ game loop:
- `web/events.js` - captures browser events, calls C++ via `Module.ccall`
- `web/web_stubs.cpp` - `push_mouse_event()` / `push_key_event()` exports, event queue
- `src/compat/graphics.hpp` - `sf::RenderWindow::pollEvent()` dequeues events

### Save/Load
- **Save**: Ctrl+S → `do_save()` writes to Emscripten MEMFS → JS syncs to IndexedDB
- **Load**: Ctrl+O intercepted in `events.js` → file dialog → writes to MEMFS → `wasm_load_from_path()` called
- Both flows bypass ASYNCIFY to avoid nested sleep call issues

### ASYNCIFY
ASYNCIFY is enabled for blocking dialog loops. Critical constraint: **never nest ASYNCIFY
sleep calls**. All `sf::sleep()` calls and dialogs in game logic are guarded with
`#ifndef __EMSCRIPTEN__`. See MEMORY.md for details.

## Web-specific Files

| File | Purpose |
|------|---------|
| `web_stubs.cpp` | Platform stub implementations |
| `boe.menus.wasm.cpp` | WASM-specific minimal menu bar |
| `events.js` | Browser event capture and routing |
| `savemanager.js` | IndexedDB save slot management |
| `filedialog.js` / `filedialog.css` | Custom file open/save dialog |
| `shell.html` | Emscripten HTML shell with splash screen |
| `TODO.md` | Remaining work items |
