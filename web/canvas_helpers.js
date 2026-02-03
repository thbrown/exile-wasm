/*
 * canvas_helpers.js
 * BoE - Canvas 2D Helper Functions
 *
 * JavaScript functions called from C++ via EM_ASM to perform Canvas 2D operations
 */

var BoE = BoE || {};

BoE.Canvas = {
    // Storage for canvases and contexts
    canvases: [],
    contexts: [],
    images: [],
    mainCanvas: null,
    mainContext: null,

    /**
     * Initialize the main canvas
     */
    init: function() {
        this.mainCanvas = document.getElementById('canvas');
        if (!this.mainCanvas) {
            console.error('Main canvas element not found!');
            return false;
        }
        this.mainContext = this.mainCanvas.getContext('2d');
        if (!this.mainContext) {
            console.error('Failed to get 2D context!');
            return false;
        }
        console.log('Canvas 2D initialized:', this.mainCanvas.width, 'x', this.mainCanvas.height);
        return true;
    },

    /**
     * Create an off-screen canvas
     * @param {number} width - Canvas width
     * @param {number} height - Canvas height
     * @returns {number} Canvas ID
     */
    createOffscreenCanvas: function(width, height) {
        var canvas = document.createElement('canvas');
        canvas.width = width;
        canvas.height = height;
        var ctx = canvas.getContext('2d');

        var id = this.canvases.length;
        this.canvases[id] = canvas;
        this.contexts[id] = ctx;

        console.log('Created off-screen canvas', id, ':', width, 'x', height);
        return id;
    },

    /**
     * Get canvas by ID
     */
    getCanvas: function(id) {
        if (id === -1) return this.mainCanvas;
        return this.canvases[id];
    },

    /**
     * Get context by ID
     */
    getContext: function(id) {
        if (id === -1) return this.mainContext;
        return this.contexts[id];
    },

    /**
     * Clear a canvas with a color
     */
    clear: function(canvasId, r, g, b, a) {
        var ctx = this.getContext(canvasId);
        if (!ctx) return;

        var canvas = this.getCanvas(canvasId);
        ctx.fillStyle = 'rgba(' + r + ',' + g + ',' + b + ',' + (a / 255.0) + ')';
        ctx.fillRect(0, 0, canvas.width, canvas.height);
    },

    /**
     * Draw a filled rectangle
     */
    fillRect: function(canvasId, x, y, width, height, r, g, b, a) {
        var ctx = this.getContext(canvasId);
        if (!ctx) return;

        ctx.fillStyle = 'rgba(' + r + ',' + g + ',' + b + ',' + (a / 255.0) + ')';
        ctx.fillRect(x, y, width, height);
    },

    /**
     * Draw a rectangle outline
     */
    strokeRect: function(canvasId, x, y, width, height, r, g, b, a, lineWidth) {
        var ctx = this.getContext(canvasId);
        if (!ctx) return;

        ctx.strokeStyle = 'rgba(' + r + ',' + g + ',' + b + ',' + (a / 255.0) + ')';
        ctx.lineWidth = lineWidth || 1;
        ctx.strokeRect(x, y, width, height);
    },

    /**
     * Draw a filled circle
     */
    fillCircle: function(canvasId, x, y, radius, r, g, b, a) {
        var ctx = this.getContext(canvasId);
        if (!ctx) return;

        ctx.fillStyle = 'rgba(' + r + ',' + g + ',' + b + ',' + (a / 255.0) + ')';
        ctx.beginPath();
        ctx.arc(x, y, radius, 0, Math.PI * 2);
        ctx.fill();
    },

    /**
     * Draw a circle outline
     */
    strokeCircle: function(canvasId, x, y, radius, r, g, b, a, lineWidth) {
        var ctx = this.getContext(canvasId);
        if (!ctx) return;

        ctx.strokeStyle = 'rgba(' + r + ',' + g + ',' + b + ',' + (a / 255.0) + ')';
        ctx.lineWidth = lineWidth || 1;
        ctx.beginPath();
        ctx.arc(x, y, radius, 0, Math.PI * 2);
        ctx.stroke();
    },

    /**
     * Draw a line
     */
    drawLine: function(canvasId, x1, y1, x2, y2, r, g, b, a, lineWidth) {
        var ctx = this.getContext(canvasId);
        if (!ctx) return;

        ctx.strokeStyle = 'rgba(' + r + ',' + g + ',' + b + ',' + (a / 255.0) + ')';
        ctx.lineWidth = lineWidth || 1;
        ctx.beginPath();
        ctx.moveTo(x1, y1);
        ctx.lineTo(x2, y2);
        ctx.stroke();
    },

    /**
     * Draw text
     */
    drawText: function(canvasId, text, x, y, fontName, fontSize, r, g, b, a) {
        var ctx = this.getContext(canvasId);
        if (!ctx) return;

        ctx.font = fontSize + 'px ' + fontName;
        ctx.fillStyle = 'rgba(' + r + ',' + g + ',' + b + ',' + (a / 255.0) + ')';
        ctx.fillText(text, x, y);
    },

    /**
     * Measure text width
     */
    measureText: function(canvasId, text, fontName, fontSize) {
        var ctx = this.getContext(canvasId);
        if (!ctx) return 0;

        ctx.font = fontSize + 'px ' + fontName;
        return ctx.measureText(text).width;
    },

    /**
     * Load an image from a file path (synchronous version using preloaded data)
     * @param {string} path - File path in Emscripten VFS
     * @returns {number} Image ID (-1 on error)
     */
    loadImageFromPath: function(path) {
        try {
            // Read file data from Emscripten VFS
            var data = FS.readFile(path);

            // Create a Blob from the data
            var blob = new Blob([data]);
            var url = URL.createObjectURL(blob);

            // Create image synchronously by storing the URL
            // The actual image will load asynchronously but we can use it immediately
            var img = new Image();
            var id = this.images.length;

            // Store a placeholder
            this.images[id] = {
                element: img,
                loaded: false,
                width: 0,
                height: 0
            };

            img.onload = function() {
                BoE.Canvas.images[id].loaded = true;
                BoE.Canvas.images[id].width = img.width;
                BoE.Canvas.images[id].height = img.height;
                console.log('Loaded image', id, 'from', path, ':', img.width, 'x', img.height);
                URL.revokeObjectURL(url);
            };

            img.onerror = function() {
                console.error('Failed to load image from path:', path);
                BoE.Canvas.images[id] = null;
                URL.revokeObjectURL(url);
            };

            img.src = url;
            return id;
        } catch (e) {
            console.error('Error loading image from path:', path, e);
            return -1;
        }
    },

    /**
     * Load an image from a URL or data URL (async)
     * @param {string} url - Image URL or data URL
     * @returns {Promise<number>} Promise that resolves to Image ID (-1 on error)
     */
    loadImage: function(url) {
        return new Promise((resolve, reject) => {
            var img = new Image();
            img.onload = function() {
                var id = BoE.Canvas.images.length;
                BoE.Canvas.images[id] = {
                    element: img,
                    loaded: true,
                    width: img.width,
                    height: img.height
                };
                console.log('Loaded image', id, ':', img.width, 'x', img.height);
                resolve(id);
            };
            img.onerror = function() {
                console.error('Failed to load image:', url);
                resolve(-1);
            };
            img.src = url;
        });
    },

    /**
     * Create a blank image
     * @param {number} width - Image width
     * @param {number} height - Image height
     * @returns {number} Image ID
     */
    createImage: function(width, height) {
        var canvas = document.createElement('canvas');
        canvas.width = width;
        canvas.height = height;

        var id = this.images.length;
        this.images[id] = {
            element: canvas,
            loaded: true,
            width: width,
            height: height
        };

        return id;
    },

    /**
     * Get image dimensions
     * @param {number} imageId - Image ID
     * @returns {object} {width, height} or null
     */
    getImageSize: function(imageId) {
        if (imageId < 0 || imageId >= this.images.length || !this.images[imageId]) {
            return null;
        }
        var img = this.images[imageId];
        return {
            width: img.width || img.element.width,
            height: img.height || img.element.height
        };
    },

    /**
     * Draw an image or part of it
     */
    drawImage: function(canvasId, imageId, sx, sy, sw, sh, dx, dy, dw, dh) {
        var ctx = this.getContext(canvasId);
        var imgObj = this.images[imageId];

        if (!ctx || !imgObj) {
            console.error('Invalid canvas or image ID', canvasId, imageId);
            return;
        }

        // Get the actual image element (could be Image or Canvas)
        var img = imgObj.element || imgObj;

        if (!imgObj.loaded) {
            console.warn('Image', imageId, 'not yet loaded');
            return;
        }

        try {
            if (arguments.length === 4) {
                // Simple draw: drawImage(canvasId, imageId, dx, dy)
                ctx.drawImage(img, sx, sy);
            } else if (arguments.length === 6) {
                // Scaled draw: drawImage(canvasId, imageId, dx, dy, dw, dh)
                ctx.drawImage(img, sx, sy, sw, sh);
            } else {
                // Full control: source rect and dest rect
                ctx.drawImage(img, sx, sy, sw, sh, dx, dy, dw, dh);
            }
        } catch (e) {
            console.error('Error drawing image:', e);
        }
    },

    /**
     * Draw one canvas onto another
     */
    drawCanvas: function(destCanvasId, srcCanvasId, sx, sy, sw, sh, dx, dy, dw, dh) {
        var destCtx = this.getContext(destCanvasId);
        var srcCanvas = this.getCanvas(srcCanvasId);

        if (!destCtx || !srcCanvas) {
            console.error('Invalid canvas ID', destCanvasId, srcCanvasId);
            return;
        }

        try {
            if (arguments.length === 4) {
                // Simple draw: drawCanvas(destId, srcId, dx, dy)
                destCtx.drawImage(srcCanvas, sx, sy);
            } else if (arguments.length === 6) {
                // Scaled draw
                destCtx.drawImage(srcCanvas, sx, sy, sw, sh);
            } else {
                // Full control
                destCtx.drawImage(srcCanvas, sx, sy, sw, sh, dx, dy, dw, dh);
            }
        } catch (e) {
            console.error('Error drawing canvas:', e);
        }
    },

    /**
     * Set clipping rectangle
     */
    setClipRect: function(canvasId, x, y, width, height) {
        var ctx = this.getContext(canvasId);
        if (!ctx) return;

        ctx.save();
        ctx.beginPath();
        ctx.rect(x, y, width, height);
        ctx.clip();
    },

    /**
     * Reset clipping
     */
    resetClip: function(canvasId) {
        var ctx = this.getContext(canvasId);
        if (!ctx) return;

        ctx.restore();
    },

    /**
     * Save canvas state
     */
    save: function(canvasId) {
        var ctx = this.getContext(canvasId);
        if (ctx) ctx.save();
    },

    /**
     * Restore canvas state
     */
    restore: function(canvasId) {
        var ctx = this.getContext(canvasId);
        if (ctx) ctx.restore();
    },

    /**
     * Set global alpha
     */
    setGlobalAlpha: function(canvasId, alpha) {
        var ctx = this.getContext(canvasId);
        if (ctx) ctx.globalAlpha = alpha;
    },

    /**
     * Set blend mode
     */
    setBlendMode: function(canvasId, mode) {
        var ctx = this.getContext(canvasId);
        if (!ctx) return;

        // mode: 0=none(source-over), 1=alpha(source-over), 2=add(lighter), 3=multiply
        switch(mode) {
            case 0:
            case 1:
                ctx.globalCompositeOperation = 'source-over';
                break;
            case 2:
                ctx.globalCompositeOperation = 'lighter';
                break;
            case 3:
                ctx.globalCompositeOperation = 'multiply';
                break;
            default:
                ctx.globalCompositeOperation = 'source-over';
        }
    }
};

// Initialize when the page loads
if (typeof Module !== 'undefined') {
    Module.onRuntimeInitialized = (function(orig) {
        return function() {
            if (orig) orig();
            BoE.Canvas.init();
        };
    })(Module.onRuntimeInitialized);
}
