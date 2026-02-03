#!/bin/bash

# Blades of Exile - WebAssembly Build Script
# This script builds the web version of Blades of Exile using Emscripten

# Configuration
EMSDK_ROOT="/d/gitRepos/emsdk"
PYTHON_EXE="$EMSDK_ROOT/python/3.13.3_64bit/python.exe"
EMCC="$EMSDK_ROOT/upstream/emscripten/emcc.py"
BUILD_DIR="build/web"
SOURCE_FILE="web/main_web.cpp"

# Create build directory if it doesn't exist
mkdir -p "$BUILD_DIR"

# Build flags
EMCC_FLAGS="-s WASM=1 -s USE_ZLIB=1 -s ALLOW_MEMORY_GROWTH=1 -g"

echo "Building Blades of Exile for WebAssembly..."
echo "Source: $SOURCE_FILE"
echo "Output: $BUILD_DIR/boe.html"

# Compile
"$PYTHON_EXE" "$EMCC" "$SOURCE_FILE" \
    -o "$BUILD_DIR/boe.html" \
    $EMCC_FLAGS

if [ $? -eq 0 ]; then
    echo "Build successful!"
    echo "Files generated:"
    ls -lh "$BUILD_DIR/boe."{wasm,js,html}

    # Copy custom index.html
    cp web/index.html "$BUILD_DIR/index.html"
    echo "Custom index.html copied."

    echo ""
    echo "To test, run: cd $BUILD_DIR && python -m http.server 8080"
    echo "Then open: http://localhost:8080"
else
    echo "Build failed!"
    exit 1
fi
