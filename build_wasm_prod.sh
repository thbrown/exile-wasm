#!/bin/bash

# Prod build: optimized -> docs/index.html (for GitHub Pages)
# Usage: bash build_wasm_prod.sh

set -e
source "$(dirname "$0")/build_wasm_common.sh"

BUILD_DIR="docs"
OUTPUT_NAME="boe"
HTML_FILE="index.html"

# Clean generated files while preserving GitHub Pages metadata (CNAME, .nojekyll, etc.)
echo "Cleaning generated files from $BUILD_DIR..."
mkdir -p "$BUILD_DIR"
find "$BUILD_DIR" -maxdepth 1 -type f \
    \( -name "*.html" -o -name "*.js" -o -name "*.wasm" -o -name "*.data" -o -name "*.css" \) \
    -delete
rm -rf "$BUILD_DIR/rsrc"

echo "=== Building Blades of Exile WASM [PROD] ==="
echo "Output: $BUILD_DIR/$HTML_FILE"
echo ""

EMCC_FLAGS=("${EMCC_BASE_FLAGS[@]}" "-O3" "-sLZ4=1")

"$PYTHON_EXE" "$EMCC" \
    "${SOURCES[@]}" \
    "${INCLUDES[@]}" \
    "${EMCC_FLAGS[@]}" \
    -o "$BUILD_DIR/$OUTPUT_NAME.html"

# Rename to index.html for GitHub Pages
mv "$BUILD_DIR/$OUTPUT_NAME.html" "$BUILD_DIR/$HTML_FILE"

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
echo "  cd $BUILD_DIR && python -m http.server 8082"
echo "  Then open: http://localhost:8082/$HTML_FILE"
echo ""
echo "To deploy: commit docs/ to git and push to GitHub Pages branch"
