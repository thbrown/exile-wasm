# WebAssembly Compilation Blockers

## Summary

Attempting to compile the real BoE game code reveals the dependencies that must be replaced or wrapped for WebAssembly.

**Status**: Still discovering blockers (headers-only compilation)
**Approach**: Fix Boost/SFML dependencies with compatibility layers

---

## Blockers Found & Fixed

### ✅ Blocker #1: Boost Filesystem
**File**: `src/universe/universe.hpp`
**Issue**: `#include <boost/filesystem/path.hpp>`
**Solution**: Added conditional compilation to use `std::filesystem` (C++17) for web builds
**Files Modified**:
- `src/global.hpp` - Added `#ifdef __EMSCRIPTEN__` to alias `fs` namespace
- `src/universe/universe.hpp` - Conditional include

### ✅ Blocker #2: Boost Iterator
**File**: `src/universe/party.hpp`
**Issue**: `#include <boost/iterator/indirect_iterator.hpp>`
**Solution**: Disabled iterator methods for web builds (direct array access still works)
**Files Modified**:
- `src/universe/party.hpp` - Wrapped iterator methods in `#ifndef __EMSCRIPTEN__`

### ✅ Blocker #3: Boost Optional
**File**: `src/damage.hpp`
**Issue**: `#include <boost/optional.hpp>`
**Solution**: Use `std::optional` (C++17) for web builds
**Files Modified**:
- `src/damage.hpp` - Added compatibility layer with `opt` namespace alias

### ✅ Blocker #4: Boost Dynamic Bitset
**Files**: `src/scenario/area.hpp`, `src/scenario/town.hpp`
**Issue**: `#include <boost/dynamic_bitset.hpp>`
**Solution**: Created `std::vector<bool>` wrapper with compatible API
**Files Created**:
- `src/compat/dynamic_bitset.hpp` - Full wrapper implementation
**Files Modified**:
- `src/scenario/area.hpp` - Use compat header
- `src/scenario/town.hpp` - Use compat header

### ✅ Blocker #5: SFML Time
**File**: `src/mathutil.hpp`
**Issue**: `#include <SFML/System/Time.hpp>`
**Solution**: Created `std::chrono` wrapper with sf::Time/Clock API
**Files Created**:
- `src/compat/time.hpp` - Full Time and Clock implementation
**Files Modified**:
- `src/mathutil.hpp` - Use compat header

---

## Blockers In Progress

### ⏳ Blocker #6: Boost Optional (again)
**File**: `src/scenario/town.hpp:200`
**Issue**: Direct use of `boost::optional<rectangle>`
**Solution**: Need to add `opt` namespace alias to this file

### ⏳ Blocker #7: Boost Filesystem (again)
**File**: `src/scenario/scenario.hpp:14`
**Issue**: `#include <boost/filesystem/path.hpp>`
**Solution**: Same as Blocker #1 - add conditional include

---

## Expected Future Blockers

Based on grep search, these are known to exist:

### Boost Dependencies
- `boost::algorithm::string` - string utilities (trim, replace, etc.)
- `boost::any` - type-erased value holder → `std::any` (C++17)
- `boost::lexical_cast` - string conversions → `std::to_string`/`std::stoi`
- `boost::math::constants` - math constants (pi, etc.)
- `boost::process` - process spawning (NOT available on web - stub/disable)
- `boost::ptr_container` - pointer containers → use unique_ptr directly

### SFML Dependencies
- `SFML/Graphics.hpp` - **MAJOR** - rendering system
  - `sf::RenderWindow` → Our `CanvasRenderWindow`
  - `sf::RenderTexture` → Our `CanvasRenderTexture`
  - `sf::Texture` → Our `CanvasTexture`
  - `sf::Sprite` → Canvas drawImage()
  - `sf::Font` → Web fonts
  - `sf::Text` → Canvas fillText()
  - `sf::Shape` → Canvas paths
  - `sf::Color` → Already abstracted in render_target.hpp
- `SFML/Audio.hpp` - Sound system → Web Audio API
- `SFML/Window.hpp` - Events, keyboard, mouse
  - `sf::Event` → Browser events
  - `sf::Keyboard` → keydown/keyup
  - `sf::Mouse` → mouse events
- `SFML/System.hpp` - Vectors, threads
  - `sf::Vector2` → Already have location/vector2d types
  - `sf::Thread` → Not needed (single-threaded)

---

## Strategy Assessment

### Current Approach: Incremental Fixing ✅
**Pros**:
- Identifies *all* real dependencies
- Creates reusable compatibility layers
- Maintains desktop build compatibility
- Documents exactly what's needed

**Cons**:
- Time-consuming
- Many files to modify
- Risk of missing dependencies in cpp files

### Progress So Far
- **Headers compiling**: Working through dependency chain
- **5 blockers fixed**: filesystem, iterator, optional, dynamic_bitset, time
- **2 blockers in queue**: More optional and filesystem uses
- **Estimate**: ~15-20 more Boost blockers
- **Big one ahead**: SFML graphics system (when we add actual game source files)

---

## Recommendation

**Continue current approach** BUT:

1. **Create a Boost compatibility header** (`src/compat/boost.hpp`) that includes all our wrappers
2. **Batch-fix remaining Boost issues** before tackling SFML
3. **Test compilation without source files first** - just headers
4. **Then add source files** one by one to find SFML usage

### Next Steps

1. Fix town.hpp boost::optional usage
2. Fix scenario.hpp filesystem include
3. Search for and fix remaining boost:: uses in headers
4. Create comprehensive compat/boost.hpp
5. Attempt full header compilation
6. Document any remaining header issues
7. Begin adding .cpp source files
8. Tackle SFML graphics system

---

## Files Modified So Far

- `src/global.hpp` - Filesystem namespace
- `src/universe/universe.hpp` - Filesystem include
- `src/universe/party.hpp` - Iterator disable
- `src/damage.hpp` - Optional compatibility
- `src/scenario/area.hpp` - Dynamic bitset
- `src/scenario/town.hpp` - Dynamic bitset
- `src/scenario/vehicle.hpp` - Added global.hpp include
- `src/mathutil.hpp` - Time compatibility

## Files Created

- `src/compat/dynamic_bitset.hpp` - Bitset wrapper (~80 lines)
- `src/compat/time.hpp` - Time/Clock wrapper (~100 lines)
- `web/minimal_game.cpp` - Test application (~60 lines)
- `build_minimal_game.sh` - Build script

---

**Date**: February 2, 2026
**Phase**: Real Game Compilation (Phase 4, Step 2)
**Status**: Making steady progress, approach validated
