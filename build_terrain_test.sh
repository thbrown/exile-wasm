#!/bin/bash

# Blades of Exile - Terrain Rendering Test Build Script

# Configuration
EMSDK_ROOT="/d/gitRepos/emsdk"
PYTHON_EXE="$EMSDK_ROOT/python/3.13.3_64bit/python.exe"
EMCC="$EMSDK_ROOT/upstream/emscripten/emcc.py"
BUILD_DIR="build/web"
SOURCE_DIR="src"
WEB_DIR="web"

# Create build directory
mkdir -p "$BUILD_DIR"

echo "Building BoE Terrain Rendering Test..."

# Source files
SOURCES=(
    "$WEB_DIR/test_terrain.cpp"
    "$SOURCE_DIR/gfx/render_target_canvas.cpp"
    "$SOURCE_DIR/gfx/render_target_factory.cpp"
)

# Include directories
INCLUDES=(
    "-I$SOURCE_DIR"
    "-I$SOURCE_DIR/gfx"
    "-I$SOURCE_DIR/platform"
)

# Build flags
EMCC_FLAGS=(
    "-sWASM=1"
    "-sUSE_ZLIB=1"
    "-sALLOW_MEMORY_GROWTH=1"
    "-sEXPORTED_FUNCTIONS=[\"_main\",\"_malloc\"]"
    "-sEXPORTED_RUNTIME_METHODS=[\"ccall\",\"cwrap\",\"FS\",\"addFunction\"]"
    "-sNO_EXIT_RUNTIME=1"
    "-sALLOW_TABLE_GROWTH=1"
    "-sSTRICT=0"
    "-g"
    "-std=c++17"
    "-lembind"
    "--preload-file" "rsrc/graphics/ter1.png@/rsrc/graphics/ter1.png"
    "--preload-file" "rsrc/graphics/pcs.png@/rsrc/graphics/pcs.png"
)

echo "Compiling with preloaded terrain graphics..."

# Compile
"$PYTHON_EXE" "$EMCC" \
    "${SOURCES[@]}" \
    "${INCLUDES[@]}" \
    "${EMCC_FLAGS[@]}" \
    -o "$BUILD_DIR/boe.html"

if [ $? -eq 0 ]; then
    echo "Build successful!"
    echo "Files generated:"
    ls -lh "$BUILD_DIR/boe."{wasm,js,html,data}

    # Copy custom files
    cp "$WEB_DIR/index.html" "$BUILD_DIR/index.html"
    cp "$WEB_DIR/canvas_helpers.js" "$BUILD_DIR/canvas_helpers.js"
    echo "Custom HTML and JS files copied."

    echo ""
    echo "Preloaded resources:"
    echo "  - rsrc/graphics/ter1.png (terrain tiles)"
    echo "  - rsrc/graphics/pcs.png (character sprites)"
    echo ""
    echo "To test, run:"
    echo "  cd $BUILD_DIR && python -m http.server 8080"
    echo "Then open: http://localhost:8080"
else
    echo "Build failed!"
    exit 1
fi
