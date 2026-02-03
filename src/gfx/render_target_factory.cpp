/*
 * render_target_factory.cpp
 * BoE - Factory functions for creating platform-specific render targets
 */

#include "render_target.hpp"

#ifdef USE_SFML
    #include "render_target_sfml.hpp"
#else
    #include "render_target_canvas.hpp"
#endif

std::unique_ptr<IRenderWindow> createRenderWindow(unsigned int width, unsigned int height, const std::string& title) {
#ifdef USE_SFML
    return std::make_unique<SfmlRenderWindow>(width, height, title);
#else
    return std::make_unique<CanvasRenderWindow>(width, height, title);
#endif
}

std::unique_ptr<IRenderTexture> createRenderTexture(unsigned int width, unsigned int height) {
#ifdef USE_SFML
    return std::make_unique<SfmlRenderTexture>(width, height);
#else
    return std::make_unique<CanvasRenderTexture>(width, height);
#endif
}

std::unique_ptr<ITexture> createTexture() {
#ifdef USE_SFML
    return std::make_unique<SfmlTexture>();
#else
    return std::make_unique<CanvasTexture>();
#endif
}
