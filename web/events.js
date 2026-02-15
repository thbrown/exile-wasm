// events.js - Bridge browser input events to C++ event queue
// Sends mouse and keyboard events from the canvas to the C++ side
// via Module.ccall to exported push_mouse_event / push_key_event functions.

(function () {
  "use strict";

  // Handle load game - called when user presses Ctrl+O
  // This avoids ASYNCIFY stack overflow by handling all async operations in JS
  async function handleLoadGame() {
    try {
      // Check if file dialog is available
      if (!Module._fileDialog || !Module._saveDB) {
        console.error('[JS] File dialog or save manager not initialized');
        alert('Load system not ready. Please try again.');
        return;
      }

      console.log('[JS] Showing file dialog for load...');

      // Show file dialog (isSave=false, no defaultName)
      const filename = await Module._fileDialog.show(false, null);

      if (!filename) {
        console.log('[JS] Load cancelled by user');
        return;
      }

      console.log('[JS] Loading file:', filename);

      // Load from IndexedDB to MEMFS
      await Module._saveDB.loadFile(filename);

      // Call C++ to load from the known MEMFS path
      const memfsPath = '/temp/Saves/' + filename;
      console.log('[JS] Calling C++ to load from:', memfsPath);

      Module.ccall('wasm_load_from_path', null, ['string'], [memfsPath]);

      console.log('[JS] Load complete!');

    } catch (err) {
      console.error('[JS] Load error:', err);
      alert('Failed to load game: ' + err.message);
    }
  }

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
      console.log('Mouse down at canvas coords:', coords.x, coords.y, 'button:', button);
      console.log('About to call push_mouse_event, Module.ccall exists:', typeof Module.ccall);
      try {
        Module.ccall(
          "push_mouse_event",
          null,
          ["number", "number", "number", "number"],
          [MOUSE_PRESSED, coords.x, coords.y, button],
        );
        console.log('push_mouse_event ccall succeeded');
      } catch(e) {
        console.error('push_mouse_event ccall failed:', e);
      }
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
      // Intercept Ctrl+O for load - handle entirely in JavaScript to avoid ASYNCIFY stack overflow
      if ((event.ctrlKey || event.metaKey) && event.code === 'KeyO') {
        event.preventDefault();
        event.stopPropagation();
        console.log('[JS] Ctrl+O intercepted - handling load in JavaScript');
        handleLoadGame();
        return;
      }

      // Prevent default for game keys to avoid browser shortcuts
      if (event.code in keyCodeMap) {
        event.preventDefault();
      }

      // Prevent browser shortcuts for Ctrl+key combinations used by the game
      if (event.ctrlKey || event.metaKey) {
        const key = event.code;
        // Prevent Ctrl+S (Save), Ctrl+O (Open), Ctrl+N (New), Ctrl+W (Wait/Close tab), Ctrl+A (Alchemy/Select all)
        if (key === 'KeyS' || key === 'KeyO' || key === 'KeyN' || key === 'KeyW' || key === 'KeyA' || key === 'KeyQ') {
          event.preventDefault();
        }
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

    // Image preloading
    Module.imageLoadCount = 0;
    Module.imageTotalCount = 0;

    // Preload all game graphics
    Module.preloadImages = function() {
      var graphicsFiles = [
        "bigscenpics.png", "blank.png", "booms.png", "buttons.png", "bwpats.png",
        "dlgbtnred.png", "dlogbtnhelp.png", "dlogbtnled.png", "dlogbtnlg.png",
        "dlogbtnmed.png", "dlogbtnsm.png", "dlogbtntall.png", "dlogpics.png",
        "dlogscrollled.png", "dlogscrollwh.png", "edbuttons.png", "edsplash.png",
        "fields.png", "fighthelp.png", "icon.png", "invenbtns.png", "inventory.png",
        "missiles.png", "monst1.png", "monst10.png", "monst11.png", "monst2.png",
        "monst3.png", "monst4.png", "monst5.png", "monst6.png", "monst7.png",
        "monst8.png", "monst9.png", "objects.png", "outhelp.png", "pcedbuttons.png",
        "pcedtitle.png", "pcs.png", "pixpats.png", "scenpics.png", "spidlogo.png",
        "startanim.png", "startbut.png", "startsplash.png", "startup.png",
        "statarea.png", "staticons.png", "talkportraits.png", "ter1.png", "ter10.png",
        "ter11.png", "ter12.png", "ter13.png", "ter14.png", "ter19.png", "ter2.png",
        "ter3.png", "ter4.png", "ter5.png", "ter6.png", "ter7.png", "ter8.png",
        "ter9.png", "teranim.png", "termap.png", "terscreen.png", "textbar.png",
        "tinyobj.png", "townhelp.png", "transcript.png", "trim.png", "vehicle.png"
      ];

      Module.imageTotalCount = graphicsFiles.length;
      console.log("Preloading " + Module.imageTotalCount + " images...");

      graphicsFiles.forEach(function(filename) {
        var fullPath = "/data/graphics/" + filename;
        try {
          var data = FS.readFile(fullPath);
          var blob = new Blob([data]);
          var url = URL.createObjectURL(blob);
          var img = new Image();

          img.onload = function() {
            Module.textureCache[fullPath] = img;
            Module.textureDimensions[fullPath] = {
              width: img.naturalWidth,
              height: img.naturalHeight
            };
            Module.imageLoadCount++;
            if (Module.imageLoadCount === Module.imageTotalCount) {
              console.log("All images loaded (" + Module.imageLoadCount + ")");
            }
          };

          img.onerror = function() {
            Module.imageLoadCount++;
            console.error("Failed to load image: " + fullPath);
          };

          img.src = url;
        } catch(e) {
          Module.imageLoadCount++;
          console.error("Failed to read image file: " + fullPath, e);
        }
      });
    };

    // Start preloading images
    Module.preloadImages();

    // Load fonts from Emscripten filesystem
    Module.loadFonts = function() {
      var fonts = {
        'BoE-Plain': '/data/fonts/plain.ttf',
        'BoE-Bold': '/data/fonts/bold.ttf',
        'BoE-Dungeon': '/data/fonts/dungeon.ttf',
        'BoE-Maidenword': '/data/fonts/maidenword.ttf'
      };

      Object.keys(fonts).forEach(function(fontName) {
        try {
          var fontPath = fonts[fontName];
          var fontData = FS.readFile(fontPath);
          var blob = new Blob([fontData], { type: 'font/truetype' });
          var url = URL.createObjectURL(blob);

          // Create @font-face style
          var style = document.createElement('style');
          style.textContent = '@font-face { font-family: "' + fontName + '"; src: url(' + url + '); }';
          document.head.appendChild(style);

          console.log('Loaded font: ' + fontName);
        } catch(e) {
          console.error('Failed to load font ' + fontName + ':', e);
        }
      });
    };

    // Load fonts when filesystem is ready
    Module.loadFonts();

    // Font name mapping (eFont enum to CSS font name)
    Module.fontMap = {
      0: 'BoE-Plain',      // FONT_PLAIN
      1: 'BoE-Bold',       // FONT_BOLD
      2: 'BoE-Dungeon',    // FONT_DUNGEON
      3: 'BoE-Maidenword'  // FONT_MAIDWORD
    };

    // Register the main canvas context
    Module.registerMainCanvas = function () {
      if (Module.drawContexts["main"]) return;
      var canvas = Module.canvas;
      if (canvas && canvas.getContext) {
        var ctx = canvas.getContext("2d");
        // Disable image smoothing for pixel-perfect rendering
        ctx.imageSmoothingEnabled = false;
        ctx.mozImageSmoothingEnabled = false;
        ctx.webkitImageSmoothingEnabled = false;
        ctx.msImageSmoothingEnabled = false;
        Module.drawContexts["main"] = ctx;
        console.log("Drawing module: main canvas registered (smoothing disabled)");
      }
    };

    // Create an off-screen canvas and register its context
    Module.createOffscreenCtx = function (id, w, h) {
      var offCanvas = document.createElement("canvas");
      offCanvas.width = w;
      offCanvas.height = h;
      var ctx = offCanvas.getContext("2d");
      // Disable image smoothing for pixel-perfect rendering
      ctx.imageSmoothingEnabled = false;
      ctx.mozImageSmoothingEnabled = false;
      ctx.webkitImageSmoothingEnabled = false;
      ctx.msImageSmoothingEnabled = false;
      Module.drawContexts[id] = ctx;
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
    Module.drawTextToCtx = function (ctxId, text, x, y, size, r, g, b, a, fontId) {
      var ctx = Module.drawContexts[ctxId];
      if (!ctx) return;

      // Get font family from font map (defaults to BoE-Plain if not specified)
      var fontFamily = Module.fontMap[fontId] || 'BoE-Plain';
      ctx.font = size + "px " + fontFamily;
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

    // Draw a line to a context
    Module.drawLineToCtx = function (ctxId, x0, y0, x1, y1, r, g, b, a, thickness) {
      var ctx = Module.drawContexts[ctxId];
      if (!ctx) return;
      ctx.strokeStyle = "rgba(" + r + "," + g + "," + b + "," + a / 255 + ")";
      ctx.lineWidth = thickness;
      ctx.beginPath();
      ctx.moveTo(x0, y0);
      ctx.lineTo(x1, y1);
      ctx.stroke();
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
