/*
 * render_target_canvas.cpp
 * BoE - Canvas 2D Implementation of Render Target
 *
 * Phase 3 Implementation - Canvas 2D backend
 */

#include "render_target_canvas.hpp"
#include <emscripten.h>
#include <emscripten/html5.h>

// For web builds, we need rectangle type but don't want full SFML dependencies
// Use a simple version for now
#ifdef USE_SFML
    #include "location.hpp"
#else
    // Simple rectangle structure for web
    struct rectangle {
        int left, top, right, bottom;
        rectangle() : left(0), top(0), right(0), bottom(0) {}
        rectangle(int l, int t, int r, int b) : left(l), top(t), right(r), bottom(b) {}
        int width() const { return right - left; }
        int height() const { return bottom - top; }
    };
#endif

// ========== CanvasRenderTexture Implementation (STUB) ==========

CanvasRenderTexture::CanvasRenderTexture()
    : width(0), height(0), canvasId(-1) {
}

CanvasRenderTexture::CanvasRenderTexture(unsigned int w, unsigned int h)
    : width(w), height(h), canvasId(-1) {
    create(w, h);
}

CanvasRenderTexture::~CanvasRenderTexture() {
    // Canvas cleanup handled by JavaScript garbage collection
    // Could explicitly delete if needed
}

void CanvasRenderTexture::clear(const Color& color) {
    if (canvasId < 0) return;

    EM_ASM({
        BoE.Canvas.clear($0, $1, $2, $3, $4);
    }, canvasId, color.r, color.g, color.b, color.a);
}

void CanvasRenderTexture::display() {
    // For Canvas 2D, display() is typically a no-op
    // All drawing is immediate
}

unsigned int CanvasRenderTexture::getWidth() const {
    return width;
}

unsigned int CanvasRenderTexture::getHeight() const {
    return height;
}

void CanvasRenderTexture::setView(rectangle rect) {
    // View transformation for Canvas 2D
    // This would require transform matrices, implementing as clip for now
    if (canvasId < 0) return;

    int w = rect.width();
    int h = rect.height();

    EM_ASM({
        BoE.Canvas.save($0);
        BoE.Canvas.setClipRect($0, $1, $2, $3, $4);
    }, canvasId, rect.left, rect.top, w, h);
}

void CanvasRenderTexture::resetView() {
    if (canvasId < 0) return;

    EM_ASM({
        BoE.Canvas.restore($0);
    }, canvasId);
}

void CanvasRenderTexture::setClipRect(rectangle rect) {
    if (canvasId < 0) return;

    int w = rect.width();
    int h = rect.height();

    EM_ASM({
        BoE.Canvas.setClipRect($0, $1, $2, $3, $4);
    }, canvasId, rect.left, rect.top, w, h);
}

void CanvasRenderTexture::resetClipRect() {
    if (canvasId < 0) return;

    EM_ASM({
        BoE.Canvas.resetClip($0);
    }, canvasId);
}

bool CanvasRenderTexture::loadFromFile(const std::string& filename) {
    // First, load the image as a texture
    int tempImageId = EM_ASM_INT({
        return BoE.Canvas.loadImageFromPath(UTF8ToString($0));
    }, filename.c_str());

    if (tempImageId < 0) {
        printf("Failed to load image for RenderTexture: %s\n", filename.c_str());
        return false;
    }

    // Get image dimensions
    unsigned int imgWidth = EM_ASM_INT({
        var size = BoE.Canvas.getImageSize($0);
        return size ? size.width : 0;
    }, tempImageId);

    unsigned int imgHeight = EM_ASM_INT({
        var size = BoE.Canvas.getImageSize($0);
        return size ? size.height : 0;
    }, tempImageId);

    // Create canvas with those dimensions
    if (!create(imgWidth, imgHeight)) {
        return false;
    }

    // Draw the image to the canvas
    EM_ASM({
        // Wait a bit for image to load, then draw
        var imgId = $0;
        var canvasId = $1;

        setTimeout(function() {
            var imgObj = BoE.Canvas.images[imgId];
            if (imgObj && imgObj.loaded) {
                BoE.Canvas.drawImage(canvasId, imgId, 0, 0);
            }
        }, 100);
    }, tempImageId, canvasId);

    return true;
}

bool CanvasRenderTexture::create(unsigned int w, unsigned int h) {
    width = w;
    height = h;

    canvasId = EM_ASM_INT({
        return BoE.Canvas.createOffscreenCanvas($0, $1);
    }, w, h);

    return canvasId >= 0;
}

// ========== CanvasRenderWindow Implementation (STUB) ==========

