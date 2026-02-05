# Building Blades of Exile for WebAssembly

## Prerequisites

1. **Emscripten SDK** must be installed and activated:
   ```bash
   # If not already installed:
   cd D:/gitRepos
   git clone https://github.com/emscripten-core/emsdk.git
   cd emsdk
   ./emsdk install 5.0.0
   ./emsdk activate 5.0.0

   # Activate for current session:
   source ./emsdk_env.sh  # Linux/Mac
   # OR
   emsdk_env.bat          # Windows
   ```

2. **Python 3** for running the web server

## Building the WASM Game

```bash
cd D:/gitRepos/cboe
bash build_minimal_game.sh
```

This will:
- Compile 94+ C++ source files to WebAssembly
- Preload game resources (graphics, sounds, fonts, etc.) into VFS
- Generate output in `build/web_game/`:
  - `boe_minimal.html` - HTML shell
  - `boe_minimal.js` - Emscripten JavaScript glue code (~300KB)
  - `boe_minimal.wasm` - Compiled game code (~45MB)
  - `boe_minimal.data` - Preloaded resources

## Running the Game Locally

Start a local web server:

```bash
cd D:/gitRepos/cboe/build/web_game
python -m http.server 8082
```

Then open in your browser:
```
http://localhost:8082/boe_minimal.html
```

**Important**: After rebuilding, always **hard refresh** the browser:
- Windows/Linux: `Ctrl + Shift + R`
- Mac: `Cmd + Shift + R`

This clears the browser cache so you see the new WASM file.

## Current Implementation Status

### ✅ Working
- **Canvas 2D rendering** - Graphics display in browser
- **Texture loading** - Images loaded from VFS and rendered
- **Text rendering** - UI labels and text display correctly
- **Animation** - Sprite frame cycling working
- **Main loop** - Non-blocking 60 FPS game loop with `emscripten_set_main_loop()`
- **Resource loading** - All game data preloaded via `--preload-file`

### ⚠️ In Progress
- **Sprite sheet rendering** - Texture rectangles need refinement
- **Draw deduplication** - May be filtering out valid sprites

### ❌ Not Yet Implemented
- **Input handling** - Mouse and keyboard events
- **Audio** - Web Audio API for sounds/music
- **Save/Load** - IndexedDB for persistent storage
- **Full UI** - Dialogs and menus

## Key Files

- `build_minimal_game.sh` - Main build script
- `web/web_stubs.cpp` - Web-specific platform implementations
- `src/compat/graphics.hpp` - Graphics abstraction layer (Canvas 2D)
- `src/compat/texture.hpp` - Texture loading (VFS to JavaScript Image)
- `src/game/boe.main.cpp` - Main loop with Emscripten integration

## Debugging

### Browser Console
Open Developer Tools (F12) to see:
- Console logs from C++ (`std::cout` → `console.log`)
- Texture loading messages
- JavaScript errors

### Common Issues

**Black screen after build:**
- Hard refresh the browser (Ctrl+Shift+R)
- Check console for errors
- Verify server is running on correct port

**"Assertion failed: No EM_ASM constant found":**
- WASM file and JS file are out of sync
- Hard refresh to clear cache

**Resources not loading:**
- Check `data` directory exists in `build/web_game/`
- Verify `--preload-file data@/data` in build script
- Check browser console for 404 errors

## Development Workflow

1. Make code changes
2. Run `bash build_minimal_game.sh`
3. Hard refresh browser (Ctrl+Shift+R)
4. Check console for errors
5. Repeat

## Next Steps for Development

### Implement Input Handling
- Add event listeners for mouse/keyboard in main loop
- Map browser events to game input
- See `src/compat/event.hpp` for event structures

### Fix Sprite Sheet Rendering
- Texture rectangles defined in `src/game/boe.graphics.cpp`
- Example: `startup_from[4]` array has correct pixel coordinates
- Ensure draw code uses these exactly as specified

### Add Audio Support
- Implement Web Audio API in `src/compat/audio.hpp`
- Load sounds from VFS
- Play using AudioContext

## Build Script Details

The `build_minimal_game.sh` script uses these key Emscripten flags:

```bash
-s ALLOW_MEMORY_GROWTH=1          # Allow WASM memory to grow
-s USE_SDL=0                       # Don't use SDL (custom Canvas 2D)
-s DISABLE_EXCEPTION_CATCHING=0    # Enable C++ exceptions
--preload-file data@/data          # Embed resources in .data file
-s EXPORTED_RUNTIME_METHODS=['FS'] # Export filesystem API
```

## Performance Notes

- WASM file is ~45MB (unoptimized debug build)
- First load takes a few seconds to download and compile
- Subsequent loads faster due to browser caching
- 60 FPS target with `emscripten_set_main_loop()`

## Troubleshooting Build Errors

**"unknown type name 'console'" in EM_ASM:**
- JavaScript syntax error in EM_ASM block
- Check for JS reserved words being parsed as C++
- Simplify complex JS expressions

**"table index is out of bounds":**
- Missing widget implementation (button, scrollbar, etc.)
- Check that all required .cpp files are in SOURCES list

**Linker errors:**
- Missing source files in build script
- Check that all dependencies are included
- Verify function implementations exist

## Resources

- [Emscripten Documentation](https://emscripten.org/docs/getting_started/index.html)
- [Porting Guide](https://emscripten.org/docs/porting/index.html)
- [Web Audio API](https://developer.mozilla.org/en-US/docs/Web/API/Web_Audio_API)
- [Canvas 2D API](https://developer.mozilla.org/en-US/docs/Web/API/Canvas_API)
