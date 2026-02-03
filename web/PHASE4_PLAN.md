# Phase 4: File I/O & Resource Loading - Implementation Plan

## Current State (End of Phase 3)

✅ **Working**:
- Canvas 2D rendering infrastructure
- Texture loading from Emscripten VFS
- Image rendering (sprites, scaling)
- Continuous render loop with embind
- Test application rendering game-like graphics

**Build Size**: 936KB WASM + 224KB JS (~300KB gzipped)

---

## Phase 4 Objectives

Get the actual game loading and displaying real scenario data in the browser.

### Success Criteria
- Load a BoE scenario file
- Display terrain tiles
- Show party character sprites
- Render UI elements
- Save/load game state

---

## Implementation Strategy

### Step 1: Resource File Analysis (1 day)

**Goal**: Understand what files the game needs and where they are.

**Tasks**:
1. Inventory game resource files:
   ```bash
   find . -name "*.png" -o -name "*.bmp" -o -name "*.wav"
   ```

2. Identify critical resources:
   - Terrain tiles (where?)
   - Character sprites (where?)
   - UI graphics (where?)
   - Sound files (for later)

3. Document file paths and sizes
4. Create list for VFS preloading

**Deliverable**: `RESOURCES.md` documenting all game assets

---

### Step 2: Minimal Game Build (2-3 days)

**Goal**: Compile a minimal version of the game that initializes but doesn't do much.

**Approach**: Create `web/test_game_init.cpp` that:
```cpp
// Minimal game initialization test
#include "boe.global.hpp"
#include "gfx/render_target_canvas.hpp"

int main() {
    printf("Initializing BoE Web Edition...\n");

    // Create window
    auto window = createRenderWindow(960, 640, "BoE");

    // Initialize subsystems (stub versions)
    // - Resource manager
    // - Terrain loader
    // - Sprite loader

    // Render loop
    while (true) {
        window->clear(Color(0, 0, 0));

        // Draw something simple

        window->display();

        // Call via embind from JS
    }

    return 0;
}
```

**Expected Challenges**:
- SFML dependencies in headers → Add #ifdef guards
- Boost filesystem → Use our IFileSystem abstraction
- Missing shaders → Stub out or remove
- Sound initialization → Stub for now

**Incremental Compilation**:
1. Start with just boe.global.hpp
2. Add one subsystem at a time
3. Stub out what doesn't compile
4. Document what needs web-specific implementation

---

### Step 3: VFS Resource Preloading (1 day)

**Goal**: Get game resources into the Emscripten VFS at build time.

**Implementation**:
```bash
# In build script
emcc ... \
    --preload-file rsrc@/rsrc \
    --preload-file graphics@/graphics \
    --preload-file scenarios@/scenarios
```

**File Organization**:
```
build/web/
├── boe.wasm
├── boe.js
├── boe.data        # Preloaded files (auto-generated)
└── index.html
```

**Test**:
```cpp
// Verify files are accessible
bool test_vfs() {
    return FS.analyzePath('/rsrc/terrain.png').exists;
}
```

---

### Step 4: Terrain Rendering Test (2 days)

**Goal**: Load and display actual game terrain tiles.

**Tasks**:
1. Port terrain loading code to use ITexture:
   ```cpp
   // In terrain loader
   auto terrain_sheet = createTexture();
   terrain_sheet->loadFromFile("/rsrc/terrain.png");
   ```

2. Implement tile rendering:
   ```cpp
   // Draw a 10x10 grid of terrain tiles
   for(int y = 0; y < 10; y++) {
       for(int x = 0; x < 10; x++) {
           rectangle src(x*32, y*32, (x+1)*32, (y+1)*32);
           rectangle dst(x*32, y*32, (x+1)*32, (y+1)*32);
           drawTextureRegion(*terrain_sheet, src, *window, dst);
       }
   }
   ```

3. Test with simple terrain array

**Deliverable**: Web page showing a grid of terrain tiles

---

### Step 5: Scenario File Loading (2-3 days)

**Goal**: Load actual BoE scenario files from VFS.

**Current Code Analysis**:
- Scenario files are binary format
- Loaded via `load_scenario()` in boe.fileio.cpp
- Uses custom file reading functions

