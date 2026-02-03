/*
 * input_handler_sfml.cpp
 * BoE - SFML Implementation of Input Handler
 */

#include "input_handler_sfml.hpp"
#include "gfx/render_target.hpp"
#include "gfx/render_target_sfml.hpp"
#include <SFML/Window.hpp>

// ========== InputEvent SFML Conversion ==========

#ifdef USE_SFML
InputEvent InputEvent::fromSfmlEvent(const sf::Event& sfEvent) {
    InputEvent event;

    switch (sfEvent.type) {
        case sf::Event::Closed:
            event.type = EventType::Closed;
            break;

        case sf::Event::KeyPressed:
            event.type = EventType::KeyPressed;
            event.key.code = SfmlInputHandler::fromSfmlKey(sfEvent.key.code);
            event.key.alt = sfEvent.key.alt;
            event.key.control = sfEvent.key.control;
            event.key.shift = sfEvent.key.shift;
            event.key.system = sfEvent.key.system;
            break;

        case sf::Event::KeyReleased:
            event.type = EventType::KeyReleased;
            event.key.code = SfmlInputHandler::fromSfmlKey(sfEvent.key.code);
            event.key.alt = sfEvent.key.alt;
            event.key.control = sfEvent.key.control;
            event.key.shift = sfEvent.key.shift;
            event.key.system = sfEvent.key.system;
            break;

        case sf::Event::MouseButtonPressed:
            event.type = EventType::MouseButtonPressed;
            event.mouseButton.button = SfmlInputHandler::fromSfmlButton(sfEvent.mouseButton.button);
            event.mouseButton.x = sfEvent.mouseButton.x;
            event.mouseButton.y = sfEvent.mouseButton.y;
            break;

        case sf::Event::MouseButtonReleased:
            event.type = EventType::MouseButtonReleased;
            event.mouseButton.button = SfmlInputHandler::fromSfmlButton(sfEvent.mouseButton.button);
            event.mouseButton.x = sfEvent.mouseButton.x;
            event.mouseButton.y = sfEvent.mouseButton.y;
            break;

        case sf::Event::MouseMoved:
            event.type = EventType::MouseMoved;
            event.mouseMove.x = sfEvent.mouseMove.x;
            event.mouseMove.y = sfEvent.mouseMove.y;
            break;

        case sf::Event::MouseWheelScrolled:
            event.type = EventType::MouseWheelScrolled;
            event.mouseWheel.delta = sfEvent.mouseWheelScroll.delta;
            event.mouseWheel.x = sfEvent.mouseWheelScroll.x;
            event.mouseWheel.y = sfEvent.mouseWheelScroll.y;
            break;

        case sf::Event::Resized:
            event.type = EventType::Resized;
            event.size.width = sfEvent.size.width;
            event.size.height = sfEvent.size.height;
            break;

        case sf::Event::LostFocus:
            event.type = EventType::LostFocus;
            break;

        case sf::Event::GainedFocus:
            event.type = EventType::GainedFocus;
            break;

        default:
            event.type = EventType::None;
            break;
    }

    return event;
}
#endif

// ========== SfmlInputHandler Implementation ==========

SfmlInputHandler::SfmlInputHandler(IRenderWindow* renderWindow)
    : window(nullptr) {
    if (renderWindow) {
        window = renderWindow->getSfmlWindow();
    }
}

bool SfmlInputHandler::pollEvent(InputEvent& event) {
    if (!window) {
        return false;
    }

    sf::Event sfEvent;
    if (window->pollEvent(sfEvent)) {
        event = InputEvent::fromSfmlEvent(sfEvent);
        return true;
    }

    return false;
}

bool SfmlInputHandler::isKeyPressed(KeyCode key) const {
    return sf::Keyboard::isKeyPressed(toSfmlKey(key));
}

bool SfmlInputHandler::isMouseButtonPressed(MouseButton button) const {
    return sf::Mouse::isButtonPressed(toSfmlButton(button));
}

location SfmlInputHandler::getMousePosition() const {
    if (!window) {
        return location(0, 0);
    }

    sf::Vector2i pos = sf::Mouse::getPosition(*window);
    return location(pos.x, pos.y);
}

// ========== Key Mapping ==========

