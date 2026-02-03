/*
 * render_target_canvas.hpp
 * BoE - Canvas 2D Implementation of Render Target
 *
 * Web (Canvas 2D) implementation of the rendering abstraction layer
 * This is a stub for Phase 2, will be implemented in Phase 3
 */

#ifndef BOE_RENDER_TARGET_CANVAS_HPP
#define BOE_RENDER_TARGET_CANVAS_HPP

#include "render_target.hpp"
#include <string>

/**
 * Canvas 2D implementation of IRenderTexture
 * Uses off-screen HTML5 Canvas for rendering
 */
class CanvasRenderTexture : public IRenderTexture {
private:
    unsigned int width;
    unsigned int height;
    int canvasId; // ID of the canvas element in JavaScript

public:
    CanvasRenderTexture();
    CanvasRenderTexture(unsigned int width, unsigned int height);
    virtual ~CanvasRenderTexture();

    // IRenderTarget interface
    void clear(const Color& color = Color(0, 0, 0, 255)) override;
    void display() override;
    unsigned int getWidth() const override;
    unsigned int getHeight() const override;
    void setView(rectangle rect) override;
    void resetView() override;
    void setClipRect(rectangle rect) override;
    void resetClipRect() override;

    // ITexture interface
    bool loadFromFile(const std::string& filename) override;
    bool create(unsigned int width, unsigned int height) override;

    // Canvas-specific accessor (for drawing operations)
    int getCanvasId() const { return canvasId; }
};

/**
 * Canvas 2D implementation of IRenderWindow
 * Uses the main HTML5 Canvas element
 */
class CanvasRenderWindow : public IRenderWindow {
private:
    unsigned int width;
    unsigned int height;
    std::string title;
    bool open;

public:
    CanvasRenderWindow(unsigned int width, unsigned int height, const std::string& title);
    virtual ~CanvasRenderWindow();

    // IRenderTarget interface
    void clear(const Color& color = Color(0, 0, 0, 255)) override;
    void display() override;
    unsigned int getWidth() const override;
    unsigned int getHeight() const override;
    void setView(rectangle rect) override;
    void resetView() override;
    void setClipRect(rectangle rect) override;
    void resetClipRect() override;

    // IRenderWindow interface
    bool isOpen() const override;
    void close() override;
};

/**
 * Canvas 2D implementation of ITexture
 * Wraps JavaScript Image objects
 */
class CanvasTexture : public ITexture {
private:
    mutable unsigned int width;  // mutable because getWidth() may update it
    mutable unsigned int height; // mutable because getHeight() may update it
    int imageId; // ID of the Image object in JavaScript

public:
    CanvasTexture();
    virtual ~CanvasTexture();

    // ITexture interface
    bool loadFromFile(const std::string& filename) override;
    bool create(unsigned int width, unsigned int height) override;
    unsigned int getWidth() const override;
    unsigned int getHeight() const override;

    // Canvas-specific accessor
    int getImageId() const override { return imageId; }
};

#endif // BOE_RENDER_TARGET_CANVAS_HPP
