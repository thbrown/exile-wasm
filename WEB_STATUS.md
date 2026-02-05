# WebAssembly Port - Current Status

## ✅ What's Working

### Core Systems
- **94+ C++ files compile** to ~45MB WASM
- **Emscripten VFS** with 5.1MB preloaded resources (graphics, fonts, sounds, etc.)
- **Non-blocking main loop** using `emscripten_set_main_loop()` at 60 FPS
- **Resource loading** from virtual filesystem

### Rendering
- **Canvas 2D** clear and draw operations
- **Texture loading** from VFS → JavaScript Image objects
- **Sprite rendering** with textures
- **Text rendering** with Canvas fillText()
- **Animation** (startanim.png cycling through frames)

### What You Can See
- Blue background
- Startup screen graphics
- Button graphics
- Animated elements
- Text labels

## ⚠️ Known Issues

### Sprite Rendering - ROOT CAUSE IDENTIFIED! 🎯
**Problem**: Sprites show only 100x100 pixels instead of full sprite sheet regions

**Root Cause Found**: `location.cpp` line 96
```cpp
rectangle::rectangle(const sf::Texture& texture) :
    top(0), left(0),
    right(texture.getSize().x),   // Returns stub 100!
    bottom(texture.getSize().y) {} // Returns stub 100!
```

**What Happens**:
1. Game code passes correct coords: `startup_from[0] = {0,0,274,602}`
2. `render_image.cpp` line 100 creates: `rectangle src_gworld_rect(texture)` → `{0,0,100,100}`
3. Line 101 clips: `src_rect &= src_gworld_rect` → `{0,0,274,602} & {0,0,100,100}` = `{0,0,100,100}`
4. Sprite draws only 100x100 pixels! ❌

**The Fix**: Make `Texture::getSize()` return actual image dimensions

**Implementation** in `src/compat/texture.hpp`:
```cpp
Vector2u getSize() const override {
    #ifdef __EMSCRIPTEN__
    // Query actual dimensions from JavaScript
    int64_t dims = EM_ASM_INT({
        var filename = UTF8ToString($0);
        if (Module.textureDimensions && Module.textureDimensions[filename]) {
            var d = Module.textureDimensions[filename];
            return (BigInt(d.width) << 32n) | BigInt(d.height);
        }
        // Fallback to stored dimensions
        return (BigInt($1) << 32n) | BigInt($2);
    }, filename_.c_str(), width_, height_);

    unsigned int w = (dims >> 32) & 0xFFFFFFFF;
    unsigned int h = dims & 0xFFFFFFFF;
    return Vector2u(w, h);
    #else
    return Vector2u(width_, height_);
    #endif
}
```

This will make `texture.getSize()` return 602x322 instead of 100x100, fixing the clipping!

### Deduplication
**Status**: Currently DISABLED (all sprites draw, even duplicates)

**Issue**: When enabled, only 3 unique sprites render instead of multiple UI elements

**Code**: `src/compat/graphics.hpp` around line 580
- Uses position + size + color to detect duplicates
- May need to include texture rect in dedup key

## ❌ Not Implemented

- **Input handling** - No mouse/keyboard events yet
- **Audio** - Web Audio API not hooked up
- **Save/Load** - No persistent storage
- **Full UI** - Many dialogs/menus missing

## 🔧 Next Steps to Continue

### 1. Fix Sprite Positioning/Scaling

**Debug approach**:
```cpp
// In graphics.hpp draw() function, add logging:
std::cout << "Drawing: texRect=(" << texRect.left << "," << texRect.top
          << "," << texRect.width << "," << texRect.height << ") "
          << "pos=(" << pos.x << "," << pos.y << ") "
          << "scale=(" << scale.x << "," << scale.y << ") "
          << "destSize=(" << w << "," << h << ")" << std::endl;
```

Compare with what the original SFML version would do.

**Key insight**: The `scale` values (6.02, 2.74, etc.) are being calculated somewhere. Find where and why.

### 2. Implement Input Handling

**Files**:
- `src/compat/event.hpp` - Event structures
- `src/compat/graphics.hpp` - RenderWindow::pollEvent()

**Approach**:
```javascript
// In HTML or JS, add event listeners:
canvas.addEventListener('click', function(e) {
    var rect = canvas.getBoundingClientRect();
    var x = e.clientX - rect.left;
    var y = e.clientY - rect.top;
    // Store event for C++ to poll
});
```

Then in C++ `pollEvent()`, retrieve stored events from JavaScript.

### 3. Re-enable Smart Deduplication

Instead of position-only dedup, use:
```cpp
// Include texture rect in key:
std::string key = std::to_string(pos.x) + "," + std::to_string(pos.y) + "," +
                  std::to_string(texRect.left) + "," + std::to_string(texRect.top);
```

This allows multiple sprites at same position if they use different texture rects.

## 📊 Architecture Overview

### Build Flow
```
C++ Source Files
    ↓
Emscripten Compiler
    ↓
WASM Module (.wasm) + JS Glue (.js)
    ↓
Preloaded Resources (.data)
    ↓
Browser loads and runs
```

### Rendering Pipeline
```
Game Code (C++)
    ↓
Creates sf::Sprite with texture rect
    ↓
Calls window.draw(sprite)
    ↓
RenderWindow::draw() in graphics.hpp
    ↓
EM_ASM calls Canvas 2D drawImage()
    ↓
Pixels on screen!
```

### Resource Loading
```
build_minimal_game.sh: --preload-file data@/data
    ↓
Resources embedded in .data file
    ↓
Emscripten VFS mounts at /data
    ↓
C++: FS.readFile("/data/graphics/startup.png")
    ↓
JavaScript: Blob → Image object
    ↓
Canvas drawImage()
```

## 🐛 Debugging Tips

### Check Texture Rectangles
```javascript
// In browser console:
Object.keys(Module.textureCache).forEach(name => {
    var img = Module.textureCache[name];
    console.log(name, img.naturalWidth + 'x' + img.naturalHeight);
});
```

### View All Draw Calls
Uncomment logging in `graphics.hpp` draw function to see every sprite being drawn.

### Check VFS Contents
```javascript
// In browser console:
FS.readdir('/data/graphics');  // List graphics files
```

## 📝 Code Locations

| Feature | File | Line |
|---------|------|------|
| Main loop | src/game/boe.main.cpp | ~450 |
| Startup screen drawing | src/game/boe.graphics.cpp | 340 |
| Sprite sheet coords | src/game/boe.graphics.cpp | 150 |
| Canvas rendering | src/compat/graphics.hpp | ~567 |
| Texture loading | src/compat/texture.hpp | ~200 |
| Text rendering | src/compat/graphics.hpp | ~650 |
| Build script | build_minimal_game.sh | - |

## 🎯 Success Criteria for "Done"

- [ ] All UI elements render correctly
- [ ] Mouse clicks work on buttons
- [ ] Can start a new game
- [ ] Can load/save parties
- [ ] Audio plays
- [ ] Full gameplay works

## 💡 Tips

- Always hard refresh (Ctrl+Shift+R) after rebuild
- Check browser console for errors
- Use `std::cout` in C++ → shows in browser console
- Test in different browsers (Chrome, Firefox) for compatibility
- Build size can be reduced with `-O3` optimization flag
