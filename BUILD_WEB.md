# Building Blades of Exile for WebAssembly

## Prerequisites

1. **Emscripten SDK 5.0.0** cloned somewhere on your machine:
   ```bash
   git clone https://github.com/emscripten-core/emsdk.git
   cd emsdk
   ./emsdk install 5.0.0
   ./emsdk activate 5.0.0
   ```
   Then set `EMSDK_ROOT` in `build_wasm_common.sh` (line 9) to the path where you cloned it.
   The build scripts reference the SDK directly (no need to `source emsdk_env.sh`).

2. **Python 3** for running a local test server.

3. **Run from the repo root** - all build scripts must be run from the `cboe/` directory.

---

## Dev Build (fast, with debug symbols)

```bash
bash build_wasm_dev.sh
```

Output in `build/web_game/`:
- `boe.html` - HTML shell with splash screen
- `boe.js` - Emscripten JavaScript glue
- `boe.wasm` - Compiled game (unoptimized)
- `boe.data` - Preloaded game resources
- `events.js`, `savemanager.js`, `filedialog.js`, `filedialog.css`
- `rsrc/cursors/*.gif`

Test it:
```bash
cd build/web_game
python -m http.server 8081
# Open: http://localhost:8081/boe.html
```

---

## Prod Build (optimized, for GitHub Pages)

```bash
bash build_wasm_prod.sh
```

Output in `docs/`:
- `index.html` - Entry point for GitHub Pages
- `boe.js`, `boe.wasm`, `boe.data`
- Web assets and cursors

Test it:
```bash
cd docs
python -m http.server 8082
# Open: http://localhost:8082/index.html
```

Commit `docs/` to deploy to GitHub Pages.

---

## Shared Configuration

Both scripts source `build_wasm_common.sh` which contains:
- Emscripten tool paths
- Full list of 107 source files
- Include paths
- Base compiler flags (WASM, ASYNCIFY, preload-file directives, etc.)

---

## After Rebuilding

Always **hard refresh** the browser to clear cached WASM:
- Windows/Linux: `Ctrl + Shift + R`
- Mac: `Cmd + Shift + R`

---

## Key Source Files

| File | Purpose |
|------|---------|
| `web/web_stubs.cpp` | Platform stubs: event queue, sound, prefs, replay |
| `web/events.js` | Browser → C++ event bridge (mouse, keyboard) |
| `web/savemanager.js` | IndexedDB save/load management |
| `web/shell.html` | Custom Emscripten HTML shell with splash screen |
| `src/compat/graphics.hpp` | SFML → Canvas 2D stub layer |
| `src/compat/audio.hpp` | SFML Audio → Web Audio API stub layer |
| `src/compat/boost_compat.hpp` | Boost → stdlib compatibility |
| `src/compat/fmt_compat.hpp` | fmt::format → variadic template stub |

---

## Debugging

Open browser DevTools (F12) to see console output from C++ (`std::cout` / `std::cerr`).

### Common Issues

**Black screen / nothing loads:**
- Hard refresh (Ctrl+Shift+R)
- Check DevTools console for errors

**"Assertion failed: No EM_ASM constant found":**
- JS and WASM files are out of sync - hard refresh

**Build fails with linker errors:**
- A new `.cpp` file may need to be added to `SOURCES` in `build_wasm_common.sh`

**Resources not found at runtime:**
- Check that the `--preload-file` directives in `build_wasm_common.sh` cover the needed paths

---

## Architecture Notes

The WASM port uses a compatibility header approach rather than full platform abstraction:
- `src/compat/graphics.hpp` replaces SFML graphics with Canvas 2D `EM_ASM_` calls
- `src/compat/audio.hpp` replaces SFML Audio with Web Audio API stubs
- `src/compat/event.hpp` / `src/compat/time.hpp` replace SFML system types

Game logic is otherwise unchanged. See `web/TODO.md` for remaining work items.
