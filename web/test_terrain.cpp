/*
 * test_terrain.cpp
 * BoE - Terrain Tile Rendering Test
 *
 * Tests loading and displaying real game terrain tiles
 */

#include <stdio.h>
#include <string>
#include <memory>
#include <emscripten.h>
#include <emscripten/bind.h>
#include <emscripten/em_js.h>
#include "render_target_canvas.hpp"

using namespace emscripten;

// Terrain tile constants
const int TILE_SIZE = 28;  // BoE uses 28x28 tiles
const int TILES_PER_ROW = 10;  // Terrain sheets have 10 tiles per row
const int GRID_WIDTH = 20;   // Display a 20x15 grid
const int GRID_HEIGHT = 15;

// Factory functions
std::unique_ptr<IRenderWindow> createRenderWindow(unsigned int width, unsigned int height, const std::string& title);
std::unique_ptr<ITexture> createTexture();

// JavaScript rendering function
EM_JS(void, render_game_scene, (int frameNum, int terrainTexId, int charTexId, int gridW, int gridH, int tileSize, int charX, int charY), {
    var ctx = BoE.Canvas.getContext(-1);

    // Draw title
    ctx.fillStyle = 'white';
    ctx.font = '24px Arial';
    ctx.fillText('BoE Terrain Test - Frame: ' + frameNum, 20, 30);

    // Draw terrain grid
    if (terrainTexId >= 0) {
        var terrainImg = BoE.Canvas.images[terrainTexId];

        if (terrainImg && terrainImg.loaded) {
            // Info
            ctx.fillStyle = 'rgb(200,255,200)';
            ctx.font = '16px Arial';
            ctx.fillText('Terrain Sheet: ' + terrainImg.width + 'x' + terrainImg.height +
                        ' (' + gridW + 'x' + gridH + ' grid)', 20, 55);

            // Calculate tiles per row in the source image
            var tilesPerRow = Math.floor(terrainImg.width / tileSize);
            var tilesPerCol = Math.floor(terrainImg.height / tileSize);
            var totalTiles = tilesPerRow * tilesPerCol;

            ctx.fillText('Tiles: ' + totalTiles + ' (' + tilesPerRow + 'x' + tilesPerCol +
                        ', ' + tileSize + 'x' + tileSize + ' each)', 20, 75);

            // Draw grid of terrain tiles
            var startY = 100;
            var tileIndex = 0;

            for (var y = 0; y < gridH; y++) {
                for (var x = 0; x < gridW; x++) {
                    // Calculate source tile position
                    var srcX = (tileIndex % tilesPerRow) * tileSize;
                    var srcY = Math.floor(tileIndex / tilesPerRow) * tileSize;

                    // Calculate destination position
                    var destX = x * tileSize;
                    var destY = startY + (y * tileSize);

                    // Draw tile
                    if (srcY + tileSize <= terrainImg.height) {
                        ctx.drawImage(terrainImg.element,
                                    srcX, srcY, tileSize, tileSize,
                                    destX, destY, tileSize, tileSize);
                    }

                    tileIndex++;
                    if (tileIndex >= totalTiles) tileIndex = 0;
                }
            }

            // Draw grid lines for clarity
            ctx.strokeStyle = 'rgba(255,255,255,0.2)';
            ctx.lineWidth = 1;
            for (var x = 0; x <= gridW; x++) {
                ctx.beginPath();
                ctx.moveTo(x * tileSize, startY);
                ctx.lineTo(x * tileSize, startY + gridH * tileSize);
                ctx.stroke();
            }
            for (var y = 0; y <= gridH; y++) {
                ctx.beginPath();
                ctx.moveTo(0, startY + y * tileSize);
                ctx.lineTo(gridW * tileSize, startY + y * tileSize);
                ctx.stroke();
            }

            // Draw character sprite on top
            if (charTexId >= 0) {
                var charImg = BoE.Canvas.images[charTexId];
                if (charImg && charImg.loaded) {
                    // Draw first character sprite (0,0 in the sheet)
                    // BoE character sprites are typically 28x36
                    var charTileW = 28;
                    var charTileH = 36;

                    // Animate by bobbing slightly
                    var bobOffset = Math.sin(frameNum * 0.1) * 2;

                    ctx.drawImage(charImg.element,
                                0, 0, charTileW, charTileH,  // Source
                                charX, charY + bobOffset, charTileW, charTileH);  // Dest

                    // Draw character info and controls
                    ctx.fillStyle = 'rgb(255,255,200)';
                    ctx.font = '14px Arial';
                    ctx.fillText('Character at (' + charX + ', ' + Math.floor(charY + bobOffset) + ')', 350, 55);
                    ctx.fillText('Use Arrow Keys to move!', 350, 75);
                }
            }
        } else {
            ctx.fillStyle = 'yellow';
            ctx.fillText('Waiting for terrain sheet to load...', 20, 100);
        }
    } else {
        ctx.fillStyle = 'red';
        ctx.fillText('No terrain texture loaded', 20, 100);
    }
});

