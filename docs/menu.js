// menu.js - HTML/CSS menu bar for WASM Blades of Exile port
// Provides window.gameMenu API called from C++ via EM_ASM.
//
// NOTE: eMenu integer values must match boe.menus.hpp exactly.
// If the enum order changes there, update the data-action attributes in
// shell.html and the eMenu constants below.

(function () {
  'use strict';

  // -------------------------------------------------------
  // Close all open dropdowns
  // -------------------------------------------------------
  function closeAll() {
    var items = document.querySelectorAll('#game-menu-bar .menu-item.open');
    items.forEach(function (item) {
      item.classList.remove('open');
    });
  }

  // -------------------------------------------------------
  // Dispatch a menu action to C++ or JS handlers
  // -------------------------------------------------------
  function doAction(actionValue) {
    closeAll();

    if (actionValue === 'file-open') {
      // Load is handled entirely in JavaScript to avoid ASYNCIFY nesting
      // handleLoadGame is defined and exposed globally in events.js
      if (typeof window.handleLoadGame === 'function') {
        window.handleLoadGame();
      } else {
        console.error('[menu.js] window.handleLoadGame not available');
      }
      return;
    }

    var action = parseInt(actionValue, 10);
    if (!isNaN(action)) {
      try {
        // Use the queue version - safe to call while ASYNCIFY main loop is sleeping.
        // pollEvent() will dequeue and dispatch on the next main loop tick.
        Module.ccall('wasm_queue_menu_action', null, ['number'], [action]);
      } catch (e) {
        console.error('[menu.js] wasm_queue_menu_action failed:', e);
      }
    }

    // Refocus canvas so keyboard shortcuts keep working
    if (Module && Module.canvas) {
      Module.canvas.focus();
    }
  }

  // -------------------------------------------------------
  // Build or rebuild spell submenu entries
  // -------------------------------------------------------
  function buildSpellSubmenu(dropdownEl, spells, spellType) {
    // Remove any previously generated spell entries (data-spell attribute)
    var old = dropdownEl.querySelectorAll('li[data-spell]');
    old.forEach(function (el) { el.parentNode.removeChild(el); });

    if (!spells || spells.length === 0) return;

    // Group by level
    var byLevel = {};
    spells.forEach(function (spell) {
      var lvl = spell.level;
      if (!byLevel[lvl]) byLevel[lvl] = [];
      byLevel[lvl].push(spell);
    });

    var levels = Object.keys(byLevel).map(Number).sort(function (a, b) { return a - b; });

    levels.forEach(function (level) {
      // Level header (not clickable)
      var header = document.createElement('li');
      header.className = 'spell-level-header';
      header.setAttribute('data-spell', 'header');
      header.textContent = 'Level ' + level;
      dropdownEl.appendChild(header);

      byLevel[level].forEach(function (spell) {
        var li = document.createElement('li');
        li.setAttribute('data-spell', String(spell.id));

        var nameSpan = document.createElement('span');
        nameSpan.className = 'spell-name';
        nameSpan.textContent = spell.name;

        var costSpan = document.createElement('span');
        costSpan.className = 'spell-cost';
        costSpan.textContent = 'C ' + (spell.cost >= 0 ? spell.cost : '?');

        li.appendChild(nameSpan);
        li.appendChild(costSpan);

        // Capture loop variables
        (function (type, id) {
          li.addEventListener('click', function (e) {
            e.stopPropagation();
            closeAll();
            try {
              // Use the queue version - safe to call while ASYNCIFY main loop is sleeping.
              Module.ccall('wasm_queue_spell_cast', null, ['number', 'number'], [type, id]);
            } catch (err) {
              console.error('[menu.js] wasm_queue_spell_cast failed:', err);
            }
            if (Module && Module.canvas) Module.canvas.focus();
          });
        })(spellType, spell.id);

        dropdownEl.appendChild(li);
      });
    });
  }

  // -------------------------------------------------------
  // Refresh spell menus by querying C++ for castable spells
  // -------------------------------------------------------
  function refreshSpellMenus() {
    if (typeof Module === 'undefined' || !Module.ccall) return;

    try {
      var mageJson   = Module.ccall('wasm_get_spell_list_json', 'string', ['number'], [0]);
      var priestJson = Module.ccall('wasm_get_spell_list_json', 'string', ['number'], [1]);

      var mageSpells   = JSON.parse(mageJson);
      var priestSpells = JSON.parse(priestJson);

      var mageDrop   = document.getElementById('cast-mage-dropdown');
      var priestDrop = document.getElementById('cast-priest-dropdown');

      if (mageDrop)   buildSpellSubmenu(mageDrop,   mageSpells,   0);
      if (priestDrop) buildSpellSubmenu(priestDrop, priestSpells, 1);
    } catch (e) {
      console.error('[menu.js] refreshSpellMenus error:', e);
    }
  }

  // -------------------------------------------------------
  // Update menu enable/disable state based on game mode
  // Called from C++ via EM_ASM when menu_activate() fires.
  // -------------------------------------------------------
  function updateState(isStartup, partyInMemory) {
    // Menus that require an active game
    var gameMenuIds = ['menu-options', 'menu-actions', 'menu-monsters',
                       'menu-cast-mage', 'menu-cast-priest'];
    gameMenuIds.forEach(function (id) {
      var el = document.getElementById(id);
      if (!el) return;
      if (isStartup) {
        el.classList.add('menu-disabled');
      } else {
        el.classList.remove('menu-disabled');
      }
    });

    // File menu items that depend on game state
    // data-action="6" = Abort, "7" = Save Game, "8" = Save As
    var abortItem  = document.querySelector('#game-menu-bar [data-action="6"]');
    var saveItem   = document.querySelector('#game-menu-bar [data-action="7"]');
    var saveAsItem = document.querySelector('#game-menu-bar [data-action="8"]');

    if (isStartup) {
      if (abortItem)  abortItem.classList.add('disabled');
      if (saveItem)   saveItem.classList.add('disabled');
      // Save As is available at startup only if a party is in memory
      if (saveAsItem) {
        if (partyInMemory) {
          saveAsItem.classList.remove('disabled');
        } else {
          saveAsItem.classList.add('disabled');
        }
      }
    } else {
      if (abortItem)  abortItem.classList.remove('disabled');
      if (saveItem)   saveItem.classList.remove('disabled');
      if (saveAsItem) saveAsItem.classList.remove('disabled');
    }
  }

  // -------------------------------------------------------
  // Initialize menu bar click handling
  // -------------------------------------------------------
  function initMenu() {
    var menuBar = document.getElementById('game-menu-bar');
    if (!menuBar) {
      console.warn('[menu.js] #game-menu-bar not found');
      return;
    }

    // Toggle dropdown when clicking a top-level menu item
    menuBar.addEventListener('click', function (e) {
      // Check if click is on a top-level menu-item (not a dropdown child)
      var item = e.target.closest('.menu-item');
      if (!item) return;

      // Ignore clicks that landed inside a dropdown
      if (e.target.closest('.dropdown')) return;

      // Don't open disabled menus
      if (item.classList.contains('menu-disabled')) {
        closeAll();
        return;
      }

      var wasOpen = item.classList.contains('open');
      closeAll();
      if (!wasOpen) {
        item.classList.add('open');
      }
    });

    // Handle dropdown item clicks (for static items with data-action)
    menuBar.addEventListener('click', function (e) {
      var li = e.target.closest('.dropdown > li');
      if (!li) return;

      // Skip non-interactive items
      if (li.classList.contains('separator')) return;
      if (li.classList.contains('disabled')) return;
      if (li.classList.contains('spell-level-header')) return;

      // Skip spell entries - they have their own click listeners
      if (li.dataset.spell && li.dataset.spell !== 'header') return;

      var action = li.dataset.action;
      if (action) {
        e.stopPropagation();
        doAction(action);
      }
    });

    // Close dropdowns when clicking anywhere outside the menu bar
    document.addEventListener('click', function (e) {
      if (!menuBar.contains(e.target)) {
        closeAll();
      }
    });

    console.log('[menu.js] Menu bar initialized');
  }

  // -------------------------------------------------------
  // Public API (called from C++ via EM_ASM and from events.js)
  // -------------------------------------------------------
  window.gameMenu = {
    updateState: updateState,
    refreshSpellMenus: refreshSpellMenus,
    close: closeAll
  };

  // Initialize as soon as DOM is ready
  if (document.readyState === 'loading') {
    document.addEventListener('DOMContentLoaded', initMenu);
  } else {
    initMenu();
  }

})();
