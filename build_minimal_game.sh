#!/bin/bash

# Minimal BoE Game Build Script for WebAssembly
# Goal: Compile real game code and identify porting blockers

# Configuration
EMSDK_ROOT="/d/gitRepos/emsdk"
PYTHON_EXE="$EMSDK_ROOT/python/3.13.3_64bit/python.exe"
EMCC="$EMSDK_ROOT/upstream/emscripten/emcc.py"
BUILD_DIR="build/web_game"
SOURCE_DIR="src"
WEB_DIR="web"

# Create build directory
mkdir -p "$BUILD_DIR"

echo "=== Building Minimal BoE Game for WebAssembly ==="
echo "Goal: Identify real game compilation blockers"
echo ""

# Start with minimal sources - just the entry point
SOURCES=(
    "$WEB_DIR/minimal_game.cpp"
    "$SOURCE_DIR/universe/universe.cpp"
)

# Add more game sources incrementally:
# GAME_SOURCES=(
#     "$SOURCE_DIR/universe/party.cpp"
#     "$SOURCE_DIR/universe/town.cpp"
#     "$SOURCE_DIR/scenario/scenario.cpp"
#     etc...
# )

# Include directories
INCLUDES=(
    "-I$SOURCE_DIR"
    "-I$SOURCE_DIR/game"
    "-I$SOURCE_DIR/gfx"
    "-I$SOURCE_DIR/platform"
    "-I$SOURCE_DIR/universe"
    "-I$SOURCE_DIR/tools"
    "-I$SOURCE_DIR/fileio"
    "-I$SOURCE_DIR/fileio/xml-parser"
    "-I$SOURCE_DIR/dialogxml"
)

# Build flags - keep it simple
EMCC_FLAGS=(
    "-sWASM=1"
    "-sALLOW_MEMORY_GROWTH=1"
    "-sEXPORTED_FUNCTIONS=[\"_main\",\"_malloc\"]"
    "-sEXPORTED_RUNTIME_METHODS=[\"ccall\",\"cwrap\"]"
    "-sNO_EXIT_RUNTIME=1"
    "-sSTRICT=0"
    "-g"
    "-std=c++17"
    "-lembind"
)

echo "Step 1: Compiling with universe.cpp..."
echo ""

# Compile
"$PYTHON_EXE" "$EMCC" \
    "${SOURCES[@]}" \
    "${INCLUDES[@]}" \
    "${EMCC_FLAGS[@]}" \
    -o "$BUILD_DIR/boe_minimal.html"

if [ $? -eq 0 ]; then
    echo ""
    echo "✅ Build successful!"
    echo ""
    echo "Files generated:"
    ls -lh "$BUILD_DIR/boe_minimal."{wasm,js,html}
    echo ""
    echo "Next steps:"
    echo "  1. Test this build works"
    echo "  2. Add universe.cpp and see what breaks"
    echo "  3. Add graphics system and identify SFML dependencies"
    echo "  4. Document all porting blockers"
    echo ""
    echo "To test:"
    echo "  cd $BUILD_DIR && python -m http.server 8081"
    echo "  Then open: http://localhost:8081/boe_minimal.html"
else
    echo ""
    echo "❌ Build failed!"
    echo ""
    echo "This is expected - we're identifying what needs to be fixed."
    echo "Check the errors above to see what dependencies are missing."
    exit 1
fi
