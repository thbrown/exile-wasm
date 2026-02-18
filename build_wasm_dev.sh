#!/bin/bash

# Dev build: fast compile with debug symbols -> build/web_game/boe.html
# Usage: bash build_wasm_dev.sh

set -e
source "$(dirname "$0")/build_wasm_common.sh"

BUILD_DIR="build/web_game"
OUTPUT_NAME="boe"

mkdir -p "$BUILD_DIR"

echo "=== Building Blades of Exile WASM [DEV] ==="
echo "Output: $BUILD_DIR/$OUTPUT_NAME.html"
echo ""

EMCC_FLAGS=("${EMCC_BASE_FLAGS[@]}" "-O0" "-g")

"$PYTHON_EXE" "$EMCC" \
    "${SOURCES[@]}" \
    "${INCLUDES[@]}" \
    "${EMCC_FLAGS[@]}" \
    -o "$BUILD_DIR/$OUTPUT_NAME.html"

echo ""
echo "Build successful!"
do_post_build "$BUILD_DIR" "$OUTPUT_NAME"
echo ""
echo "To test:"
echo "  cd $BUILD_DIR && python -m http.server 8081"
echo "  Then open: http://localhost:8081/$OUTPUT_NAME.html"
