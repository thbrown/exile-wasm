/**
 * File Dialog Manager for Blades of Exile WASM
 * HTML-based file picker for save/load operations
 */

class FileDialog {
    constructor() {
        this.overlay = null;
        this.dialog = null;
        this.fileList = null;
        this.filenameInput = null;
        this.selectedFile = null;
        this.isSaving = false;
        this.resolveCallback = null;
        this.rejectCallback = null;

        this.init();
    }

    /**
     * Initialize dialog HTML structure
     */
    init() {
        // Create overlay
        const overlay = document.createElement('div');
        overlay.className = 'boe-file-dialog-overlay';
        overlay.id = 'boe-file-dialog-overlay';

        overlay.innerHTML = `
            <div class="boe-file-dialog">
                <div class="boe-file-dialog-header">
                    <h2 id="boe-dialog-title">Load Game</h2>
                </div>
                <div class="boe-file-dialog-body">
                    <div id="boe-error-message" style="display: none;" class="boe-error"></div>

                    <div class="boe-file-list-container">
                        <table class="boe-file-list">
                            <thead>
                                <tr>
                                    <th>Filename</th>
                                    <th>Scenario</th>
                                    <th>Date</th>
                                    <th>Size</th>
                                </tr>
                            </thead>
                            <tbody id="boe-file-list-body">
                                <tr class="boe-file-list-empty">
                                    <td colspan="4">No save files found</td>
                                </tr>
                            </tbody>
                        </table>
                    </div>

                    <div class="boe-file-input-section">
                        <label for="boe-filename-input">Filename:</label>
                        <input type="text" id="boe-filename-input" placeholder="Enter filename..." autocomplete="off">
                    </div>
                </div>
                <div class="boe-file-dialog-footer">
                    <div class="boe-file-dialog-footer-left">
                        <input type="file" id="boe-file-import" accept=".exg,.boe,.SAV,.mac" style="display:none">
                        <button id="boe-btn-import" class="boe-btn-info">Import File...</button>
                        <button id="boe-btn-delete" class="boe-btn-danger" disabled>Delete</button>
                        <button id="boe-btn-download" class="boe-btn-info" disabled>Download</button>
                    </div>
                    <div class="boe-file-dialog-footer-right">
                        <button id="boe-btn-cancel" class="boe-btn-secondary">Cancel</button>
                        <button id="boe-btn-confirm" class="boe-btn-primary" disabled>Load</button>
                    </div>
                </div>
            </div>
        `;

        document.body.appendChild(overlay);

        // Cache DOM elements
        this.overlay = overlay;
        this.dialog = overlay.querySelector('.boe-file-dialog');
        this.fileListBody = document.getElementById('boe-file-list-body');
        this.filenameInput = document.getElementById('boe-filename-input');
        this.errorMessage = document.getElementById('boe-error-message');

        // Attach event listeners
        this.attachListeners();
    }

    /**
     * Attach event listeners to dialog elements
     */
    attachListeners() {
        // Prevent clicks inside dialog from closing it
        this.dialog.addEventListener('click', (e) => {
            e.stopPropagation();
        });

        // Close on overlay click
        this.overlay.addEventListener('click', () => {
            this.close(null);
        });

        // Filename input
        this.filenameInput.addEventListener('input', () => {
            this.updateButtonStates();
            this.hideError();
        });

        this.filenameInput.addEventListener('keydown', (e) => {
            if (e.key === 'Enter') {
                this.confirm();
            } else if (e.key === 'Escape') {
                this.close(null);
            }
        });

        // Buttons
        document.getElementById('boe-btn-cancel').addEventListener('click', () => {
            this.close(null);
        });

        document.getElementById('boe-btn-confirm').addEventListener('click', () => {
            this.confirm();
        });

        document.getElementById('boe-btn-delete').addEventListener('click', () => {
            this.deleteSelected();
        });

        document.getElementById('boe-btn-download').addEventListener('click', () => {
            this.downloadSelected();
        });

        // Import file button
        document.getElementById('boe-btn-import').addEventListener('click', () => {
            document.getElementById('boe-file-import').click();
        });

        // File input change
        document.getElementById('boe-file-import').addEventListener('change', async (e) => {
            await this.importFile(e.target.files[0]);
            e.target.value = ''; // Reset input
        });
    }

