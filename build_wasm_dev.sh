#!/bin/bash

# Dev build: fast compile with debug symbols -> build/web_game/boe.html
# Usage: bash build_wasm_dev.sh

set -e
source "$(dirname "$0")/build_wasm_common.sh"

BUILD_DIR="build/web_game"
OUTPUT_NAME="boe"
HTML_FILE="${OUTPUT_NAME}.html"

# Clean old output entirely
echo "Cleaning $BUILD_DIR..."
rm -rf "$BUILD_DIR"
mkdir -p "$BUILD_DIR"

echo "=== Building Blades of Exile WASM [DEV] ==="
echo "Output: $BUILD_DIR/$HTML_FILE"
echo ""

EMCC_FLAGS=("${EMCC_BASE_FLAGS[@]}" "-O0" "-g")

"$PYTHON_EXE" "$EMCC" \
    "${SOURCES[@]}" \
    "${INCLUDES[@]}" \
    "${EMCC_FLAGS[@]}" \
    -o "$BUILD_DIR/$OUTPUT_NAME.html"

# Replace auto-loading script tag with deferred splash screen loader
# Match both verbose (-O0) and minified (-O3) script tag formats
sed -E -i "s|<script[^>]*src=[\"']?${OUTPUT_NAME}\.js[\"']?[^>]*></script>|<script>var wasmScriptUrl = \"${OUTPUT_NAME}.js\";</script>|" \
    "$BUILD_DIR/$HTML_FILE"

copy_web_assets "$BUILD_DIR" "$HTML_FILE"

echo ""
echo "Build successful!"
echo ""
echo "Files generated:"
ls -lh "$BUILD_DIR/${OUTPUT_NAME}."{wasm,js} \
       "$BUILD_DIR/$HTML_FILE" \
       "$BUILD_DIR/events.js" \
       "$BUILD_DIR/savemanager.js" \
       "$BUILD_DIR/filedialog.js" \
       "$BUILD_DIR/filedialog.css" \
       "$BUILD_DIR/loading-overlay.js"
echo ""
echo "To test:"
echo "  cd $BUILD_DIR && python -m http.server 8081"
echo "  Then open: http://localhost:8081/$HTML_FILE"