**Adaptation Needed**:
```cpp
// Original (desktop)
fs::ifstream file(scenario_path, std::ios::binary);

// Web version (via IFileSystem)
auto fs = getFileSystem(); // EmscriptenFileSystem
auto data = fs->readFile("/scenarios/example.boes");
// Parse binary data
```

**Steps**:
1. Find scenario file format documentation
2. Verify files can be read from VFS
3. Port binary parsing to work with VFS
4. Test loading a scenario

---

### Step 6: Save Game Persistence (2 days)

**Goal**: Save/load game state using IndexedDB.

**Architecture**:
```
C++ Game Logic
      ↓
  Save to VFS (/save/game.sav)
      ↓
Sync VFS to IndexedDB (IDBFS)
      ↓
Browser IndexedDB Storage
```

**Implementation**:
```javascript
// Mount IDBFS for save directory
FS.mkdir('/save');
FS.mount(IDBFS, {}, '/save');

// Load existing saves
FS.syncfs(true, function(err) {
    if (!err) console.log('Saves loaded');
});

// After save operation
FS.syncfs(false, function(err) {
    if (!err) console.log('Save persisted');
});
```

**C++ Side**:
```cpp
// Save game
void save_game(const std::string& filename) {
    auto fs = getFileSystem();
    fs->writeFile("/save/" + filename, save_data);

    // Trigger sync to IndexedDB
    EM_ASM({
        FS.syncfs(false, function(err) {});
    });
}
```

---

### Step 7: File Picker for Scenarios (1 day)

**Goal**: Let users load their own scenario files.

**Implementation**:
```html
<!-- File picker button -->
<input type="file" id="scenario-picker" accept=".boes,.exs">

<script>
document.getElementById('scenario-picker').addEventListener('change', function(e) {
    var file = e.target.files[0];
    var reader = new FileReader();

    reader.onload = function(e) {
        var data = new Uint8Array(e.target.result);

        // Write to VFS
        FS.writeFile('/scenarios/uploaded.boes', data);

        // Trigger C++ to load it
        Module.loadScenario('/scenarios/uploaded.boes');
    };

    reader.readAsArrayBuffer(file);
});
</script>
```

---

## Timeline

| Task | Duration | Dependencies |
|------|----------|--------------|
| Resource Analysis | 1 day | None |
| Minimal Game Build | 3 days | Resource Analysis |
| VFS Preloading | 1 day | Resource Analysis |
| Terrain Rendering | 2 days | Minimal Build, VFS |
| Scenario Loading | 3 days | VFS, Terrain |
| Save Persistence | 2 days | Scenario Loading |
| File Picker | 1 day | Scenario Loading |

**Total**: ~13 days (2.5 weeks)

---

## Risk Mitigation

### High Risk: Binary File Format Compatibility
- **Risk**: Scenario files may have endianness or platform issues
- **Mitigation**: Test with multiple scenarios early, add validation

### Medium Risk: Resource File Size
- **Risk**: Preloading all resources could make initial download huge
- **Mitigation**: Lazy-load optional content, use compression

### Medium Risk: Memory Constraints
- **Risk**: Browser may limit WASM memory
- **Mitigation**: Use ALLOW_MEMORY_GROWTH, profile memory usage

---

## Next Immediate Actions

1. **Catalog Resources** (Start Here):
   ```bash
   cd /d/gitRepos/cboe
   find rsrc -type f > web/RESOURCES.txt
   ls -lh rsrc/* > web/RESOURCE_SIZES.txt
   ```

2. **Create Minimal Build Script**:
   - Copy build_texture_test.sh to build_game.sh
   - Add boe.global.hpp and basic game files
   - See what compiles

3. **Document Compilation Errors**:
   - Track what doesn't compile
   - Identify patterns (SFML, Boost, etc.)
   - Plan abstractions

---

## Success Metrics

- [ ] Can compile minimal game initialization
- [ ] Can load terrain sprite sheet
- [ ] Can display 10x10 terrain grid
- [ ] Can read scenario file from VFS
- [ ] Can save to IndexedDB
- [ ] Can load user scenario file

When all checked, **Phase 4 is complete** and we're ready for Phase 5 (Audio) or Phase 6 (Input).
