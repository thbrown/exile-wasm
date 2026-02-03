/*
 * render_image_canvas.cpp
 * BoE - Canvas 2D Image Rendering Functions
 */

#include "render_image_canvas.hpp"
#include "render_target_canvas.hpp"
#include <emscripten.h>

// Rectangle type (simplified for web builds)
#ifndef USE_SFML
struct rectangle {
    int left, top, right, bottom;
    rectangle() : left(0), top(0), right(0), bottom(0) {}
    rectangle(int l, int t, int r, int b) : left(l), top(t), right(r), bottom(b) {}
    int width() const { return right - left; }
    int height() const { return bottom - top; }
};
#endif

/**
 * Get canvas ID from render target
 */
static int getCanvasId(IRenderTarget& target) {
    // Try to cast to CanvasRenderWindow
    auto* window = dynamic_cast<CanvasRenderWindow*>(&target);
    if (window) {
        return -1; // Main canvas
    }

    // Try to cast to CanvasRenderTexture
    auto* texture = dynamic_cast<CanvasRenderTexture*>(&target);
    if (texture) {
        return texture->getCanvasId();
    }

    return -1; // Default to main canvas
}

/**
 * Get image ID from texture
 */
static int getImageId(const ITexture& texture) {
    auto* canvasTexture = dynamic_cast<const CanvasTexture*>(&texture);
    if (canvasTexture) {
        return canvasTexture->getImageId();
    }
    return -1;
}

void drawTexture(const ITexture& texture, IRenderTarget& target, int x, int y) {
    int imageId = getImageId(texture);
    int canvasId = getCanvasId(target);

    if (imageId < 0) {
        return;
    }

    EM_ASM({
        BoE.Canvas.drawImage($0, $1, $2, $3);
    }, canvasId, imageId, x, y);
}

void drawTexture(const ITexture& texture, IRenderTarget& target, rectangle destRect) {
    int imageId = getImageId(texture);
    int canvasId = getCanvasId(target);

    if (imageId < 0) {
        return;
    }

    int w = destRect.width();
    int h = destRect.height();

    EM_ASM({
        BoE.Canvas.drawImage($0, $1, $2, $3, $4, $5);
    }, canvasId, imageId, destRect.left, destRect.top, w, h);
}

void drawTextureRegion(const ITexture& texture, rectangle sourceRect,
                       IRenderTarget& target, int x, int y) {
    int imageId = getImageId(texture);
    int canvasId = getCanvasId(target);

    if (imageId < 0) {
        return;
    }

    int sw = sourceRect.width();
    int sh = sourceRect.height();

    EM_ASM({
        // Draw with source rect, dest at x,y with same size
        BoE.Canvas.drawImage($0, $1, $2, $3, $4, $5, $6, $7, $8, $9);
    }, canvasId, imageId,
       sourceRect.left, sourceRect.top, sw, sh,
       x, y, sw, sh);
}

void drawTextureRegion(const ITexture& texture, rectangle sourceRect,
                       IRenderTarget& target, rectangle destRect) {
    int imageId = getImageId(texture);
    int canvasId = getCanvasId(target);

    if (imageId < 0) {
        return;
    }

    int sw = sourceRect.width();
    int sh = sourceRect.height();
    int dw = destRect.width();
    int dh = destRect.height();

    EM_ASM({
        BoE.Canvas.drawImage($0, $1, $2, $3, $4, $5, $6, $7, $8, $9);
    }, canvasId, imageId,
       sourceRect.left, sourceRect.top, sw, sh,
       destRect.left, destRect.top, dw, dh);
}

void drawTextureRegion(const ITexture& texture, rectangle sourceRect,
                       IRenderTarget& target, rectangle destRect,
                       const Color& tint) {
    int imageId = getImageId(texture);
    int canvasId = getCanvasId(target);

    if (imageId < 0) {
        return;
    }

    int sw = sourceRect.width();
    int sh = sourceRect.height();
    int dw = destRect.width();
    int dh = destRect.height();

    // If tint is white (255,255,255), draw normally
    if (tint.r == 255 && tint.g == 255 && tint.b == 255) {
        EM_ASM({
            BoE.Canvas.drawImage($0, $1, $2, $3, $4, $5, $6, $7, $8, $9);
        }, canvasId, imageId,
           sourceRect.left, sourceRect.top, sw, sh,
           destRect.left, destRect.top, dw, dh);
    } else {
        // Apply color tint using globalAlpha and composite operations
        // This is a simplified tint - full tinting would require canvas manipulation
        EM_ASM({
            var ctx = BoE.Canvas.getContext($0);
            if (!ctx) return;

            ctx.save();

            // Draw the image normally first
            BoE.Canvas.drawImage($0, $1, $2, $3, $4, $5, $6, $7, $8, $9);

            // Apply tint using composite operations
            // This is a simplified approach - full color tinting is complex
            ctx.globalAlpha = $10 / 255.0;
            ctx.fillStyle = 'rgb(' + $11 + ',' + $12 + ',' + $13 + ')';
            ctx.globalCompositeOperation = 'multiply';
            ctx.fillRect($6, $7, $8, $9);

            ctx.restore();
        }, canvasId, imageId,
           sourceRect.left, sourceRect.top, sw, sh,
           destRect.left, destRect.top, dw, dh,
           tint.a, tint.r, tint.g, tint.b);
    }
}
