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
    # "$WEB_DIR/minimal_game.cpp"  # No longer needed - using real main() from boe.main.cpp
    "$WEB_DIR/web_stubs.cpp"  # Temporary stubs for missing functions
    "$SOURCE_DIR/location.cpp"
    "$SOURCE_DIR/mathutil.cpp"
    "$SOURCE_DIR/damage.cpp"
    "$SOURCE_DIR/universe/universe.cpp"
    "$SOURCE_DIR/universe/party.cpp"
    "$SOURCE_DIR/universe/pc.cpp"
    "$SOURCE_DIR/universe/living.cpp"
    "$SOURCE_DIR/scenario/item.cpp"
    "$SOURCE_DIR/scenario/monster.cpp"
    "$SOURCE_DIR/scenario/town.cpp"
    "$SOURCE_DIR/scenario/terrain.cpp"
    "$SOURCE_DIR/scenario/scenario.cpp"
    "$SOURCE_DIR/scenario/outdoors.cpp"
    "$SOURCE_DIR/scenario/shop.cpp"
    "$SOURCE_DIR/scenario/talking.cpp"
    "$SOURCE_DIR/scenario/special.cpp"
    "$SOURCE_DIR/scenario/special-general.cpp"
    "$SOURCE_DIR/scenario/special-affect.cpp"
    "$SOURCE_DIR/scenario/special-condition.cpp"
    "$SOURCE_DIR/scenario/special-oneshot.cpp"
    "$SOURCE_DIR/scenario/special-town.cpp"
    "$SOURCE_DIR/scenario/special-rect.cpp"
    "$SOURCE_DIR/scenario/special-outdoor.cpp"
    "$SOURCE_DIR/fileio/tagfile.cpp"
    "$SOURCE_DIR/fileio/fileio.cpp"
    "$SOURCE_DIR/fileio/fileio_scen.cpp"
    "$SOURCE_DIR/fileio/fileio_party.cpp"
    "$SOURCE_DIR/universe/creature.cpp"
    "$SOURCE_DIR/scenario/quest.cpp"
    "$SOURCE_DIR/scenario/vehicle.cpp"
    "$SOURCE_DIR/game/boe.items.cpp"
    "$SOURCE_DIR/game/boe.monster.cpp"
    "$SOURCE_DIR/game/boe.party.cpp"
    "$SOURCE_DIR/game/boe.combat.cpp"
    "$SOURCE_DIR/game/boe.town.cpp"
    "$SOURCE_DIR/game/boe.specials.cpp"
    "$SOURCE_DIR/game/boe.text.cpp"
    "$SOURCE_DIR/game/boe.locutils.cpp"
    "$SOURCE_DIR/game/boe.graphics.cpp"
    "$SOURCE_DIR/game/boe.newgraph.cpp"
    "$SOURCE_DIR/game/boe.graphutil.cpp"
    "$SOURCE_DIR/game/boe.infodlg.cpp"
    "$SOURCE_DIR/game/boe.actions.cpp"
    "$SOURCE_DIR/game/boe.fileio.cpp"
    "$SOURCE_DIR/game/boe.main.cpp"
    "$SOURCE_DIR/game/boe.dlgutil.cpp"
    # "$SOURCE_DIR/game/boe.menu.cpp"  # Requires TGUI - Phase 7
    "$SOURCE_DIR/game/boe.startup.cpp"
    "$SOURCE_DIR/game/boe.townspec.cpp"
    "$SOURCE_DIR/game/boe.ui.cpp"
    "$SOURCE_DIR/universe/population.cpp"
    "$SOURCE_DIR/gfx/gfxsheets.cpp"
    "$SOURCE_DIR/gfx/render_image.cpp"
    "$SOURCE_DIR/gfx/render_shapes.cpp"
    "$SOURCE_DIR/gfx/render_text.cpp"
    "$SOURCE_DIR/pattern.cpp"
    "$SOURCE_DIR/tools/framerate_limiter.cpp"
    "$SOURCE_DIR/dialogxml/dialogs/dialog.cpp"
    "$SOURCE_DIR/dialogxml/dialogs/choicedlog.cpp"
    "$SOURCE_DIR/dialogxml/dialogs/pictchoice.cpp"
    "$SOURCE_DIR/dialogxml/widgets/pict.cpp"
    "$SOURCE_DIR/dialogxml/widgets/control.cpp"
    "$SOURCE_DIR/dialogxml/widgets/message.cpp"
    "$SOURCE_DIR/dialogxml/widgets/led.cpp"
    "$SOURCE_DIR/dialogxml/widgets/ledgroup.cpp"
    "$SOURCE_DIR/dialogxml/widgets/container.cpp"
    "$SOURCE_DIR/dialogxml/widgets/scrollbar.cpp"
    "$SOURCE_DIR/dialogxml/widgets/button.cpp"
    "$SOURCE_DIR/dialogxml/keycodes.cpp"
    "$SOURCE_DIR/sounds.cpp"
    "$SOURCE_DIR/fileio/estreams.cpp"  # Now has ptr_set compatibility layer
    "$SOURCE_DIR/spell.cpp"
    "$SOURCE_DIR/enchant.cpp"
    "$SOURCE_DIR/alchemy.cpp"
    "$SOURCE_DIR/utility.cpp"
    "$SOURCE_DIR/fileio/tarball.cpp"
    "$SOURCE_DIR/fileio/map_parse.cpp"
    "$SOURCE_DIR/fileio/resmgr/res_cursor.cpp"
    "$SOURCE_DIR/fileio/resmgr/res_dialog.cpp"
    "$SOURCE_DIR/fileio/resmgr/res_font.cpp"
    "$SOURCE_DIR/fileio/resmgr/res_image.cpp"
    "$SOURCE_DIR/fileio/resmgr/res_sound.cpp"
    "$SOURCE_DIR/fileio/resmgr/res_strings.cpp"
    "$SOURCE_DIR/fileio/gzstream/gzstream.cpp"
    "$SOURCE_DIR/fileio/xml-parser/ticpp.cpp"
    "$SOURCE_DIR/fileio/xml-parser/tinystr.cpp"
    "$SOURCE_DIR/fileio/xml-parser/tinyxml.cpp"
    "$SOURCE_DIR/fileio/xml-parser/tinyxmlerror.cpp"
    "$SOURCE_DIR/fileio/xml-parser/tinyxmlparser.cpp"
    # "$SOURCE_DIR/tools/replay.cpp"  # Requires cppcodec - Phase 8 (not critical for initial build)
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
    "-I$SOURCE_DIR/fileio/gzstream"
    "-I$SOURCE_DIR/dialogxml"
)

