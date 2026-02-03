#!/bin/bash

# Blades of Exile - WebAssembly Test Build Script
# Builds the Canvas 2D test

# Configuration
EMSDK_ROOT="/d/gitRepos/emsdk"
PYTHON_EXE="$EMSDK_ROOT/python/3.13.3_64bit/python.exe"
EMCC="$EMSDK_ROOT/upstream/emscripten/emcc.py"
BUILD_DIR="build/web"
SOURCE_DIR="src"
WEB_DIR="web"

# Create build directory if it doesn't exist
mkdir -p "$BUILD_DIR"

echo "Building Blades of Exile Canvas 2D Test..."

# Source files
SOURCES=(
    "$WEB_DIR/test_canvas.cpp"
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
    "-s" "WASM=1"
    "-s" "USE_ZLIB=1"
    "-s" "ALLOW_MEMORY_GROWTH=1"
    "-s" "EXPORTED_FUNCTIONS=['_main','_malloc']"
    "-s" "EXPORTED_RUNTIME_METHODS=['ccall','cwrap']"
    "-g"
    "-std=c++17"
)

echo "Compiling..."

# Compile
"$PYTHON_EXE" "$EMCC" \
    "${SOURCES[@]}" \
    "${INCLUDES[@]}" \
    "${EMCC_FLAGS[@]}" \
    -o "$BUILD_DIR/boe.html"

if [ $? -eq 0 ]; then
    echo "Build successful!"
    echo "Files generated:"
    ls -lh "$BUILD_DIR/boe."{wasm,js,html}

    # Copy custom index.html and canvas_helpers.js
    cp "$WEB_DIR/index.html" "$BUILD_DIR/index.html"
    cp "$WEB_DIR/canvas_helpers.js" "$BUILD_DIR/canvas_helpers.js"
    echo "Custom HTML and JS files copied."

    echo ""
    echo "To test, run:"
    echo "  cd $BUILD_DIR && python -m http.server 8080"
    echo "Then open: http://localhost:8080"
else
    echo "Build failed!"
    exit 1
fi
