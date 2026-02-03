/*
 * input_handler_sfml.hpp
 * BoE - SFML Implementation of Input Handler
 *
 * Desktop (SFML) implementation of the input abstraction layer
 */

#ifndef BOE_INPUT_HANDLER_SFML_HPP
#define BOE_INPUT_HANDLER_SFML_HPP

#include "input_handler.hpp"
#include <SFML/Window.hpp>

class IRenderWindow;

/**
 * SFML implementation of IInputHandler
 * Wraps sf::Window event handling
 */
class SfmlInputHandler : public IInputHandler {
private:
    sf::Window* window;

public:
    explicit SfmlInputHandler(IRenderWindow* renderWindow);
    virtual ~SfmlInputHandler() = default;

    // IInputHandler interface
    bool pollEvent(InputEvent& event) override;
    bool isKeyPressed(KeyCode key) const override;
    bool isMouseButtonPressed(MouseButton button) const override;
    location getMousePosition() const override;

private:
    // Helper conversion functions
    static KeyCode fromSfmlKey(sf::Keyboard::Key key);
    static sf::Keyboard::Key toSfmlKey(KeyCode key);
    static MouseButton fromSfmlButton(sf::Mouse::Button button);
    static sf::Mouse::Button toSfmlButton(MouseButton button);
};

#endif // BOE_INPUT_HANDLER_SFML_HPP
