/*
 * render_image_canvas.hpp
 * BoE - Canvas 2D Image Rendering Functions
 *
 * High-level image drawing functions for Canvas 2D backend
 */

#ifndef BOE_RENDER_IMAGE_CANVAS_HPP
#define BOE_RENDER_IMAGE_CANVAS_HPP

#include "render_target.hpp"
#include <string>

// Forward declaration
struct rectangle;

/**
 * Draw a texture to a render target
 * @param texture - Source texture
 * @param target - Destination render target
 * @param x - X position
 * @param y - Y position
 */
void drawTexture(const ITexture& texture, IRenderTarget& target, int x, int y);

/**
 * Draw a texture with scaling
 * @param texture - Source texture
 * @param target - Destination render target
 * @param destRect - Destination rectangle
 */
void drawTexture(const ITexture& texture, IRenderTarget& target, rectangle destRect);

/**
 * Draw part of a texture (sprite from sprite sheet)
 * @param texture - Source texture
 * @param sourceRect - Source rectangle in texture
 * @param target - Destination render target
 * @param x - X position
 * @param y - Y position
 */
void drawTextureRegion(const ITexture& texture, rectangle sourceRect,
                       IRenderTarget& target, int x, int y);

/**
 * Draw part of a texture with scaling
 * @param texture - Source texture
 * @param sourceRect - Source rectangle in texture
 * @param target - Destination render target
 * @param destRect - Destination rectangle
 */
void drawTextureRegion(const ITexture& texture, rectangle sourceRect,
                       IRenderTarget& target, rectangle destRect);

/**
 * Draw part of a texture with color tint
 * @param texture - Source texture
 * @param sourceRect - Source rectangle in texture
 * @param target - Destination render target
 * @param destRect - Destination rectangle
 * @param tint - Color tint (Color::WHITE for no tint)
 */
void drawTextureRegion(const ITexture& texture, rectangle sourceRect,
                       IRenderTarget& target, rectangle destRect,
                       const Color& tint);

#endif // BOE_RENDER_IMAGE_CANVAS_HPP
