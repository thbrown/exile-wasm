// events.js - Bridge browser input events to C++ event queue
// Sends mouse and keyboard events from the canvas to the C++ side
// via Module.ccall to exported push_mouse_event / push_key_event functions.

(function () {
  "use strict";

  // Wait for the Module to be ready
  function initEvents() {
    var canvas = Module.canvas;
    if (!canvas) {
      console.error("events.js: No canvas found on Module");
      return;
    }

    // Make canvas focusable
    canvas.setAttribute("tabindex", "0");
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
      KeyA: 0,
      KeyB: 1,
      KeyC: 2,
      KeyD: 3,
      KeyE: 4,
      KeyF: 5,
      KeyG: 6,
      KeyH: 7,
      KeyI: 8,
      KeyJ: 9,
      KeyK: 10,
      KeyL: 11,
      KeyM: 12,
      KeyN: 13,
      KeyO: 14,
      KeyP: 15,
      KeyQ: 16,
      KeyR: 17,
      KeyS: 18,
      KeyT: 19,
      KeyU: 20,
      KeyV: 21,
      KeyW: 22,
      KeyX: 23,
      KeyY: 24,
      KeyZ: 25,
      Digit0: 26,
      Digit1: 27,
      Digit2: 28,
      Digit3: 29,
      Digit4: 30,
      Digit5: 31,
      Digit6: 32,
      Digit7: 33,
      Digit8: 34,
      Digit9: 35,
      Escape: 36,
      ControlLeft: 37,
      ShiftLeft: 38,
      AltLeft: 39,
      MetaLeft: 40,
      ControlRight: 41,
      ShiftRight: 42,
      AltRight: 43,
      MetaRight: 44,
      ContextMenu: 45,
      BracketLeft: 46,
      BracketRight: 47,
      Semicolon: 48,
      Comma: 49,
      Period: 50,
      Quote: 51,
      Slash: 52,
      Backslash: 53,
      Backquote: 54,
      Equal: 55,
      Minus: 56,
      Space: 57,
      Enter: 58,
      Backspace: 59,
      Tab: 60,
      PageUp: 61,
      PageDown: 62,
      End: 63,
      Home: 64,
      Insert: 65,
      Delete: 66,
      NumpadAdd: 67,
      NumpadSubtract: 68,
      NumpadMultiply: 69,
      NumpadDivide: 70,
      ArrowLeft: 71,
      ArrowRight: 72,
      ArrowUp: 73,
      ArrowDown: 74,
      Numpad0: 75,
      Numpad1: 76,
      Numpad2: 77,
      Numpad3: 78,
      Numpad4: 79,
      Numpad5: 80,
      Numpad6: 81,
      Numpad7: 82,
      Numpad8: 83,
      Numpad9: 84,
      F1: 85,
      F2: 86,
      F3: 87,
      F4: 88,
      F5: 89,
      F6: 90,
      F7: 91,
      F8: 92,
      F9: 93,
      F10: 94,
      F11: 95,
      F12: 96,
      F13: 97,
      F14: 98,
      F15: 99,
      Pause: 100,
    };

    // Map browser mouse button to sf::Mouse::Button
    function mapMouseButton(browserButton) {
      switch (browserButton) {
        case 0:
          return 0; // Left
        case 2:
          return 1; // Right
        case 1:
          return 2; // Middle
        case 3:
          return 3; // XButton1
        case 4:
          return 4; // XButton2
        default:
          return 0;
      }
    }

    // Get mouse position relative to canvas, accounting for CSS scaling
    function getCanvasCoords(event) {
      var rect = canvas.getBoundingClientRect();
      var scaleX = canvas.width / rect.width;
      var scaleY = canvas.height / rect.height;
      return {
        x: Math.floor((event.clientX - rect.left) * scaleX),
        y: Math.floor((event.clientY - rect.top) * scaleY),
      };
    }

    // Mouse events
    canvas.addEventListener("mousedown", function (event) {
      event.preventDefault();
      canvas.focus();
      var coords = getCanvasCoords(event);
      var button = mapMouseButton(event.button);
      Module.ccall(
        "push_mouse_event",
        null,
        ["number", "number", "number", "number"],
        [MOUSE_PRESSED, coords.x, coords.y, button],
      );
    });

    canvas.addEventListener("mouseup", function (event) {
      event.preventDefault();
      var coords = getCanvasCoords(event);
      var button = mapMouseButton(event.button);
      Module.ccall(
        "push_mouse_event",
        null,
        ["number", "number", "number", "number"],
        [MOUSE_RELEASED, coords.x, coords.y, button],
      );
    });

    canvas.addEventListener("mousemove", function (event) {
      var coords = getCanvasCoords(event);
      Module.ccall(
        "push_mouse_event",
        null,
        ["number", "number", "number", "number"],
        [MOUSE_MOVED, coords.x, coords.y, 0],
      );
    });

    // Keyboard events
    canvas.addEventListener("keydown", function (event) {
      // Prevent default for game keys to avoid browser shortcuts
      if (event.code in keyCodeMap) {
        event.preventDefault();
      }
      var sfKey = event.code in keyCodeMap ? keyCodeMap[event.code] : -1;
      Module.ccall(
        "push_key_event",
        null,
        ["number", "number", "number", "number", "number"],
        [
          KEY_PRESSED,
          sfKey,
          event.shiftKey ? 1 : 0,
          event.ctrlKey ? 1 : 0,
          event.altKey ? 1 : 0,
        ],
      );
    });

    canvas.addEventListener("keyup", function (event) {
      var sfKey = event.code in keyCodeMap ? keyCodeMap[event.code] : -1;
      Module.ccall(
        "push_key_event",
        null,
        ["number", "number", "number", "number", "number"],
        [
          KEY_RELEASED,
          sfKey,
          event.shiftKey ? 1 : 0,
          event.ctrlKey ? 1 : 0,
          event.altKey ? 1 : 0,
        ],
      );
    });

    // TextEntered events for text fields
    canvas.addEventListener("keypress", function (event) {
      if (event.charCode && !event.ctrlKey && !event.altKey && !event.metaKey) {
        Module.ccall(
          "push_key_event",
          null,
          ["number", "number", "number", "number", "number"],
          [KEY_TEXT, event.charCode, event.shiftKey ? 1 : 0, 0, 0],
        );
      }
    });

    // Prevent context menu on right-click
    canvas.addEventListener("contextmenu", function (event) {
      event.preventDefault();
    });

    console.log("events.js: Event bridge initialized");
  }

  // ========================================================
  // Drawing Module - shared drawing functions for RenderWindow
  // and RenderTexture (off-screen canvas)
  // ========================================================
  function initDrawingModule() {
    if (!Module.textureCache) Module.textureCache = {};
    if (!Module.textureDimensions) Module.textureDimensions = {};

    // Registry mapping context IDs to Canvas 2D contexts
    Module.drawContexts = {};

    // Counter for unique RenderTexture IDs
    Module._rtCounter = 0;

    // Register the main canvas context
    Module.registerMainCanvas = function () {
      if (Module.drawContexts["main"]) return;
      var canvas = Module.canvas;
      if (canvas && canvas.getContext) {
        Module.drawContexts["main"] = canvas.getContext("2d");
        console.log("Drawing module: main canvas registered");
      }
    };

    // Create an off-screen canvas and register its context
    Module.createOffscreenCtx = function (id, w, h) {
      var offCanvas = document.createElement("canvas");
      offCanvas.width = w;
      offCanvas.height = h;
      Module.drawContexts[id] = offCanvas.getContext("2d");
      // Store the canvas element itself so it can be used as a drawImage source
      Module.textureCache[id] = offCanvas;
      console.log(
        'Drawing module: created offscreen context "' + id + '" ' + w + "x" + h,
      );
    };

    // Clear a context by ID
    Module.clearCtx = function (id, r, g, b) {
      var ctx = Module.drawContexts[id];
      if (!ctx) return;
      ctx.fillStyle = "rgb(" + r + "," + g + "," + b + ")";
      ctx.fillRect(0, 0, ctx.canvas.width, ctx.canvas.height);
    };

    // Draw a sprite (from texture cache) to a context
    Module.drawSpriteToCtx = function (
      ctxId,
      filename,
      sx,
      sy,
      sw,
      sh,
      dx,
      dy,
      dw,
      dh,
    ) {
      var ctx = Module.drawContexts[ctxId];
      if (!ctx) return;

      var src = Module.textureCache[filename];
      if (!src) {
        // Try to load from filesystem
        try {
          var data = FS.readFile(filename);
          var blob = new Blob([data]);
          var url = URL.createObjectURL(blob);
          var img = new Image();
          img.onload = function () {
            // Store dimensions when image loads
            if (!Module.textureDimensions) Module.textureDimensions = {};
            Module.textureDimensions[filename] = {
              width: img.naturalWidth,
              height: img.naturalHeight,
            };
            console.log(
              "Texture loaded: " +
                filename +
                " (" +
                img.naturalWidth +
                "x" +
                img.naturalHeight +
                ")",
            );
            // Request a redraw to show newly loaded texture
            if (Module.ccall) {
              try {
                Module.ccall("request_redraw", null, [], []);
              } catch (e) {
                console.warn("Failed to request redraw:", e);
              }
            }
          };
          img.src = url;
          Module.textureCache[filename] = img;
          src = img;
        } catch (e) {
          console.warn("Failed to load texture: " + filename, e);
          ctx.fillStyle = "red";
          ctx.fillRect(dx, dy, dw, dh);
          return;
        }
      }

      // Check if source is ready (Image needs to be loaded, Canvas is always ready)
      if (src instanceof HTMLCanvasElement) {
        ctx.drawImage(src, sx, sy, sw, sh, dx, dy, dw, dh);
      } else if (src.complete && src.naturalWidth > 0) {
        ctx.drawImage(src, sx, sy, sw, sh, dx, dy, dw, dh);
      } else {
        // Image not loaded yet - draw placeholder
        ctx.fillStyle = "yellow";
        ctx.fillRect(dx, dy, dw, dh);
        console.log("Texture not loaded yet, drawing yellow: " + filename);
      }
    };

    // Draw text to a context
    Module.drawTextToCtx = function (ctxId, text, x, y, size, r, g, b, a) {
      var ctx = Module.drawContexts[ctxId];
      if (!ctx) return;
      ctx.font = size + "px sans-serif";
      ctx.fillStyle = "rgba(" + r + "," + g + "," + b + "," + a / 255 + ")";
      ctx.textBaseline = "top";
      ctx.fillText(text, x, y);
    };

    // Draw a filled rectangle to a context
    Module.drawRectToCtx = function (ctxId, x, y, w, h, r, g, b, a) {
      var ctx = Module.drawContexts[ctxId];
      if (!ctx) return;
      ctx.fillStyle = "rgba(" + r + "," + g + "," + b + "," + a / 255 + ")";
      ctx.fillRect(x, y, w, h);
    };

    // Draw a rectangle outline to a context
    Module.drawFrameRectToCtx = function (
      ctxId,
      x,
      y,
      w,
      h,
      r,
      g,
      b,
      a,
      thickness,
    ) {
      var ctx = Module.drawContexts[ctxId];
      if (!ctx) return;
      ctx.strokeStyle = "rgba(" + r + "," + g + "," + b + "," + a / 255 + ")";
      ctx.lineWidth = thickness;
      ctx.strokeRect(x, y, w, h);
    };

    console.log("events.js: Drawing module initialized");
  }

  // Hook into Module's initialization
  if (typeof Module !== "undefined") {
    var originalOnRuntimeInitialized = Module.onRuntimeInitialized;
    Module.onRuntimeInitialized = function () {
      if (originalOnRuntimeInitialized) originalOnRuntimeInitialized();
      initDrawingModule();
      initEvents();
    };
  }
})();