    /**
     * Show the file dialog
     * @param {boolean} isSaving - True for save dialog, false for load dialog
     * @param {string} defaultName - Default filename for save dialog
     * @returns {Promise<string>} - Selected filename or null if cancelled
     */
    show(isSaving, defaultName = '') {
        return new Promise((resolve, reject) => {
            this.resolveCallback = resolve;
            this.rejectCallback = reject;
            this.isSaving = isSaving;
            this.selectedFile = null;

            // Update UI for save/load mode
            const title = document.getElementById('boe-dialog-title');
            const confirmBtn = document.getElementById('boe-btn-confirm');

            if (isSaving) {
                title.textContent = 'Save Game';
                confirmBtn.textContent = 'Save';
                this.filenameInput.value = defaultName || '';
                this.filenameInput.focus();
            } else {
                title.textContent = 'Load Game';
                confirmBtn.textContent = 'Load';
                this.filenameInput.value = '';
            }

            this.hideError();
            this.loadFileList();
            this.updateButtonStates();
            this.overlay.classList.add('active');
        });
    }

    /**
     * Close the dialog
     * @param {string|null} result - Filename or null if cancelled
     */
    close(result) {
        this.overlay.classList.remove('active');
        if (this.resolveCallback) {
            this.resolveCallback(result);
            this.resolveCallback = null;
            this.rejectCallback = null;
        }
    }

    /**
     * Load and display file list from IndexedDB
     */
    async loadFileList() {
        try {
            const files = await Module._saveDB.listFiles();

            if (files.length === 0) {
                this.fileListBody.innerHTML = `
                    <tr class="boe-file-list-empty">
                        <td colspan="4">No save files found</td>
                    </tr>
                `;
                return;
            }

            this.fileListBody.innerHTML = files.map(file => {
                const date = new Date(file.timestamp).toLocaleString();
                const size = this.formatFileSize(file.size);
                const scenario = file.scenario || '—';

                return `
                    <tr data-filename="${this.escapeHtml(file.filename)}">
                        <td>${this.escapeHtml(file.filename)}</td>
                        <td class="boe-text-small">${this.escapeHtml(scenario)}</td>
                        <td class="boe-file-date">${date}</td>
                        <td class="boe-file-size">${size}</td>
                    </tr>
                `;
            }).join('');

            // Attach click handlers to rows
            this.fileListBody.querySelectorAll('tr').forEach(row => {
                row.addEventListener('click', () => {
                    this.selectFile(row.dataset.filename);
                });
            });
        } catch (err) {
            console.error('Failed to load file list:', err);
            this.showError('Failed to load save files');
        }
    }

    /**
     * Select a file from the list
     * @param {string} filename - Filename to select
     */
    selectFile(filename) {
        this.selectedFile = filename;
        this.filenameInput.value = filename;

        // Update visual selection
        this.fileListBody.querySelectorAll('tr').forEach(row => {
            row.classList.toggle('selected', row.dataset.filename === filename);
        });

        this.updateButtonStates();
        this.hideError();
    }

    /**
     * Update button enabled/disabled states
     */
    updateButtonStates() {
        const filename = this.filenameInput.value.trim();
        const hasFilename = filename.length > 0;
        const hasSelection = this.selectedFile !== null;

        const confirmBtn = document.getElementById('boe-btn-confirm');
        const deleteBtn = document.getElementById('boe-btn-delete');
        const downloadBtn = document.getElementById('boe-btn-download');

        confirmBtn.disabled = !hasFilename;
        deleteBtn.disabled = !hasSelection;
        downloadBtn.disabled = !hasSelection;
    }

    /**
     * Confirm action (save or load)
     */
    async confirm() {
        const filename = this.filenameInput.value.trim();

        if (!filename) {
            this.showError('Please enter a filename');
            return;
        }

        // Ensure .exg extension
        const finalFilename = filename.endsWith('.exg') ? filename : filename + '.exg';

        if (this.isSaving) {
            // Check if file exists
            const files = await Module._saveDB.listFiles();
            const exists = files.some(f => f.filename === finalFilename);

            if (exists) {
                if (!confirm(`File "${finalFilename}" already exists. Overwrite?`)) {
                    return;
                }
            }
        } else {
            // Load mode - verify file exists
            const files = await Module._saveDB.listFiles();
            const exists = files.some(f => f.filename === finalFilename);

            if (!exists) {
                this.showError('File not found: ' + finalFilename);
                return;
            }
        }

        this.close(finalFilename);
    }

