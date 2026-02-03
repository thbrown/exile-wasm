/*
 * minimal_game.cpp
 * Minimal BoE game build for WebAssembly
 *
 * Goal: Compile core game code and identify all web porting blockers
 */

#include <stdio.h>
#include <emscripten.h>
#include <emscripten/bind.h>

// Start with just the global header
#include "../src/game/boe.global.hpp"

using namespace emscripten;

bool game_running = false;

// Minimal initialization - just try to get the game headers compiling
void init_game() {
    printf("=== BoE WebAssembly - Minimal Game Build ===\n");
    printf("Goal: Identify porting blockers\n\n");

    printf("Testing core includes:\n");
    printf("  boe.global.hpp: OK\n");
    printf("  cUniverse defined: %s\n", sizeof(cUniverse) > 0 ? "YES" : "NO");

    game_running = true;
    printf("\nGame initialized!\n");
}

// Minimal main loop - one iteration
void main_loop() {
    if (!game_running) return;

    // For now, just print once
    static bool printed = false;
    if (!printed) {
        printf("Main loop running...\n");
        printed = true;
    }
}

int main(int argc, char** argv) {
    printf("Starting minimal BoE build...\n");

    init_game();

    // Set up Emscripten main loop
    emscripten_set_main_loop(main_loop, 30, 0);

    return 0;
}

EMSCRIPTEN_BINDINGS(minimal_game) {
    function("init_game", &init_game);
    function("main_loop", &main_loop);
}
