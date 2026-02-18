#!/bin/bash

# Prod build: optimized -> docs/index.html (for GitHub Pages)
# Usage: bash build_wasm_prod.sh

set -e
source "$(dirname "$0")/build_wasm_common.sh"

BUILD_DIR="docs"
OUTPUT_NAME="boe"

mkdir -p "$BUILD_DIR"

echo "=== Building Blades of Exile WASM [PROD] ==="
echo "Output: $BUILD_DIR/index.html"
echo ""

EMCC_FLAGS=("${EMCC_BASE_FLAGS[@]}" "-O3" "-sLZ4=1")

"$PYTHON_EXE" "$EMCC" \
    "${SOURCES[@]}" \
    "${INCLUDES[@]}" \
    "${EMCC_FLAGS[@]}" \
    -o "$BUILD_DIR/$OUTPUT_NAME.html"

# Rename to index.html for GitHub Pages
mv "$BUILD_DIR/$OUTPUT_NAME.html" "$BUILD_DIR/index.html"

# Run post-build with a temp name so do_post_build can find the right sed pattern
# We've already renamed html, so fix it up manually
cp "$WEB_DIR/events.js"      "$BUILD_DIR/events.js"
cp "$WEB_DIR/savemanager.js" "$BUILD_DIR/savemanager.js"
cp "$WEB_DIR/filedialog.js"  "$BUILD_DIR/filedialog.js"
cp "$WEB_DIR/filedialog.css" "$BUILD_DIR/filedialog.css"
mkdir -p "$BUILD_DIR/rsrc/cursors"
cp rsrc/cursors/*.gif "$BUILD_DIR/rsrc/cursors/"

# Match both verbose (-O0) and minified (-O3) script tag formats
sed -E -i "s|<script[^>]*src=[\"']?${OUTPUT_NAME}\.js[\"']?[^>]*></script>|<script>var wasmScriptUrl = \"${OUTPUT_NAME}.js\";</script>|" \
    "$BUILD_DIR/index.html"

echo "Copied web assets and cursors to $BUILD_DIR"
echo "Modified HTML to enable splash screen with on-demand loading"
echo ""
echo "Files generated:"
ls -lh "$BUILD_DIR/${OUTPUT_NAME}."{wasm,js} \
       "$BUILD_DIR/index.html" \
       "$BUILD_DIR/events.js" \
       "$BUILD_DIR/savemanager.js" \
       "$BUILD_DIR/filedialog.js" \
       "$BUILD_DIR/filedialog.css"
echo ""
echo "To test:"
echo "  cd $BUILD_DIR && python -m http.server 8082"
echo "  Then open: http://localhost:8082/index.html"
echo ""
echo "To deploy: commit docs/ to git and push to GitHub Pages branch"
