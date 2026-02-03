/*
 * render_target_sfml.cpp
 * BoE - SFML Implementation of Render Target
 */

#include "render_target_sfml.hpp"
#include "render_target.hpp"
#include <SFML/Graphics.hpp>

// ========== Color Conversions ==========

#ifdef USE_SFML
Color::Color(const sf::Color& sfColor)
    : r(sfColor.r), g(sfColor.g), b(sfColor.b), a(sfColor.a) {}

sf::Color Color::toSfColor() const {
    return sf::Color(r, g, b, a);
}
#endif

// ========== SfmlRenderTexture Implementation ==========

SfmlRenderTexture::SfmlRenderTexture()
    : renderTexture(std::make_unique<sf::RenderTexture>()) {
}

SfmlRenderTexture::SfmlRenderTexture(unsigned int width, unsigned int height)
    : renderTexture(std::make_unique<sf::RenderTexture>()) {
    create(width, height);
}

void SfmlRenderTexture::clear(const Color& color) {
    renderTexture->clear(color.toSfColor());
}

void SfmlRenderTexture::display() {
    renderTexture->display();
}

unsigned int SfmlRenderTexture::getWidth() const {
    return renderTexture->getSize().x;
}

unsigned int SfmlRenderTexture::getHeight() const {
    return renderTexture->getSize().y;
}

void SfmlRenderTexture::setView(rectangle rect) {
    sf::View view(sf::FloatRect(rect.left, rect.top, rect.width(), rect.height()));
    renderTexture->setView(view);
}

void SfmlRenderTexture::resetView() {
    renderTexture->setView(renderTexture->getDefaultView());
}

void SfmlRenderTexture::setClipRect(rectangle rect) {
    // SFML uses glScissor for clipping
    sf::View view = renderTexture->getView();
    view.setViewport(sf::FloatRect(
        static_cast<float>(rect.left) / getWidth(),
        static_cast<float>(rect.top) / getHeight(),
        static_cast<float>(rect.width()) / getWidth(),
        static_cast<float>(rect.height()) / getHeight()
    ));
    renderTexture->setView(view);
}

void SfmlRenderTexture::resetClipRect() {
    resetView();
}

bool SfmlRenderTexture::loadFromFile(const std::string& filename) {
    // RenderTextures don't load from files directly
    // Create a regular texture, load it, then copy to render texture
    sf::Texture temp;
    if (!temp.loadFromFile(filename)) {
        return false;
    }

    if (!create(temp.getSize().x, temp.getSize().y)) {
        return false;
    }

    sf::Sprite sprite(temp);
    renderTexture->draw(sprite);
    renderTexture->display();
    return true;
}

bool SfmlRenderTexture::create(unsigned int width, unsigned int height) {
    return renderTexture->create(width, height);
}

sf::RenderTarget* SfmlRenderTexture::getSfmlTarget() {
    return renderTexture.get();
}

const sf::RenderTarget* SfmlRenderTexture::getSfmlTarget() const {
    return renderTexture.get();
}

const sf::Texture* SfmlRenderTexture::getSfmlTexture() const {
    return &renderTexture->getTexture();
}

sf::RenderTexture* SfmlRenderTexture::getSfmlRenderTexture() {
    return renderTexture.get();
}

const sf::RenderTexture* SfmlRenderTexture::getSfmlRenderTexture() const {
    return renderTexture.get();
}

// ========== SfmlRenderWindow Implementation ==========

SfmlRenderWindow::SfmlRenderWindow(unsigned int width, unsigned int height, const std::string& title)
    : window(std::make_unique<sf::RenderWindow>(sf::VideoMode(width, height), title)) {
}

void SfmlRenderWindow::clear(const Color& color) {
    window->clear(color.toSfColor());
}

void SfmlRenderWindow::display() {
    window->display();
}

unsigned int SfmlRenderWindow::getWidth() const {
    return window->getSize().x;
}

unsigned int SfmlRenderWindow::getHeight() const {
    return window->getSize().y;
}

void SfmlRenderWindow::setView(rectangle rect) {
    sf::View view(sf::FloatRect(rect.left, rect.top, rect.width(), rect.height()));
    window->setView(view);
}

void SfmlRenderWindow::resetView() {
    window->setView(window->getDefaultView());
}

void SfmlRenderWindow::setClipRect(rectangle rect) {
    sf::View view = window->getView();
    view.setViewport(sf::FloatRect(
        static_cast<float>(rect.left) / getWidth(),
        static_cast<float>(rect.top) / getHeight(),
        static_cast<float>(rect.width()) / getWidth(),
        static_cast<float>(rect.height()) / getHeight()
    ));
    window->setView(view);
}

void SfmlRenderWindow::resetClipRect() {
    resetView();
}

bool SfmlRenderWindow::isOpen() const {
    return window->isOpen();
}

void SfmlRenderWindow::close() {
    window->close();
}

sf::RenderTarget* SfmlRenderWindow::getSfmlTarget() {
    return window.get();
}

const sf::RenderTarget* SfmlRenderWindow::getSfmlTarget() const {
    return window.get();
}

sf::RenderWindow* SfmlRenderWindow::getSfmlWindow() {
    return window.get();
}

const sf::RenderWindow* SfmlRenderWindow::getSfmlWindow() const {
    return window.get();
}

// ========== SfmlTexture Implementation ==========

SfmlTexture::SfmlTexture()
    : texture(std::make_unique<sf::Texture>()) {
}

bool SfmlTexture::loadFromFile(const std::string& filename) {
    return texture->loadFromFile(filename);
}

bool SfmlTexture::create(unsigned int width, unsigned int height) {
    return texture->create(width, height);
}

unsigned int SfmlTexture::getWidth() const {
    return texture->getSize().x;
}

unsigned int SfmlTexture::getHeight() const {
    return texture->getSize().y;
}

const sf::Texture* SfmlTexture::getSfmlTexture() const {
    return texture.get();
}

// ========== Factory Functions ==========

std::unique_ptr<IRenderWindow> createRenderWindow(unsigned int width, unsigned int height, const std::string& title) {
#ifdef USE_SFML
    return std::make_unique<SfmlRenderWindow>(width, height, title);
#else
    // Web implementation will be created in Phase 3
    return nullptr;
#endif
}

std::unique_ptr<IRenderTexture> createRenderTexture(unsigned int width, unsigned int height) {
#ifdef USE_SFML
    return std::make_unique<SfmlRenderTexture>(width, height);
#else
    // Web implementation will be created in Phase 3
    return nullptr;
#endif
}
