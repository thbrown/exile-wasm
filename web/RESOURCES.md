# BoE Resource Inventory

## Summary

| Category | Size | Description |
|----------|------|-------------|
| Graphics | 2.7MB | Sprite sheets, terrain tiles, UI elements |
| Total Resources | 39MB | Graphics + scenarios + sounds + data |

## Graphics Breakdown (rsrc/graphics/)

### Core Game Graphics
- **Terrain Tiles**: ter1.png - ter14.png, ter19.png, teranim.png (~200KB total)
- **Monster Sprites**: monst1.png - monst11.png (~280KB total)
- **Character Sprites**: pc1.png - pc11.png (~300KB total)
- **UI Elements**: buttons, dialogs, inventory (~500KB total)

### Sprite Sheet Format
All appear to be PNG sprite sheets with multiple tiles/sprites per sheet.
Standard tile size appears to be 32x32 or 64x64 pixels.

## Preloading Strategy

### Phase 1: Essential Graphics Only (~1.5MB)
```
rsrc/graphics/ter*.png        # Terrain
rsrc/graphics/pc*.png          # Characters
rsrc/graphics/monst*.png       # Monsters
rsrc/graphics/dlog*.png        # UI dialogs
rsrc/graphics/buttons.png      # Buttons
```

### Phase 2: Full Graphics (~2.7MB)
All rsrc/graphics/*.png files

### Phase 3: Complete Resources (~39MB)
- Graphics
- Base scenarios
- Sound effects (when audio implemented)

## Build Script Integration

```bash
# Minimal (Phase 1)
emcc ... --preload-file rsrc/graphics/ter@/rsrc/graphics \
         --preload-file rsrc/graphics/pc@/rsrc/graphics \
         --preload-file rsrc/graphics/monst@/rsrc/graphics

# Full Graphics (Phase 2)
emcc ... --preload-file rsrc/graphics@/rsrc/graphics

# Complete (Phase 3)
emcc ... --preload-file rsrc@/rsrc
```

## Compression Estimates

With gzip compression (typical for WASM):
- 2.7MB graphics → ~800KB - 1.2MB
- 39MB total → ~10MB - 15MB

## Next Steps

1. ✅ Inventory complete
2. ⏳ Create test with real terrain tiles
3. ⏳ Implement sprite sheet loader
4. ⏳ Test with preloaded resources

---

**Created**: February 2, 2026
**Last Updated**: February 2, 2026
