// events.js - Bridge browser input events to C++ event queue
// Sends mouse and keyboard events from the canvas to the C++ side
// via Module.ccall to exported push_mouse_event / push_key_event functions.

(function() {
    'use strict';

    // Wait for the Module to be ready
    function initEvents() {
        var canvas = Module.canvas;
        if (!canvas) {
            console.error('events.js: No canvas found on Module');
            return;
        }

        // Make canvas focusable
        canvas.setAttribute('tabindex', '0');
        canvas.focus();

        // Mouse event type constants (must match C++ side)
        var MOUSE_PRESSED = 0;
        var MOUSE_RELEASED = 1;
        var MOUSE_MOVED = 2;

        // Key event type constants (must match C++ side)
        var KEY_PRESSED = 0;
        var KEY_RELEASED = 1;
        var KEY_TEXT = 2;

        // Map browser event.code to sf::Keyboard::Key enum values
        // Based on sf::Keyboard::Key enum in event.hpp
        var keyCodeMap = {
            'KeyA': 0, 'KeyB': 1, 'KeyC': 2, 'KeyD': 3, 'KeyE': 4,
            'KeyF': 5, 'KeyG': 6, 'KeyH': 7, 'KeyI': 8, 'KeyJ': 9,
            'KeyK': 10, 'KeyL': 11, 'KeyM': 12, 'KeyN': 13, 'KeyO': 14,
            'KeyP': 15, 'KeyQ': 16, 'KeyR': 17, 'KeyS': 18, 'KeyT': 19,
            'KeyU': 20, 'KeyV': 21, 'KeyW': 22, 'KeyX': 23, 'KeyY': 24, 'KeyZ': 25,
            'Digit0': 26, 'Digit1': 27, 'Digit2': 28, 'Digit3': 29, 'Digit4': 30,
            'Digit5': 31, 'Digit6': 32, 'Digit7': 33, 'Digit8': 34, 'Digit9': 35,
            'Escape': 36,
            'ControlLeft': 37, 'ShiftLeft': 38, 'AltLeft': 39, 'MetaLeft': 40,
            'ControlRight': 41, 'ShiftRight': 42, 'AltRight': 43, 'MetaRight': 44,
            'ContextMenu': 45,
            'BracketLeft': 46, 'BracketRight': 47,
            'Semicolon': 48, 'Comma': 49, 'Period': 50, 'Quote': 51,
            'Slash': 52, 'Backslash': 53, 'Backquote': 54,
            'Equal': 55, 'Minus': 56,
            'Space': 57, 'Enter': 58, 'Backspace': 59, 'Tab': 60,
            'PageUp': 61, 'PageDown': 62, 'End': 63, 'Home': 64,
            'Insert': 65, 'Delete': 66,
            'NumpadAdd': 67, 'NumpadSubtract': 68, 'NumpadMultiply': 69, 'NumpadDivide': 70,
            'ArrowLeft': 71, 'ArrowRight': 72, 'ArrowUp': 73, 'ArrowDown': 74,
            'Numpad0': 75, 'Numpad1': 76, 'Numpad2': 77, 'Numpad3': 78, 'Numpad4': 79,
            'Numpad5': 80, 'Numpad6': 81, 'Numpad7': 82, 'Numpad8': 83, 'Numpad9': 84,
            'F1': 85, 'F2': 86, 'F3': 87, 'F4': 88, 'F5': 89,
            'F6': 90, 'F7': 91, 'F8': 92, 'F9': 93, 'F10': 94,
            'F11': 95, 'F12': 96, 'F13': 97, 'F14': 98, 'F15': 99,
            'Pause': 100
        };

        // Map browser mouse button to sf::Mouse::Button
        function mapMouseButton(browserButton) {
            switch (browserButton) {
                case 0: return 0; // Left
                case 2: return 1; // Right
                case 1: return 2; // Middle
                case 3: return 3; // XButton1
                case 4: return 4; // XButton2
                default: return 0;
            }
        }

        // Get mouse position relative to canvas, accounting for CSS scaling
        function getCanvasCoords(event) {
            var rect = canvas.getBoundingClientRect();
            var scaleX = canvas.width / rect.width;
            var scaleY = canvas.height / rect.height;
            return {
                x: Math.floor((event.clientX - rect.left) * scaleX),
                y: Math.floor((event.clientY - rect.top) * scaleY)
            };
        }

        // Mouse events
        canvas.addEventListener('mousedown', function(event) {
            event.preventDefault();
            canvas.focus();
            var coords = getCanvasCoords(event);
            var button = mapMouseButton(event.button);
            Module.ccall('push_mouse_event', null,
                ['number', 'number', 'number', 'number'],
                [MOUSE_PRESSED, coords.x, coords.y, button]);
        });

        canvas.addEventListener('mouseup', function(event) {
            event.preventDefault();
            var coords = getCanvasCoords(event);
            var button = mapMouseButton(event.button);
            Module.ccall('push_mouse_event', null,
                ['number', 'number', 'number', 'number'],
                [MOUSE_RELEASED, coords.x, coords.y, button]);
        });

        canvas.addEventListener('mousemove', function(event) {
            var coords = getCanvasCoords(event);
            Module.ccall('push_mouse_event', null,
                ['number', 'number', 'number', 'number'],
                [MOUSE_MOVED, coords.x, coords.y, 0]);
        });

        // Keyboard events
        canvas.addEventListener('keydown', function(event) {
            // Prevent default for game keys to avoid browser shortcuts
            if (event.code in keyCodeMap) {
                event.preventDefault();
            }
            var sfKey = (event.code in keyCodeMap) ? keyCodeMap[event.code] : -1;
            Module.ccall('push_key_event', null,
                ['number', 'number', 'number', 'number', 'number'],
                [KEY_PRESSED, sfKey, event.shiftKey ? 1 : 0, event.ctrlKey ? 1 : 0, event.altKey ? 1 : 0]);
        });

        canvas.addEventListener('keyup', function(event) {
            var sfKey = (event.code in keyCodeMap) ? keyCodeMap[event.code] : -1;
            Module.ccall('push_key_event', null,
                ['number', 'number', 'number', 'number', 'number'],
                [KEY_RELEASED, sfKey, event.shiftKey ? 1 : 0, event.ctrlKey ? 1 : 0, event.altKey ? 1 : 0]);
        });

        // TextEntered events for text fields
        canvas.addEventListener('keypress', function(event) {
            if (event.charCode && !event.ctrlKey && !event.altKey && !event.metaKey) {
                Module.ccall('push_key_event', null,
                    ['number', 'number', 'number', 'number', 'number'],
                    [KEY_TEXT, event.charCode, event.shiftKey ? 1 : 0, 0, 0]);
            }
        });

        // Prevent context menu on right-click
        canvas.addEventListener('contextmenu', function(event) {
            event.preventDefault();
        });

        console.log('events.js: Event bridge initialized');
    }

    // Hook into Module's initialization
    if (typeof Module !== 'undefined') {
        var originalOnRuntimeInitialized = Module.onRuntimeInitialized;
        Module.onRuntimeInitialized = function() {
            if (originalOnRuntimeInitialized) originalOnRuntimeInitialized();
            initEvents();
        };
    }
})();
