/*
 * input_handler.hpp
 * BoE - Platform Abstraction Layer for Input
 *
 * Interface for platform-independent input handling
 * Abstracts SFML events (desktop) and browser events (web)
 */

#ifndef BOE_INPUT_HANDLER_HPP
#define BOE_INPUT_HANDLER_HPP

#include <string>
#include "location.hpp"

// Forward declarations
namespace sf {
    class Event;
}

/**
 * Platform-independent key codes
 * Based on common keyboard keys used in BoE
 */
enum class KeyCode {
    Unknown = -1,

    // Letters
    A, B, C, D, E, F, G, H, I, J, K, L, M,
    N, O, P, Q, R, S, T, U, V, W, X, Y, Z,

    // Numbers
    Num0, Num1, Num2, Num3, Num4, Num5, Num6, Num7, Num8, Num9,

    // Arrow keys
    Left, Right, Up, Down,

    // Function keys
    F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,

    // Modifiers
    LShift, RShift, LControl, RControl, LAlt, RAlt,

    // Special keys
    Escape, Enter, Space, Tab, Backspace, Delete,
    Home, End, PageUp, PageDown,

    // Numpad
    Numpad0, Numpad1, Numpad2, Numpad3, Numpad4,
    Numpad5, Numpad6, Numpad7, Numpad8, Numpad9,
    NumpadEnter, NumpadPlus, NumpadMinus,
    NumpadMultiply, NumpadDivide,

    // Punctuation
    Comma, Period, Slash, Semicolon, Quote,
    LBracket, RBracket, Backslash,
    Minus, Equal
};

/**
 * Mouse buttons
 */
enum class MouseButton {
    Left,
    Right,
    Middle
};

/**
 * Event types
 */
enum class EventType {
    None,
    Closed,
    KeyPressed,
    KeyReleased,
    MouseButtonPressed,
    MouseButtonReleased,
    MouseMoved,
    MouseWheelScrolled,
    Resized,
    LostFocus,
    GainedFocus
};

/**
 * Platform-independent input event
 */
struct InputEvent {
    EventType type = EventType::None;

    // Keyboard event data
    struct KeyEvent {
        KeyCode code = KeyCode::Unknown;
        bool alt = false;
        bool control = false;
        bool shift = false;
        bool system = false;
    } key;

    // Mouse button event data
    struct MouseButtonEvent {
        MouseButton button = MouseButton::Left;
        int x = 0;
        int y = 0;
    } mouseButton;

    // Mouse move event data
    struct MouseMoveEvent {
        int x = 0;
        int y = 0;
    } mouseMove;

    // Mouse wheel event data
    struct MouseWheelEvent {
        float delta = 0.0f;
        int x = 0;
        int y = 0;
    } mouseWheel;

    // Resize event data
    struct SizeEvent {
        unsigned int width = 0;
        unsigned int height = 0;
    } size;

#ifdef USE_SFML
    // Conversion from SFML event
    static InputEvent fromSfmlEvent(const sf::Event& sfEvent);
    // Conversion to SFML event (for compatibility)
    sf::Event toSfmlEvent() const;
#endif
};

/**
 * Platform-independent input handler interface
 */
class IInputHandler {
public:
    virtual ~IInputHandler() = default;

    // Poll for next event
    virtual bool pollEvent(InputEvent& event) = 0;

    // Check if a key is currently pressed (real-time input)
    virtual bool isKeyPressed(KeyCode key) const = 0;

    // Check if a mouse button is currently pressed
    virtual bool isMouseButtonPressed(MouseButton button) const = 0;

    // Get current mouse position
    virtual location getMousePosition() const = 0;
};

// Factory function for creating platform-specific implementations
class IRenderWindow;
std::unique_ptr<IInputHandler> createInputHandler(IRenderWindow* window);

#endif // BOE_INPUT_HANDLER_HPP
