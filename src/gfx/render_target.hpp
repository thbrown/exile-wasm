/*
 * render_target.hpp
 * BoE - Platform Abstraction Layer
 *
 * Interface for platform-independent rendering
 * Abstracts SFML (desktop) and Canvas 2D (web)
 */

#ifndef BOE_RENDER_TARGET_HPP
#define BOE_RENDER_TARGET_HPP

#include <memory>
#include <string>

// Forward declarations
struct rectangle;
namespace sf {
    class RenderTarget;
    class RenderTexture;
    class RenderWindow;
    class Color;
    class BlendMode;
}

class ITexture;
class rectangle;

/**
 * Platform-independent color representation
 */
struct Color {
    unsigned char r, g, b, a;

    Color() : r(0), g(0), b(0), a(255) {}
    Color(unsigned char r, unsigned char g, unsigned char b, unsigned char a = 255)
        : r(r), g(g), b(b), a(a) {}

    // Conversion to/from SFML Color (for desktop builds)
#ifdef USE_SFML
    Color(const sf::Color& sfColor);
    sf::Color toSfColor() const;
#endif
};

/**
 * Platform-independent blend mode
 */
enum class BlendMode {
    None,       // No blending (replace)
    Alpha,      // Standard alpha blending
    Add,        // Additive blending
    Multiply    // Multiplicative blending
};

/**
 * Platform-independent render target interface
 * This is the base interface for all rendering surfaces (windows, textures, canvas)
 */
class IRenderTarget {
public:
    virtual ~IRenderTarget() = default;

    // Core rendering operations
    virtual void clear(const Color& color = Color(0, 0, 0, 255)) = 0;
    virtual void display() = 0;

    // Dimensions
    virtual unsigned int getWidth() const = 0;
    virtual unsigned int getHeight() const = 0;

    // View/viewport management
    virtual void setView(rectangle rect) = 0;
    virtual void resetView() = 0;

    // Clipping
    virtual void setClipRect(rectangle rect) = 0;
    virtual void resetClipRect() = 0;

    // Platform-specific accessor (for gradual migration)
    // Returns SFML RenderTarget on desktop, nullptr on web
    virtual sf::RenderTarget* getSfmlTarget() { return nullptr; }
    virtual const sf::RenderTarget* getSfmlTarget() const { return nullptr; }
};

/**
 * Texture interface
 * Represents an image/texture that can be drawn to a render target
 */
class ITexture {
public:
    virtual ~ITexture() = default;

    // Loading
    virtual bool loadFromFile(const std::string& filename) = 0;
    virtual bool create(unsigned int width, unsigned int height) = 0;

    // Dimensions
    virtual unsigned int getWidth() const = 0;
    virtual unsigned int getHeight() const = 0;

    // Platform-specific accessors
#ifdef USE_SFML
    virtual const sf::Texture* getSfmlTexture() const { return nullptr; }
#endif

    // Canvas-specific accessor (returns -1 for non-canvas textures)
    virtual int getImageId() const { return -1; }
};

/**
 * Render texture interface
 * A texture that can also be used as a render target (for off-screen rendering)
 */
class IRenderTexture : public IRenderTarget, public ITexture {
public:
    virtual ~IRenderTexture() = default;

    // Both RenderTarget and Texture interfaces are inherited

    // Platform-specific accessor (for gradual migration)
    virtual sf::RenderTexture* getSfmlRenderTexture() { return nullptr; }
    virtual const sf::RenderTexture* getSfmlRenderTexture() const { return nullptr; }
};

/**
 * Render window interface
 * The main window for rendering the game
 */
class IRenderWindow : public IRenderTarget {
public:
    virtual ~IRenderWindow() = default;

    // Window management
    virtual bool isOpen() const = 0;
    virtual void close() = 0;

    // Event handling (handled separately by input abstraction)

    // Platform-specific accessor (for gradual migration)
    virtual sf::RenderWindow* getSfmlWindow() { return nullptr; }
    virtual const sf::RenderWindow* getSfmlWindow() const { return nullptr; }
};

// Factory functions for creating platform-specific implementations
std::unique_ptr<IRenderWindow> createRenderWindow(unsigned int width, unsigned int height, const std::string& title);
std::unique_ptr<IRenderTexture> createRenderTexture(unsigned int width, unsigned int height);

#endif // BOE_RENDER_TARGET_HPP
