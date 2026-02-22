#!/bin/bash

# Shared WASM build configuration for Blades of Exile
# Source this file from build_wasm_dev.sh or build_wasm_prod.sh

# -------------------------------------------------------
# Tool paths
# -------------------------------------------------------
EMSDK_ROOT="/d/gitRepos/emsdk"
PYTHON_EXE="$EMSDK_ROOT/python/3.13.3_64bit/python.exe"
EMCC="$EMSDK_ROOT/upstream/emscripten/emcc.py"
SOURCE_DIR="src"
WEB_DIR="web"

# -------------------------------------------------------
# Source files
# -------------------------------------------------------
SOURCES=(
    "$WEB_DIR/web_stubs.cpp"
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
    "$SOURCE_DIR/game/boe.menu.cpp"
    "$WEB_DIR/boe.menus.wasm.cpp"
    "$SOURCE_DIR/pcedit/pc.editors.cpp"
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
    "$SOURCE_DIR/tools/drawable_manager.cpp"
    "$SOURCE_DIR/dialogxml/dialogs/dialog.cpp"
    "$SOURCE_DIR/dialogxml/dialogs/choicedlog.cpp"
    "$SOURCE_DIR/dialogxml/dialogs/pictchoice.cpp"
    "$SOURCE_DIR/dialogxml/dialogs/3choice.cpp"
    "$SOURCE_DIR/dialogxml/dialogs/strdlog.cpp"
    "$SOURCE_DIR/dialogxml/widgets/pict.cpp"
    "$SOURCE_DIR/dialogxml/widgets/control.cpp"
    "$SOURCE_DIR/dialogxml/widgets/message.cpp"
    "$SOURCE_DIR/dialogxml/widgets/led.cpp"
    "$SOURCE_DIR/dialogxml/widgets/ledgroup.cpp"
    "$SOURCE_DIR/dialogxml/widgets/container.cpp"
    "$SOURCE_DIR/dialogxml/widgets/scrollbar.cpp"
    "$SOURCE_DIR/dialogxml/widgets/button.cpp"
    "$SOURCE_DIR/dialogxml/widgets/basicbtns.cpp"
    "$SOURCE_DIR/dialogxml/widgets/field.cpp"
    "$SOURCE_DIR/dialogxml/widgets/stack.cpp"
    "$SOURCE_DIR/dialogxml/widgets/line.cpp"
    "$SOURCE_DIR/dialogxml/widgets/scrollpane.cpp"
    "$SOURCE_DIR/dialogxml/widgets/tilemap.cpp"
    "$SOURCE_DIR/dialogxml/keycodes.cpp"
    "$SOURCE_DIR/sounds.cpp"
    "$SOURCE_DIR/fileio/estreams.cpp"
    "$SOURCE_DIR/spell.cpp"
    "$SOURCE_DIR/enchant.cpp"
    "$SOURCE_DIR/alchemy.cpp"
    "$SOURCE_DIR/utility.cpp"
    "$SOURCE_DIR/view_dialogs.cpp"
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
    "$SOURCE_DIR/porting.cpp"
)

# -------------------------------------------------------
# Include directories
# -------------------------------------------------------
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
    "-I$SOURCE_DIR/pcedit"
)

# -------------------------------------------------------
# Base compiler flags (no optimization flags - set in dev/prod scripts)
# -------------------------------------------------------
EMCC_BASE_FLAGS=(
    "-sWASM=1"
    "-sALLOW_MEMORY_GROWTH=1"
    "-sEXPORTED_FUNCTIONS=[\"_main\",\"_malloc\",\"_push_mouse_event\",\"_push_key_event\",\"_set_dialog_result\",\"_sync_save_to_indexeddb\",\"_save_party_to_autosave\",\"_wasm_load_from_path\",\"_wasm_menu_action\",\"_wasm_queue_menu_action\",\"_wasm_get_spell_list_json\",\"_wasm_cast_spell\",\"_wasm_queue_spell_cast\"]"
    "-sEXPORTED_RUNTIME_METHODS=[\"ccall\",\"cwrap\"]"
    "-sNO_EXIT_RUNTIME=1"
    "-sSTRICT=0"
    "-sDISABLE_EXCEPTION_CATCHING=0"
    "-DTIXML_USE_TICPP"
    "-DBOOST_FALLTHROUGH=[[fallthrough]]"
    "--preload-file" "data@/data"
    "--preload-file" "rsrc/scenarios@/Blades of Exile Scenarios"
    "--preload-file" "rsrc/dialogs@/rsrc/dialogs"
    "--preload-file" "rsrc/cursors@/rsrc/cursors"
    "--preload-file" "data/sounds@/data/sounds"
    "-std=c++17"
    "-lembind"
    "-sASYNCIFY"
    "-sSTACK_SIZE=16777216"
    "-sUSE_ZLIB=1"
    "-sASYNCIFY_STACK_SIZE=2097152"
    "--shell-file" "web/shell.html"
)

# -------------------------------------------------------
# Copy web assets to BUILD_DIR (idempotent, safe to call without recompiling WASM)
# Usage: copy_web_assets <BUILD_DIR> <HTML_FILE>
# HTML_FILE is just the filename, e.g. "boe.html" or "index.html"
# -------------------------------------------------------
copy_web_assets() {
    local BUILD_DIR="$1"
    local HTML_FILE="$2"

    cp "$WEB_DIR/events.js"           "$BUILD_DIR/events.js"
    cp "$WEB_DIR/savemanager.js"      "$BUILD_DIR/savemanager.js"
    cp "$WEB_DIR/filedialog.js"       "$BUILD_DIR/filedialog.js"
    cp "$WEB_DIR/filedialog.css"      "$BUILD_DIR/filedialog.css"
    cp "$WEB_DIR/menu.js"             "$BUILD_DIR/menu.js"
    cp "$WEB_DIR/loading-overlay.js"  "$BUILD_DIR/loading-overlay.js"

    mkdir -p "$BUILD_DIR/rsrc/cursors"
    cp rsrc/cursors/*.gif "$BUILD_DIR/rsrc/cursors/"

    # Inject loading-overlay.js before </body> (idempotent)
    if ! grep -q 'loading-overlay.js' "$BUILD_DIR/$HTML_FILE"; then
        sed -i 's|</body>|<script type="text/javascript" src="loading-overlay.js"></script>\n</body>|' \
            "$BUILD_DIR/$HTML_FILE"
    fi
}
