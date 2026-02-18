# WASM Port - Remaining Work

## Unimplemented Stubs

### Replay System
- `pop_next_action()`, `encode_file()`, `decode_file()` in `tools/replay.hpp` / `replay.cpp`
- Requires the cppcodec library (base64/hex encoding) which is not available for WASM builds
- The stubs compile and link, but replay recording/playback is disabled
- Stub locations: `web/web_stubs.cpp`

### sf::Image File Loading
- `sf::Image::loadFromFile()`, `loadFromStream()`, `loadFromMemory()` return a dummy 1x1 image
- Textures load correctly via JavaScript Image objects (the normal game flow), so this is not blocking
- However `sf::Image` pixel manipulation from files does not work
- Location: `src/compat/texture.hpp`

### Preferences (prefs.cpp)
- Too many boost dependencies to port; using localStorage stubs in `web/web_stubs.cpp` instead
- `get_pref()`, `set_pref()` etc. are stubbed to read/write a simple in-memory map backed by localStorage
- Some preferences may not persist correctly across sessions

### cStringChoice Dialog
- String choice dialog stub exists in `web/web_stubs.cpp`
- Not currently triggered in the main game flow, so not blocking

### nav_get_rsrc()
- Resource file picker (for navigating to resource directories)
- Not applicable for web builds; stubbed to return empty path

---

## Phase 2 Abstraction Layer (Unused)

Phase 2 of the port created abstract platform interfaces (IRenderTarget, IInputHandler,
ISoundPlayer, IFileSystem) with both SFML desktop implementations and Canvas/Web stubs.
The actual WASM port bypassed these abstractions and uses the compat header approach
instead (`src/compat/graphics.hpp`, `audio.hpp`, etc.).

These abstraction files exist but are **not compiled or used by the game**:

| File | Purpose |
|------|---------|
| `src/gfx/render_target.hpp` | Abstract render target interface |
| `src/gfx/render_target_canvas.cpp` | Canvas 2D implementation |
| `src/gfx/render_target_sfml.cpp` | SFML desktop implementation |
| `src/platform/input_handler.hpp` | Abstract input handler interface |
| `src/platform/input_handler_web.cpp` | Web input implementation |
| `src/platform/input_handler_sfml.cpp` | SFML desktop implementation |
| `src/audio/sound_player.hpp` | Abstract sound player interface |
| `src/audio/sound_player_webaudio.cpp` | Web Audio API implementation |
| `src/audio/sound_player_sfml.cpp` | SFML desktop implementation |
| `src/platform/filesystem.hpp` | Abstract filesystem interface |
| `src/platform/filesystem_emscripten.cpp` | Emscripten MEMFS implementation |
| `src/platform/filesystem_native.cpp` | Native filesystem implementation |
| `src/gfx/render_image_canvas.cpp` | Canvas image rendering |
| `src/gfx/render_target_factory.cpp` | Factory for creating render targets |

**Decision needed**: Either complete the migration to these abstractions or delete them.
The compat header approach is simpler and currently working; migrating would be a significant refactor.

---

## Known Limitations

### Shader Effects
- GLSL shaders are stubbed out; Canvas 2D API cannot run GLSL
- Some visual effects (certain spell animations, etc.) may be missing or simplified
- Location: `src/compat/graphics.hpp` shader stubs

### Color Tinting
- Uses Canvas 2D multiply blend mode which may not match SFML's tinting exactly
- Affects tinted sprites (status effects, etc.)

### Text Layout
- `Text::getLocalBounds()` and `findCharacterPos()` use rough estimates (0.6 * characterSize per char)
- May differ from actual Canvas rendering, causing minor text alignment issues
- Location: `src/compat/graphics.hpp`

### Hardcoded Graphics File List
- `web/events.js` (around lines 379-453) has a hardcoded list of graphics files to preload
- Must be updated manually if new graphics files are added to `rsrc/graphics/`

---

## Potential Improvements

- **Batch EM_ASM_ calls** to reduce JavaScript-C++ overhead (currently many small calls per frame)
- **Cache texture dimensions** after first lookup (currently queries JS every time via EM_ASM_INT)
- **Web Workers** for non-rendering tasks (scenario loading, save/load)
- **Performance profiling** and optimization pass
- **Error handling** for failed texture loads (currently silently uses placeholder)
- **Progressive loading** - load critical graphics first, defer non-essential content