KeyCode SfmlInputHandler::fromSfmlKey(sf::Keyboard::Key key) {
    switch (key) {
        case sf::Keyboard::A: return KeyCode::A;
        case sf::Keyboard::B: return KeyCode::B;
        case sf::Keyboard::C: return KeyCode::C;
        case sf::Keyboard::D: return KeyCode::D;
        case sf::Keyboard::E: return KeyCode::E;
        case sf::Keyboard::F: return KeyCode::F;
        case sf::Keyboard::G: return KeyCode::G;
        case sf::Keyboard::H: return KeyCode::H;
        case sf::Keyboard::I: return KeyCode::I;
        case sf::Keyboard::J: return KeyCode::J;
        case sf::Keyboard::K: return KeyCode::K;
        case sf::Keyboard::L: return KeyCode::L;
        case sf::Keyboard::M: return KeyCode::M;
        case sf::Keyboard::N: return KeyCode::N;
        case sf::Keyboard::O: return KeyCode::O;
        case sf::Keyboard::P: return KeyCode::P;
        case sf::Keyboard::Q: return KeyCode::Q;
        case sf::Keyboard::R: return KeyCode::R;
        case sf::Keyboard::S: return KeyCode::S;
        case sf::Keyboard::T: return KeyCode::T;
        case sf::Keyboard::U: return KeyCode::U;
        case sf::Keyboard::V: return KeyCode::V;
        case sf::Keyboard::W: return KeyCode::W;
        case sf::Keyboard::X: return KeyCode::X;
        case sf::Keyboard::Y: return KeyCode::Y;
        case sf::Keyboard::Z: return KeyCode::Z;

        case sf::Keyboard::Num0: return KeyCode::Num0;
        case sf::Keyboard::Num1: return KeyCode::Num1;
        case sf::Keyboard::Num2: return KeyCode::Num2;
        case sf::Keyboard::Num3: return KeyCode::Num3;
        case sf::Keyboard::Num4: return KeyCode::Num4;
        case sf::Keyboard::Num5: return KeyCode::Num5;
        case sf::Keyboard::Num6: return KeyCode::Num6;
        case sf::Keyboard::Num7: return KeyCode::Num7;
        case sf::Keyboard::Num8: return KeyCode::Num8;
        case sf::Keyboard::Num9: return KeyCode::Num9;

        case sf::Keyboard::Left: return KeyCode::Left;
        case sf::Keyboard::Right: return KeyCode::Right;
        case sf::Keyboard::Up: return KeyCode::Up;
        case sf::Keyboard::Down: return KeyCode::Down;

        case sf::Keyboard::F1: return KeyCode::F1;
        case sf::Keyboard::F2: return KeyCode::F2;
        case sf::Keyboard::F3: return KeyCode::F3;
        case sf::Keyboard::F4: return KeyCode::F4;
        case sf::Keyboard::F5: return KeyCode::F5;
        case sf::Keyboard::F6: return KeyCode::F6;
        case sf::Keyboard::F7: return KeyCode::F7;
        case sf::Keyboard::F8: return KeyCode::F8;
        case sf::Keyboard::F9: return KeyCode::F9;
        case sf::Keyboard::F10: return KeyCode::F10;
        case sf::Keyboard::F11: return KeyCode::F11;
        case sf::Keyboard::F12: return KeyCode::F12;

        case sf::Keyboard::LShift: return KeyCode::LShift;
        case sf::Keyboard::RShift: return KeyCode::RShift;
        case sf::Keyboard::LControl: return KeyCode::LControl;
        case sf::Keyboard::RControl: return KeyCode::RControl;
        case sf::Keyboard::LAlt: return KeyCode::LAlt;
        case sf::Keyboard::RAlt: return KeyCode::RAlt;

        case sf::Keyboard::Escape: return KeyCode::Escape;
        case sf::Keyboard::Enter: return KeyCode::Enter;
        case sf::Keyboard::Space: return KeyCode::Space;
        case sf::Keyboard::Tab: return KeyCode::Tab;
        case sf::Keyboard::Backspace: return KeyCode::Backspace;
        case sf::Keyboard::Delete: return KeyCode::Delete;
        case sf::Keyboard::Home: return KeyCode::Home;
        case sf::Keyboard::End: return KeyCode::End;
        case sf::Keyboard::PageUp: return KeyCode::PageUp;
        case sf::Keyboard::PageDown: return KeyCode::PageDown;

        case sf::Keyboard::Numpad0: return KeyCode::Numpad0;
        case sf::Keyboard::Numpad1: return KeyCode::Numpad1;
        case sf::Keyboard::Numpad2: return KeyCode::Numpad2;
        case sf::Keyboard::Numpad3: return KeyCode::Numpad3;
        case sf::Keyboard::Numpad4: return KeyCode::Numpad4;
        case sf::Keyboard::Numpad5: return KeyCode::Numpad5;
        case sf::Keyboard::Numpad6: return KeyCode::Numpad6;
        case sf::Keyboard::Numpad7: return KeyCode::Numpad7;
        case sf::Keyboard::Numpad8: return KeyCode::Numpad8;
        case sf::Keyboard::Numpad9: return KeyCode::Numpad9;

        case sf::Keyboard::Comma: return KeyCode::Comma;
        case sf::Keyboard::Period: return KeyCode::Period;
        case sf::Keyboard::Slash: return KeyCode::Slash;
        case sf::Keyboard::Semicolon: return KeyCode::Semicolon;
        case sf::Keyboard::Quote: return KeyCode::Quote;
        case sf::Keyboard::LBracket: return KeyCode::LBracket;
        case sf::Keyboard::RBracket: return KeyCode::RBracket;
        case sf::Keyboard::Backslash: return KeyCode::Backslash;
        case sf::Keyboard::Hyphen: return KeyCode::Minus;
        case sf::Keyboard::Equal: return KeyCode::Equal;

        default: return KeyCode::Unknown;
    }
}