    /**
     * Delete selected file
     */
    async deleteSelected() {
        if (!this.selectedFile) return;

        if (!confirm(`Delete "${this.selectedFile}"? This cannot be undone.`)) {
            return;
        }

        try {
            await Module._saveDB.deleteFile(this.selectedFile);
            this.selectedFile = null;
            this.filenameInput.value = '';
            this.loadFileList();
            this.updateButtonStates();
            this.hideError();
        } catch (err) {
            console.error('Failed to delete file:', err);
            this.showError('Failed to delete file');
        }
    }

    /**
     * Download selected file to disk
     */
    async downloadSelected() {
        if (!this.selectedFile) return;

        try {
            await Module._saveDB.downloadFile(this.selectedFile);
        } catch (err) {
            console.error('Failed to download file:', err);
            this.showError('Failed to download file');
        }
    }

    /**
     * Import file from local filesystem
     * @param {File} file - File object from input element
     */
    async importFile(file) {
        if (!file) return;

        try {
            // Read file into ArrayBuffer
            const arrayBuffer = await file.arrayBuffer();
            const data = new Uint8Array(arrayBuffer);

            // Ensure filename has .exg extension
            let filename = file.name;
            if (!filename.endsWith('.exg') && !filename.endsWith('.boe') &&
                !filename.endsWith('.SAV') && !filename.endsWith('.mac')) {
                filename += '.exg';
            }

            // Save to IndexedDB
            await Module._saveDB.saveFile(filename, data);

            // Write to MEMFS
            const memfsPath = '/temp/Saves/' + filename;
            FS.writeFile(memfsPath, data);

            console.log('[FileDialog] Imported file:', filename);

            // Refresh file list and select the imported file
            await this.loadFileList();
            this.selectFile(filename);
            this.filenameInput.value = filename;
            this.updateButtonStates();
        } catch (err) {
            console.error('Failed to import file:', err);
            this.showError('Failed to import file: ' + err.message);
        }
    }

    /**
     * Show error message
     * @param {string} message - Error message to display
     */
    showError(message) {
        this.errorMessage.textContent = message;
        this.errorMessage.style.display = 'block';
    }

    /**
     * Hide error message
     */
    hideError() {
        this.errorMessage.style.display = 'none';
    }

    /**
     * Format file size for display
     * @param {number} bytes - File size in bytes
     * @returns {string} - Formatted size string
     */
    formatFileSize(bytes) {
        if (!bytes) return '—';
        if (bytes < 1024) return bytes + ' B';
        if (bytes < 1024 * 1024) return (bytes / 1024).toFixed(1) + ' KB';
        return (bytes / (1024 * 1024)).toFixed(1) + ' MB';
    }

    /**
     * Escape HTML to prevent XSS
     * @param {string} str - String to escape
     * @returns {string} - Escaped string
     */
    escapeHtml(str) {
        const div = document.createElement('div');
        div.textContent = str;
        return div.innerHTML;
    }
}

// Initialize global instance and bridge functions
if (typeof Module !== 'undefined') {
    Module._fileDialog = new FileDialog();

    /**
     * Show load dialog (called from C++)
     * @returns {Promise<string>} - Filename string or empty string if cancelled
     */
    Module._showLoadDialog = async function() {
        try {
            const filename = await Module._fileDialog.show(false, null);
            if (!filename) return '';

            // Load from IndexedDB to MEMFS
            await Module._saveDB.loadFile(filename);

            // Return filename string directly
            return filename;
        } catch (err) {
            console.error('Load dialog error:', err);
            return '';
        }
    };

    /**
     * Show save dialog (called from C++)
     * @param {number} defaultNamePtr - Pointer to default filename string
     * @returns {Promise<string>} - Filename string or empty string if cancelled
     */
    Module._showSaveDialog = async function(defaultNamePtr) {
        try {
            const defaultName = defaultNamePtr ? UTF8ToString(defaultNamePtr) : '';
            const filename = await Module._fileDialog.show(true, defaultName);
            if (!filename) return '';

            // Return filename string directly
            return filename;
        } catch (err) {
            console.error('Save dialog error:', err);
            return '';
        }
    };

    /**
     * Sync save file from MEMFS to IndexedDB (called after C++ writes file)
     * @param {string} filename - Name of the file to sync
     */
    Module._syncSaveToIndexedDB = async function(filename) {
        try {
            const path = '/temp/Saves/' + filename;
            if (!FS.analyzePath(path).exists) {
                console.error('File not found in MEMFS:', path);
                return;
            }

            const data = FS.readFile(path);
            await Module._saveDB.saveFile(filename, data);
            console.log('Synced to IndexedDB:', filename);
        } catch (err) {
            console.error('Failed to sync save to IndexedDB:', err);
        }
    };
}
