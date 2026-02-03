#include <stdio.h>
#include <emscripten.h>

int main(int argc, char** argv) {
    printf("BoE WASM initialized\n");
    printf("Blades of Exile - WebAssembly Port\n");
    printf("Emscripten Version: %d.%d.%d\n",
           __EMSCRIPTEN_major__,
           __EMSCRIPTEN_minor__,
           __EMSCRIPTEN_tiny__);

    EM_ASM({
        console.log('BoE WASM module loaded successfully!');
    });

    return 0;
}