sf::Keyboard::Key SfmlInputHandler::toSfmlKey(KeyCode key) {
    switch (key) {
        case KeyCode::A: return sf::Keyboard::A;
        case KeyCode::B: return sf::Keyboard::B;
        case KeyCode::C: return sf::Keyboard::C;
        case KeyCode::D: return sf::Keyboard::D;
        case KeyCode::E: return sf::Keyboard::E;
        case KeyCode::F: return sf::Keyboard::F;
        case KeyCode::G: return sf::Keyboard::G;
        case KeyCode::H: return sf::Keyboard::H;
        case KeyCode::I: return sf::Keyboard::I;
        case KeyCode::J: return sf::Keyboard::J;
        case KeyCode::K: return sf::Keyboard::K;
        case KeyCode::L: return sf::Keyboard::L;
        case KeyCode::M: return sf::Keyboard::M;
        case KeyCode::N: return sf::Keyboard::N;
        case KeyCode::O: return sf::Keyboard::O;
        case KeyCode::P: return sf::Keyboard::P;
        case KeyCode::Q: return sf::Keyboard::Q;
        case KeyCode::R: return sf::Keyboard::R;
        case KeyCode::S: return sf::Keyboard::S;
        case KeyCode::T: return sf::Keyboard::T;
        case KeyCode::U: return sf::Keyboard::U;
        case KeyCode::V: return sf::Keyboard::V;
        case KeyCode::W: return sf::Keyboard::W;
        case KeyCode::X: return sf::Keyboard::X;
        case KeyCode::Y: return sf::Keyboard::Y;
        case KeyCode::Z: return sf::Keyboard::Z;

        case KeyCode::Num0: return sf::Keyboard::Num0;
        case KeyCode::Num1: return sf::Keyboard::Num1;
        case KeyCode::Num2: return sf::Keyboard::Num2;
        case KeyCode::Num3: return sf::Keyboard::Num3;
        case KeyCode::Num4: return sf::Keyboard::Num4;
        case KeyCode::Num5: return sf::Keyboard::Num5;
        case KeyCode::Num6: return sf::Keyboard::Num6;
        case KeyCode::Num7: return sf::Keyboard::Num7;
        case KeyCode::Num8: return sf::Keyboard::Num8;
        case KeyCode::Num9: return sf::Keyboard::Num9;

        case KeyCode::Left: return sf::Keyboard::Left;
        case KeyCode::Right: return sf::Keyboard::Right;
        case KeyCode::Up: return sf::Keyboard::Up;
        case KeyCode::Down: return sf::Keyboard::Down;

        case KeyCode::Escape: return sf::Keyboard::Escape;
        case KeyCode::Enter: return sf::Keyboard::Enter;
        case KeyCode::Space: return sf::Keyboard::Space;

        default: return sf::Keyboard::Unknown;
    }
}

MouseButton SfmlInputHandler::fromSfmlButton(sf::Mouse::Button button) {
    switch (button) {
        case sf::Mouse::Left: return MouseButton::Left;
        case sf::Mouse::Right: return MouseButton::Right;
        case sf::Mouse::Middle: return MouseButton::Middle;
        default: return MouseButton::Left;
    }
}

sf::Mouse::Button SfmlInputHandler::toSfmlButton(MouseButton button) {
    switch (button) {
        case MouseButton::Left: return sf::Mouse::Left;
        case MouseButton::Right: return sf::Mouse::Right;
        case MouseButton::Middle: return sf::Mouse::Middle;
        default: return sf::Mouse::Left;
    }
}

// ========== Factory Function ==========

std::unique_ptr<IInputHandler> createInputHandler(IRenderWindow* window) {
#ifdef USE_SFML
    return std::make_unique<SfmlInputHandler>(window);
#else
    // Web implementation will be created in Phase 6
    return nullptr;
#endif
}
