# Blades of Exile - WebAssembly Port

## Phase 1: Build System & Emscripten Setup ✅ COMPLETE

Successfully set up the WebAssembly build pipeline for Blades of Exile.

### Completed Tasks

1. **Emscripten SDK Installation**: Installed Emscripten 5.0.0
2. **Build Directory Structure**: Created `build/web/` and `web/` directories
3. **CMakeLists.txt**: Created minimal CMake configuration for web builds
4. **HTML Shell**: Created custom `index.html` with canvas and console output
5. **Test Compilation**: Successfully compiled and tested basic WASM module

### Verification Results

- ✅ Web build produces `boe.wasm` + `boe.js`
- ✅ Browser console shows "BoE WASM initialized"
- ✅ No compilation errors
- ✅ Desktop builds unaffected (still use SConstruct)

### How to Build

```bash
# Using the build script (recommended)
./build_web.sh

# Or manually
cd build/web
/d/gitRepos/emsdk/python/3.13.3_64bit/python.exe \
  /d/gitRepos/emsdk/upstream/emscripten/emcc.py \
  ../../web/main_web.cpp \
  -o boe.html \
  -s WASM=1 \
  -s USE_ZLIB=1 \
  -s ALLOW_MEMORY_GROWTH=1 \
  -g
```

### How to Test

```bash
# Start local web server
cd build/web
python -m http.server 8080

# Or test with Node.js
/d/gitRepos/emsdk/node/22.16.0_64bit/bin/node.exe boe.js
```

Then open: http://localhost:8080

### File Structure

```
cboe/
├── web/                      # Web-specific source files
│   ├── index.html           # Custom HTML shell
│   ├── main_web.cpp         # Minimal test main file
│   └── README.md            # This file
├── build/web/               # Build output directory
│   ├── boe.wasm            # Compiled WebAssembly binary
│   ├── boe.js              # Emscripten JavaScript glue code
│   ├── boe.html            # Generated HTML (test)
│   └── index.html          # Custom HTML shell (copied)
├── CMakeLists.txt           # CMake configuration for web builds
└── build_web.sh             # Build script
```

### Current Status

**Phase 1**: ✅ Complete (Build System & Emscripten Setup)
**Phase 2**: ✅ Complete (Platform Abstraction Layer)
**Phase 3**: 🔄 Next (Canvas 2D Rendering Implementation)

### Phase 2 Completion

Created platform-independent abstraction layers:
- ✅ Graphics (IRenderTarget, ITexture, IRenderWindow, IRenderTexture)
- ✅ Input (IInputHandler, InputEvent, KeyCode mapping)
- ✅ Audio (ISoundPlayer, ISound)
- ✅ File System (IFileSystem, Path)

**Implementation Status**:
- Desktop (SFML): Fully implemented
- Web (Canvas/Browser): Stub implementations (to be completed in Phases 3-6)

See [PHASE2_SUMMARY.md](./PHASE2_SUMMARY.md) for detailed information.

### Next Steps

Phase 3 will implement Canvas 2D rendering:
- Implement Canvas 2D backend for graphics
- Display game terrain and UI
- Text rendering with web fonts
- Off-screen canvas for RenderTextures

---

## Development Notes

### Emscripten Configuration

Current flags:
- `-s WASM=1` - Compile to WebAssembly
- `-s USE_ZLIB=1` - Include zlib (built-in)
- `-s ALLOW_MEMORY_GROWTH=1` - Allow dynamic memory growth
- `-g` - Include debug symbols

### Browser Requirements

- Modern browser with WebAssembly support
- Chrome 57+, Firefox 52+, Safari 11+, Edge 16+

### Known Limitations

- Desktop builds continue to use SConstruct (this is intentional)
- CMake build currently only supports Emscripten target
- Resources not yet preloaded (Phase 4)
- No graphics rendering yet (Phase 3)