# Build flags - keep it simple
EMCC_FLAGS=(
    "-sWASM=1"
    "-sALLOW_MEMORY_GROWTH=1"
    "-sEXPORTED_FUNCTIONS=[\"_main\",\"_malloc\",\"_push_mouse_event\",\"_push_key_event\"]"
    "-sEXPORTED_RUNTIME_METHODS=[\"ccall\",\"cwrap\"]"
    "-sNO_EXIT_RUNTIME=1"
    "-sSTRICT=0"
    "-sDISABLE_EXCEPTION_CATCHING=0"
    "-DTIXML_USE_TICPP"
    "--preload-file"
    "data@/data"
    "--preload-file"
    "rsrc/scenarios@/Blades of Exile Scenarios"
    "-g"
    "-std=c++17"
    "-lembind"
    "-sASYNCIFY"
    "-sASYNCIFY_STACK_SIZE=65536"
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

    # Copy events.js to build directory
    cp "$WEB_DIR/events.js" "$BUILD_DIR/events.js"
    echo "Copied events.js to $BUILD_DIR"

    # Inject events.js script tag into generated HTML (before boe_minimal.js)
    sed -i 's|<script async type="text/javascript" src="boe_minimal.js"></script>|<script type="text/javascript" src="events.js"></script>\n    <script async type="text/javascript" src="boe_minimal.js"></script>|' "$BUILD_DIR/boe_minimal.html"
    echo "Injected events.js into HTML"

    echo ""
    echo "Files generated:"
    ls -lh "$BUILD_DIR/boe_minimal."{wasm,js,html} "$BUILD_DIR/events.js"
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
