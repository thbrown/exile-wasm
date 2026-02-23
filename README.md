Blades of Exile — WebAssembly Port
=====================================

This is a WebAssembly fork of [Open Blades of Exile](https://github.com/calref/cboe),
the open-source release of Spiderweb Software's classic RPG creator.
The goal is to make the game playable in a modern browser with no installation required.

**Play in Browser:** https://thbrown.github.io/exile-wasm/

---

Building from Scratch
---------------------

### 1. Clone with submodules

```bash
git clone --recursive https://github.com/<your-fork>/cboe
cd cboe
```

If you already cloned without `--recursive`:

```bash
git submodule update --init
```

### 2. Install Emscripten SDK 5.0.0

```bash
git clone https://github.com/emscripten-core/emsdk.git
cd emsdk
./emsdk install 5.0.0
./emsdk activate 5.0.0
cd ..
```

### 3. Set EMSDK_ROOT

Open `build_wasm_common.sh` and edit line 9 to match where you cloned the SDK:

```bash
EMSDK_ROOT="/path/to/emsdk"
```

### 4. Dev build (fast, unoptimized)

```bash
bash build_wasm_dev.sh
cd build/web_game && python -m http.server 8081
# Open: http://localhost:8081/boe.html
```

### 5. Prod build (optimized, for GitHub Pages)

```bash
bash build_wasm_prod.sh
cd docs && python -m http.server 8082
# Open: http://localhost:8082/index.html
```

Commit the `docs/` directory and push to deploy via GitHub Pages.

### 6. Hard-refresh after rebuilding

Browser caches WASM aggressively. After any rebuild, press `Ctrl+Shift+R`
to force a full reload.

---

Web Architecture
----------------

**Compat headers** (`src/compat/`): Thin stubs replace SFML/Boost with
Canvas 2D API calls, Web Audio, and standard C++ containers. No SFML or
Boost libraries are linked.

**Event bridge** (`web/events.js`): Browser DOM events are captured in
JavaScript and forwarded to C++ via `Module.ccall` → `push_mouse_event` /
`push_key_event` → an `std::queue` that `pollEvent()` drains each frame.

**Save / Load**:
- *Save* — `Ctrl+S` writes to Emscripten MEMFS, then JavaScript syncs to
  IndexedDB.
- *Load* — `Ctrl+O` is intercepted entirely in JavaScript (before C++
  sees it). JS shows a file picker, writes the chosen file into MEMFS,
  then calls `wasm_load_from_path()`. This avoids nested ASYNCIFY calls.

**ASYNCIFY**: Enables blocking dialog loops by suspending and resuming the
C++ call stack. It is only safe at the top level. All `sf::sleep()` calls
and any dialog invocations inside game logic are guarded with
`#ifndef __EMSCRIPTEN__` to prevent nesting.

**HTML menu bar** (`web/menu.js`, `web/shell.html`): A native `<nav>`
element sits above the 800×600 canvas. C++ drives it via `EM_ASM`; JS
calls back via `Module.ccall` (`wasm_menu_action`, `wasm_cast_spell`, etc.).

---

Key Files
---------

| File | Purpose |
|------|---------|
| `web/web_stubs.cpp` | Platform-specific C++ implementations (rendering, audio, file I/O) |
| `web/events.js` | Browser → C++ event bridge; intercepts Ctrl+O/Ctrl+S |
| `web/shell.html` | Emscripten HTML shell; contains menu bar markup |
| `src/compat/graphics.hpp` | SFML Graphics API stub backed by Canvas 2D |
| `web/savemanager.js` | IndexedDB save/load and MEMFS sync logic |

---

Known Limitations
-----------------

See [web/TODO.md](./web/TODO.md) for the current list of missing features
and known issues.

---

Helpful Links
-------------

- [BoE Forum at Spiderweb Software](http://spiderwebforums.ipbhost.com/index.php?/forum/12-blades-of-exile/)
- [Official IRC channel](https://web.libera.chat/#openboe) (`#openboe` on irc.libera.chat)
- [Original Game Source](http://www.spiderwebsoftware.com/blades/opensource.html) — where it all began
- [Official Web Site](https://openboe.com/) — documentation, scenario archive, and more
