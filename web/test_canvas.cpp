/*
 * test_canvas.cpp
 * BoE - Canvas 2D Rendering Test
 *
 * Tests the Canvas 2D abstraction layer
 */

#include <stdio.h>
#include <string>
#include <memory>
#include <emscripten.h>

// Forward declare to avoid including full render_target.hpp with SFML dependencies
class IRenderWindow;
class IRenderTexture;
struct Color {
    unsigned char r, g, b, a;
    Color(unsigned char r, unsigned char g, unsigned char b, unsigned char a = 255)
        : r(r), g(g), b(b), a(a) {}
};

// Factory functions
std::unique_ptr<IRenderWindow> createRenderWindow(unsigned int width, unsigned int height, const std::string& title);
std::unique_ptr<IRenderTexture> createRenderTexture(unsigned int width, unsigned int height);

// IRenderWindow interface (minimal for testing)
class IRenderWindow {
public:
    virtual ~IRenderWindow() = default;
    virtual void clear(const Color& color) = 0;
    virtual void display() = 0;
    virtual unsigned int getWidth() const = 0;
    virtual unsigned int getHeight() const = 0;
    virtual bool isOpen() const = 0;
};

// IRenderTexture interface (minimal for testing)
class IRenderTexture {
public:
    virtual ~IRenderTexture() = default;
    virtual void clear(const Color& color) = 0;
    virtual void display() = 0;
    virtual unsigned int getWidth() const = 0;
    virtual unsigned int getHeight() const = 0;
    virtual int getCanvasId() const = 0;
};

// Global window for rendering
std::unique_ptr<IRenderWindow> g_window;
std::unique_ptr<IRenderTexture> g_offscreenTexture;
int frame = 0;

void main_loop() {
    if (!g_window || !g_window->isOpen()) {
        return;
    }

    // Clear the main window
    g_window->clear(Color(30, 30, 40, 255));

    // Draw some test graphics directly via EM_ASM
    // (We'll implement higher-level drawing functions later)

    // Draw a rectangle
    EM_ASM({
        var r = 100 + Math.sin($0 / 30.0) * 50;
        var g = 150;
        var b = 200 + Math.cos($0 / 40.0) * 50;
        BoE.Canvas.fillRect(-1, 100, 100, 200, 150, r, g, b, 255);
    }, frame);

    // Draw a circle
    EM_ASM({
        var x = 400 + Math.cos($0 / 50.0) * 100;
        var y = 300 + Math.sin($0 / 50.0) * 100;
        BoE.Canvas.fillCircle(-1, x, y, 50, 255, 200, 100, 255);
    }, frame);

    // Draw text
    EM_ASM({
        var text = 'Frame: ' + $0;
        BoE.Canvas.drawText(-1, text, 20, 40, 'Arial', 24, 255, 255, 255, 255);
    }, frame);

    // Draw a line
    EM_ASM({
        BoE.Canvas.drawLine(-1, 50, 500, 900, 500, 100, 255, 100, 255, 3);
    });

    // Test off-screen canvas
    if (g_offscreenTexture) {
        g_offscreenTexture->clear(Color(50, 100, 150, 255));

        EM_ASM({
            // Draw something on the off-screen canvas
            BoE.Canvas.fillRect($0, 10, 10, 100, 100, 255, 255, 0, 255);
            BoE.Canvas.fillCircle($0, 80, 80, 30, 255, 0, 255, 255);

            // Copy off-screen canvas to main canvas
            BoE.Canvas.drawCanvas(-1, $0, 0, 0, 150, 150, 600, 100, 150, 150);
        }, g_offscreenTexture->getCanvasId());
    }

    g_window->display();
    frame++;
}

int main(int argc, char** argv) {
    printf("BoE Canvas 2D Test\n");
    printf("Phase 3: Canvas 2D Rendering Implementation\n\n");

    // Create render window
    g_window = createRenderWindow(960, 640, "BoE Canvas Test");

    if (!g_window) {
        printf("ERROR: Failed to create render window!\n");
        return 1;
    }

    printf("Render window created: %dx%d\n", g_window->getWidth(), g_window->getHeight());

    // Create off-screen texture
    g_offscreenTexture = createRenderTexture(150, 150);

    if (!g_offscreenTexture) {
        printf("ERROR: Failed to create render texture!\n");
        return 1;
    }

    printf("Render texture created: %dx%d\n",
           g_offscreenTexture->getWidth(),
           g_offscreenTexture->getHeight());

    printf("\nStarting render loop...\n");

    // Set up the main loop
    emscripten_set_main_loop(main_loop, 30, 1);

    return 0;
}
