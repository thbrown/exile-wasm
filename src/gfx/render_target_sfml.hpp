/*
 * render_target_sfml.hpp
 * BoE - SFML Implementation of Render Target
 *
 * Desktop (SFML) implementation of the rendering abstraction layer
 */

#ifndef BOE_RENDER_TARGET_SFML_HPP
#define BOE_RENDER_TARGET_SFML_HPP

#include "render_target.hpp"
#include <SFML/Graphics.hpp>
#include <memory>

/**
 * SFML implementation of IRenderTexture
 * Wraps sf::RenderTexture for compatibility with abstraction layer
 */
class SfmlRenderTexture : public IRenderTexture {
private:
    std::unique_ptr<sf::RenderTexture> renderTexture;

public:
    SfmlRenderTexture();
    SfmlRenderTexture(unsigned int width, unsigned int height);
    virtual ~SfmlRenderTexture() = default;

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

    // Platform-specific accessors
    sf::RenderTarget* getSfmlTarget() override;
    const sf::RenderTarget* getSfmlTarget() const override;
    const sf::Texture* getSfmlTexture() const override;
    sf::RenderTexture* getSfmlRenderTexture() override;
    const sf::RenderTexture* getSfmlRenderTexture() const override;

    // Direct access to underlying SFML object (for migration)
    sf::RenderTexture& get() { return *renderTexture; }
    const sf::RenderTexture& get() const { return *renderTexture; }
};

/**
 * SFML implementation of IRenderWindow
 * Wraps sf::RenderWindow for compatibility with abstraction layer
 */
class SfmlRenderWindow : public IRenderWindow {
private:
    std::unique_ptr<sf::RenderWindow> window;

public:
    SfmlRenderWindow(unsigned int width, unsigned int height, const std::string& title);
    virtual ~SfmlRenderWindow() = default;

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

    // Platform-specific accessors
    sf::RenderTarget* getSfmlTarget() override;
    const sf::RenderTarget* getSfmlTarget() const override;
    sf::RenderWindow* getSfmlWindow() override;
    const sf::RenderWindow* getSfmlWindow() const override;

    // Direct access to underlying SFML object (for migration)
    sf::RenderWindow& get() { return *window; }
    const sf::RenderWindow& get() const { return *window; }
};

/**
 * SFML implementation of ITexture
 * Wraps sf::Texture for compatibility with abstraction layer
 */
class SfmlTexture : public ITexture {
private:
    std::unique_ptr<sf::Texture> texture;

public:
    SfmlTexture();
    virtual ~SfmlTexture() = default;

    // ITexture interface
    bool loadFromFile(const std::string& filename) override;
    bool create(unsigned int width, unsigned int height) override;
    unsigned int getWidth() const override;
    unsigned int getHeight() const override;

    // Platform-specific accessor
    const sf::Texture* getSfmlTexture() const override;

    // Direct access to underlying SFML object (for migration)
    sf::Texture& get() { return *texture; }
    const sf::Texture& get() const { return *texture; }
};

#endif // BOE_RENDER_TARGET_SFML_HPP