// Global state
std::unique_ptr<IRenderWindow> g_window;
std::unique_ptr<ITexture> g_terrain_sheet;
std::unique_ptr<ITexture> g_char_sheet;  // Character sprites
int frame = 0;
int char_x = 280;  // Character position
int char_y = 260;
const int MOVE_SPEED = 4;  // Pixels per keypress

void main_loop() {
    if (!g_window) return;

    // Clear
    g_window->clear(Color(20, 30, 20, 255));  // Dark green background

    // Get texture IDs
    int terrainId = -1;
    if (g_terrain_sheet) {
        CanvasTexture* tex = static_cast<CanvasTexture*>(g_terrain_sheet.get());
        terrainId = tex->getImageId();
    }

    int charId = -1;
    if (g_char_sheet) {
        CanvasTexture* tex = static_cast<CanvasTexture*>(g_char_sheet.get());
        charId = tex->getImageId();
    }

    // Render game scene
    render_game_scene(frame, terrainId, charId, GRID_WIDTH, GRID_HEIGHT, TILE_SIZE, char_x, char_y);

    g_window->display();
    frame++;
}

void loadTerrainSheet() {
    printf("Loading terrain sheet from /rsrc/graphics/ter1.png...\n");

    g_terrain_sheet = createTexture();
    if (!g_terrain_sheet) {
        printf("ERROR: Failed to create texture object!\n");
        return;
    }

    if (!g_terrain_sheet->loadFromFile("/rsrc/graphics/ter1.png")) {
        printf("ERROR: Failed to load terrain sheet!\n");
        g_terrain_sheet.reset();
        return;
    }

    printf("Terrain sheet loaded successfully!\n");
}

void loadCharacterSheet() {
    printf("Loading character sheet from /rsrc/graphics/pcs.png...\n");

    g_char_sheet = createTexture();
    if (!g_char_sheet) {
        printf("ERROR: Failed to create character texture object!\n");
        return;
    }

    if (!g_char_sheet->loadFromFile("/rsrc/graphics/pcs.png")) {
        printf("ERROR: Failed to load character sheet!\n");
        g_char_sheet.reset();
        return;
    }

    printf("Character sheet loaded successfully!\n");
}

// Keyboard input handler
void handleKeyPress(const std::string& key) {
    const int grid_start_y = 100;  // Where the terrain grid starts
    const int grid_end_y = grid_start_y + (GRID_HEIGHT * TILE_SIZE) - 36;  // Character height is 36
    const int grid_end_x = (GRID_WIDTH * TILE_SIZE) - 28;  // Character width is 28

    if (key == "ArrowUp") {
        char_y -= MOVE_SPEED;
        if (char_y < grid_start_y) char_y = grid_start_y;
    } else if (key == "ArrowDown") {
        char_y += MOVE_SPEED;
        if (char_y > grid_end_y) char_y = grid_end_y;
    } else if (key == "ArrowLeft") {
        char_x -= MOVE_SPEED;
        if (char_x < 0) char_x = 0;
    } else if (key == "ArrowRight") {
        char_x += MOVE_SPEED;
        if (char_x > grid_end_x) char_x = grid_end_x;
    }

    printf("Key: %s, Char pos: (%d, %d)\n", key.c_str(), char_x, char_y);
}

int main(int argc, char** argv) {
    printf("BoE Terrain Rendering Test\n");
    printf("Phase 4: Real Game Graphics Integration\n\n");

    // Create render window
    g_window = createRenderWindow(560, 520, "BoE Terrain Test");

    if (!g_window) {
        printf("ERROR: Failed to create render window!\n");
        return 1;
    }

    printf("Render window created: %dx%d\n", g_window->getWidth(), g_window->getHeight());

    // Load graphics after a delay (to ensure VFS is ready)
    printf("Attempting to load game graphics...\n");
    EM_ASM({
        setTimeout(function() {
            console.log('Loading terrain and character sheets...');
            Module.loadTerrainSheet();
            Module.loadCharacterSheet();
        }, 100);
    });

    printf("\nStarting render loop...\n");

    // Set up keyboard input
    EM_ASM({
        console.log('Setting up keyboard input...');
        document.addEventListener('keydown', function(e) {
            // Prevent default for arrow keys (stop page scrolling)
            if (e.key.startsWith('Arrow')) {
                e.preventDefault();
                Module.handleKeyPress(e.key);
            }
        });
        console.log('Keyboard input ready. Use arrow keys to move the character!');
    });

    // Set up continuous render loop
    EM_ASM({
        function renderLoop() {
            try {
                Module.main_loop();
            } catch(e) {
                console.error('Error in main_loop:', e);
                return;
            }
            setTimeout(renderLoop, 1000/30);  // 30 FPS
        }
        setTimeout(renderLoop, 100);
    });

    return 0;
}

// Export functions via embind
EMSCRIPTEN_BINDINGS(terrain_module) {
    function("main_loop", &main_loop);
    function("loadTerrainSheet", &loadTerrainSheet);
    function("loadCharacterSheet", &loadCharacterSheet);
    function("handleKeyPress", &handleKeyPress);
}
