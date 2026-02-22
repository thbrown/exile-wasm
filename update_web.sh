#!/bin/bash
# Fast web asset update — copies JS/CSS files and patches the HTML without recompiling WASM.
# Usage: bash update_web.sh [build_dir] [html_file]
#   build_dir  default: build/web_game
#   html_file  default: boe.html  (use "index.html" for the prod docs/ build)

set -e
source "$(dirname "$0")/build_wasm_common.sh"

BUILD_DIR="${1:-build/web_game}"
HTML_FILE="${2:-boe.html}"

if [ ! -f "$BUILD_DIR/$HTML_FILE" ]; then
    echo "ERROR: $BUILD_DIR/$HTML_FILE not found. Run a full build first."
    exit 1
fi

copy_web_assets "$BUILD_DIR" "$HTML_FILE"
echo "Web assets updated in $BUILD_DIR"
