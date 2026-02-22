// loading-overlay.js
// Creates the loading overlay UI and exposes showGameLoading/hideGameLoading.
// Loaded as a standalone script so changes never require a WASM recompile.
(function () {
    // Inject CSS
    var style = document.createElement('style');
    style.textContent = [
        '#game-loading-overlay {',
        '  display: none;',
        '  position: fixed;',
        '  top: 0; left: 0; right: 0; bottom: 0;',
        '  background: rgba(0, 0, 0, 0.75);',
        '  z-index: 500;',
        '  justify-content: center;',
        '  align-items: center;',
        '}',
        '#game-loading-overlay.visible { display: flex; }',
        '#game-loading-box {',
        '  background: #222;',
        '  border: 1px solid #555;',
        '  border-radius: 8px;',
        '  padding: 32px 48px;',
        '  display: flex;',
        '  flex-direction: column;',
        '  align-items: center;',
        '  gap: 16px;',
        '  color: #ddd;',
        '  font-family: sans-serif;',
        '  font-size: 1.1em;',
        '}',
        '.loading-spinner {',
        '  width: 40px;',
        '  height: 40px;',
        '  border: 3px solid #444;',
        '  border-top-color: #aaa;',
        '  border-radius: 50%;',
        '  animation: spin 0.8s linear infinite;',
        '}',
        '@keyframes spin { to { transform: rotate(360deg); } }',
    ].join('\n');
    document.head.appendChild(style);

    // Inject HTML
    var overlay = document.createElement('div');
    overlay.id = 'game-loading-overlay';
    overlay.innerHTML =
        '<div id="game-loading-box">' +
        '<div class="loading-spinner"></div>' +
        '<p id="game-loading-message">Loading...</p>' +
        '</div>';
    document.body.appendChild(overlay);

    // Public API
    window.showGameLoading = function (msg) {
        document.getElementById('game-loading-message').textContent = msg || 'Loading...';
        document.getElementById('game-loading-overlay').classList.add('visible');
    };
    window.hideGameLoading = function () {
        document.getElementById('game-loading-overlay').classList.remove('visible');
    };
})();