CanvasRenderWindow::CanvasRenderWindow(unsigned int w, unsigned int h, const std::string& t)
    : width(w), height(h), title(t), open(true) {
    // Initialize the main canvas
    bool success = EM_ASM_INT({
        if (!BoE.Canvas.mainCanvas) {
            return BoE.Canvas.init() ? 1 : 0;
        }
        // Set canvas dimensions
        BoE.Canvas.mainCanvas.width = $0;
        BoE.Canvas.mainCanvas.height = $1;
        document.title = UTF8ToString($2);
        return 1;
    }, w, h, t.c_str());

    if (!success) {
        printf("Warning: Failed to initialize main canvas\n");
    }
}

CanvasRenderWindow::~CanvasRenderWindow() {
    // Main canvas cleanup not needed
}

void CanvasRenderWindow::clear(const Color& color) {
    EM_ASM({
        BoE.Canvas.clear(-1, $0, $1, $2, $3);
    }, color.r, color.g, color.b, color.a);
}

void CanvasRenderWindow::display() {
    // For Canvas 2D, display() is a no-op
    // Drawing is immediate
}

unsigned int CanvasRenderWindow::getWidth() const {
    return width;
}

unsigned int CanvasRenderWindow::getHeight() const {
    return height;
}

void CanvasRenderWindow::setView(rectangle rect) {
    int w = rect.width();
    int h = rect.height();

    EM_ASM({
        BoE.Canvas.save(-1);
        BoE.Canvas.setClipRect(-1, $0, $1, $2, $3);
    }, rect.left, rect.top, w, h);
}

void CanvasRenderWindow::resetView() {
    EM_ASM({
        BoE.Canvas.restore(-1);
    });
}

void CanvasRenderWindow::setClipRect(rectangle rect) {
    int w = rect.width();
    int h = rect.height();

    EM_ASM({
        BoE.Canvas.setClipRect(-1, $0, $1, $2, $3);
    }, rect.left, rect.top, w, h);
}

void CanvasRenderWindow::resetClipRect() {
    EM_ASM({
        BoE.Canvas.resetClip(-1);
    });
}

bool CanvasRenderWindow::isOpen() const {
    return open;
}

void CanvasRenderWindow::close() {
    open = false;
}

// ========== CanvasTexture Implementation (STUB) ==========

CanvasTexture::CanvasTexture()
    : width(0), height(0), imageId(-1) {
}

CanvasTexture::~CanvasTexture() {
    // Image cleanup handled by JavaScript garbage collection
    // Could explicitly delete if needed:
    // if (imageId >= 0) {
    //     EM_ASM({ delete BoE.Canvas.images[$0]; }, imageId);
    // }
}

bool CanvasTexture::loadFromFile(const std::string& filename) {
    // Load image from Emscripten VFS
    imageId = EM_ASM_INT({
        return BoE.Canvas.loadImageFromPath(UTF8ToString($0));
    }, filename.c_str());

    if (imageId < 0) {
        printf("Failed to load image: %s\n", filename.c_str());
        return false;
    }

    // Get image dimensions (may not be available immediately if still loading)
    // Wait a moment for the image to load synchronously
    EM_ASM({
        var imgObj = BoE.Canvas.images[$0];
        if (imgObj && imgObj.element) {
            // Image is loading, we can get dimensions when loaded
            // For now, store what we have
        }
    }, imageId);

    // Try to get dimensions
    width = EM_ASM_INT({
        var size = BoE.Canvas.getImageSize($0);
        return size ? size.width : 0;
    }, imageId);

    height = EM_ASM_INT({
        var size = BoE.Canvas.getImageSize($0);
        return size ? size.height : 0;
    }, imageId);

    printf("Loaded texture %s: ID=%d, %dx%d\n", filename.c_str(), imageId, width, height);
    return true;
}

bool CanvasTexture::create(unsigned int w, unsigned int h) {
    width = w;
    height = h;

    // Create blank image (using canvas)
    imageId = EM_ASM_INT({
        return BoE.Canvas.createImage($0, $1);
    }, w, h);

    return imageId >= 0;
}

unsigned int CanvasTexture::getWidth() const {
    // If dimensions not yet known, try to get them
    if (width == 0 && imageId >= 0) {
        return EM_ASM_INT({
            var size = BoE.Canvas.getImageSize($0);
            return size ? size.width : 0;
        }, imageId);
    }
    return width;
}

unsigned int CanvasTexture::getHeight() const {
    // If dimensions not yet known, try to get them
    if (height == 0 && imageId >= 0) {
        return EM_ASM_INT({
            var size = BoE.Canvas.getImageSize($0);
            return size ? size.height : 0;
        }, imageId);
    }
    return height;
}
