/*
 * test_texture.cpp
 * BoE - Canvas 2D Texture Loading Test
 *
 * Tests texture loading and image rendering
 */

#include <stdio.h>
#include <string>
#include <memory>
#include <emscripten.h>
#include <emscripten/bind.h>
#include <emscripten/em_js.h>
#include "render_target_canvas.hpp"

using namespace emscripten;

// Define rendering function in JavaScript using EM_JS
EM_JS(void, render_frame, (int frameNum, int textureId), {
    var ctx = BoE.Canvas.getContext(-1);

    // Draw text
    ctx.fillStyle = 'white';
    ctx.font = '24px Arial';
    ctx.fillText('Texture Loading Test - Frame: ' + frameNum, 20, 40);

    // Draw border
    ctx.strokeStyle = 'rgb(100,255,100)';
    ctx.lineWidth = 2;
    ctx.strokeRect(10, 10, 940, 620);

    // Draw texture if available
    if (textureId >= 0) {
        var img = BoE.Canvas.images[textureId];

        // Debug info
        ctx.fillStyle = 'yellow';
        ctx.font = '14px Arial';
        ctx.fillText('Texture ID: ' + textureId + ', Img exists: ' + (img ? 'yes' : 'no'), 20, 100);

        if (img) {
            ctx.fillText('Loaded: ' + img.loaded + ', Size: ' + img.width + 'x' + img.height, 20, 120);

            if (img.loaded) {
                // Draw original size
                ctx.drawImage(img.element, 50, 150);
                // Draw scaled
                ctx.drawImage(img.element, 300, 150, img.width * 0.5, img.height * 0.5);

                // Info text
                ctx.fillStyle = 'rgb(200,200,255)';
                ctx.font = '18px Arial';
                ctx.fillText('Image rendering at 50,150 and 300,150', 20, 80);
            } else {
                ctx.fillText('Image still loading...', 20, 140);
            }
        }
    } else {
        ctx.fillStyle = 'red';
        ctx.font = '14px Arial';
        ctx.fillText('No texture (ID = ' + textureId + ')', 20, 100);
    }
});

// Factory functions (declarations - implementations in render_target_factory.cpp)
std::unique_ptr<IRenderWindow> createRenderWindow(unsigned int width, unsigned int height, const std::string& title);
std::unique_ptr<ITexture> createTexture();

// Global window and texture
std::unique_ptr<IRenderWindow> g_window;
std::unique_ptr<ITexture> g_texture;
int frame = 0;

void main_loop() {
    if (!g_window) return;

    // Clear the canvas
    g_window->clear(Color(20, 20, 30, 255));

    // Get texture ID - cast to CanvasTexture to avoid virtual dispatch issues
    int imgId = -1;
    if (g_texture) {
        // Direct cast to CanvasTexture
        CanvasTexture* canvasTex = static_cast<CanvasTexture*>(g_texture.get());
        imgId = canvasTex->getImageId();
    }

    // Render via JavaScript
    render_frame(frame, imgId);

    // Display the frame
    g_window->display();

    frame++;
}

int main(int argc, char** argv) {
    printf("BoE Texture Loading Test\n");
    printf("Phase 3: Canvas 2D Texture Implementation\n\n");

    // Create render window
    g_window = createRenderWindow(960, 640, "BoE Texture Test");

    if (!g_window) {
        printf("ERROR: Failed to create render window!\n");
        return 1;
    }

    printf("Render window created: %dx%d\n", g_window->getWidth(), g_window->getHeight());

    // Create a test image in the virtual filesystem
    printf("Creating test image data...\n");
    EM_ASM({
        // Create a simple test image using canvas
        var testCanvas = document.createElement('canvas');
        testCanvas.width = 100;
        testCanvas.height = 100;
        var ctx = testCanvas.getContext('2d');

        // Draw a gradient
        var gradient = ctx.createLinearGradient(0, 0, 100, 100);
        gradient.addColorStop(0, 'red');
        gradient.addColorStop(0.5, 'yellow');
        gradient.addColorStop(1, 'blue');
        ctx.fillStyle = gradient;
        ctx.fillRect(0, 0, 100, 100);

        // Draw a circle
        ctx.fillStyle = 'white';
        ctx.beginPath();
        ctx.arc(50, 50, 30, 0, Math.PI * 2);
        ctx.fill();

        // Draw text
        ctx.fillStyle = 'black';
        ctx.font = '20px Arial';
        ctx.textAlign = 'center';
        ctx.fillText('BoE', 50, 55);

        // Convert to data URL
        testCanvas.toBlob(function(blob) {
            var reader = new FileReader();
            reader.onload = function() {
                var data = new Uint8Array(reader.result);

                // Write to virtual filesystem
                if (!FS.analyzePath('/test').exists) {
                    FS.mkdir('/test');
                }
                FS.writeFile('/test/logo.png', data);
                console.log('Test image written to /test/logo.png');
            };
            reader.readAsArrayBuffer(blob);
        });
    });

    // Wait a moment for the image to be created, then load it
    printf("Waiting for test image to be created...\n");

    // Try to load the texture after a delay
    EM_ASM({
        setTimeout(function() {
            console.log('Attempting to load texture...');
            // Call via embind
            Module.loadTestTexture();
        }, 500);
    });

    printf("\nStarting render loop...\n");

    // Set up the main loop using JavaScript requestAnimationFrame
    EM_ASM({
        function renderLoop() {
            // Call the C++ main_loop function via embind
            try {
                Module.main_loop();
            } catch(e) {
                console.error('Error in main_loop:', e);
                return; // Stop on error
            }

            // Continue loop indefinitely at 30 FPS
            setTimeout(renderLoop, 1000/30);
        }

        // Start the loop after Module is ready
        setTimeout(renderLoop, 100);
    });

    return 0;
}

// Export function to load texture (called from JavaScript)
void loadTestTexture() {
        printf("Loading test texture from /test/logo.png...\n");

        g_texture = createTexture();
        if (!g_texture) {
            printf("ERROR: Failed to create texture object!\n");
            return;
        }

        if (!g_texture->loadFromFile("/test/logo.png")) {
            printf("ERROR: Failed to load texture from file!\n");
            g_texture.reset();
            return;
        }

    printf("Texture loaded successfully: %dx%d\n",
           g_texture->getWidth(), g_texture->getHeight());
}

// Export functions using embind
EMSCRIPTEN_BINDINGS(boe_module) {
    function("main_loop", &main_loop);
    function("loadTestTexture", &loadTestTexture);
}
