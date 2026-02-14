/**
 * IndexedDB Save File Manager for Blades of Exile WASM
 * Handles persistent storage of save files in the browser
 */

class SaveManager {
    constructor() {
        this.dbName = 'BoESaves';
        this.storeName = 'saveFiles';
        this.db = null;
    }

    /**
     * Initialize IndexedDB connection
     * @returns {Promise<void>}
     */
    init() {
        return new Promise((resolve, reject) => {
            const request = indexedDB.open(this.dbName, 1);

            request.onerror = () => {
                console.error('IndexedDB failed to open:', request.error);
                reject(request.error);
            };

            request.onsuccess = () => {
                this.db = request.result;
                console.log('IndexedDB initialized:', this.dbName);
                resolve();
            };

            request.onupgradeneeded = (event) => {
                const db = event.target.result;

                // Create object store if it doesn't exist
                if (!db.objectStoreNames.contains(this.storeName)) {
                    const objectStore = db.createObjectStore(this.storeName, { keyPath: 'filename' });
                    objectStore.createIndex('timestamp', 'timestamp', { unique: false });
                    objectStore.createIndex('scenario', 'scenario', { unique: false });
                    console.log('Created object store:', this.storeName);
                }
            };
        });
    }

    /**
     * List all save files sorted by timestamp (newest first)
     * @returns {Promise<Array>}
     */
    listFiles() {
        return new Promise((resolve, reject) => {
            if (!this.db) {
                reject(new Error('Database not initialized'));
                return;
            }

            const transaction = this.db.transaction([this.storeName], 'readonly');
            const objectStore = transaction.objectStore(this.storeName);
            const request = objectStore.getAll();

            request.onsuccess = () => {
                const files = request.result || [];
                // Sort by timestamp descending (newest first)
                files.sort((a, b) => b.timestamp - a.timestamp);
                resolve(files);
            };

            request.onerror = () => {
                console.error('Failed to list files:', request.error);
                reject(request.error);
            };
        });
    }

    /**
     * Save a file to IndexedDB
     * @param {string} filename - Name of the save file
     * @param {Uint8Array} data - Binary save data
     * @param {string} scenario - Optional scenario name
     * @returns {Promise<void>}
     */
    saveFile(filename, data, scenario = '') {
        return new Promise((resolve, reject) => {
            if (!this.db) {
                reject(new Error('Database not initialized'));
                return;
            }

            const transaction = this.db.transaction([this.storeName], 'readwrite');
            const objectStore = transaction.objectStore(this.storeName);

            const saveData = {
                filename: filename,
                data: data,
                timestamp: Date.now(),
                size: data.byteLength,
                scenario: scenario
            };

            const request = objectStore.put(saveData);

            request.onsuccess = () => {
                console.log('Saved file to IndexedDB:', filename, saveData.size, 'bytes');
                resolve();
            };

            request.onerror = () => {
                console.error('Failed to save file:', request.error);
                reject(request.error);
            };
        });
    }

    /**
     * Load a file from IndexedDB and write to MEMFS
     * @param {string} filename - Name of the save file
     * @returns {Promise<Uint8Array>}
     */
    loadFile(filename) {
        return new Promise((resolve, reject) => {
            if (!this.db) {
                reject(new Error('Database not initialized'));
                return;
            }

            const transaction = this.db.transaction([this.storeName], 'readonly');
            const objectStore = transaction.objectStore(this.storeName);
            const request = objectStore.get(filename);

            request.onsuccess = () => {
                if (!request.result) {
                    reject(new Error('File not found: ' + filename));
                    return;
                }

                const data = request.result.data;

                // Write to MEMFS
                try {
                    // Ensure directory exists
                    const dir = '/temp/Saves';
                    if (!FS.analyzePath(dir).exists) {
                        FS.mkdirTree(dir);
                    }

                    const path = dir + '/' + filename;
                    FS.writeFile(path, data);
                    console.log('Loaded file to MEMFS:', path, data.byteLength, 'bytes');
                    resolve(data);
                } catch (err) {
                    console.error('Failed to write to MEMFS:', err);
                    reject(err);
                }
            };

            request.onerror = () => {
                console.error('Failed to load file:', request.error);
                reject(request.error);
            };
        });
    }

    /**
     * Delete a file from IndexedDB
     * @param {string} filename - Name of the save file
     * @returns {Promise<void>}
     */
    deleteFile(filename) {
        return new Promise((resolve, reject) => {
            if (!this.db) {
                reject(new Error('Database not initialized'));
                return;
            }

            const transaction = this.db.transaction([this.storeName], 'readwrite');
            const objectStore = transaction.objectStore(this.storeName);
            const request = objectStore.delete(filename);

            request.onsuccess = () => {
                console.log('Deleted file from IndexedDB:', filename);
                resolve();
            };

            request.onerror = () => {
                console.error('Failed to delete file:', request.error);
                reject(request.error);
            };
        });
    }

    /**
     * Download a file from IndexedDB to local disk
     * @param {string} filename - Name of the save file
     * @returns {Promise<void>}
     */
    downloadFile(filename) {
        return new Promise((resolve, reject) => {
            if (!this.db) {
                reject(new Error('Database not initialized'));
                return;
            }

            const transaction = this.db.transaction([this.storeName], 'readonly');
            const objectStore = transaction.objectStore(this.storeName);
            const request = objectStore.get(filename);

            request.onsuccess = () => {
                if (!request.result) {
                    reject(new Error('File not found: ' + filename));
                    return;
                }

                try {
                    const blob = new Blob([request.result.data], {
                        type: 'application/octet-stream'
                    });
                    const url = URL.createObjectURL(blob);
                    const a = document.createElement('a');
                    a.href = url;
                    a.download = filename;
                    document.body.appendChild(a);
                    a.click();
                    document.body.removeChild(a);
                    URL.revokeObjectURL(url);
                    console.log('Downloaded file:', filename);
                    resolve();
                } catch (err) {
                    console.error('Failed to download file:', err);
                    reject(err);
                }
            };

            request.onerror = () => {
                console.error('Failed to retrieve file for download:', request.error);
                reject(request.error);
            };
        });
    }

    /**
     * Autosave with rotation (keeps max N autosaves)
     * @param {string} baseName - Base filename (e.g., "mysave.auto")
     * @param {number} maxCount - Maximum number of autosaves to keep
     * @param {string} reason - Reason for autosave (for logging)
     * @returns {Promise<string>} - Filename of created autosave
     */
    async autosave(baseName, maxCount, reason) {
        try {
            // List existing autosaves
            const allFiles = await this.listFiles();
            const autosaves = allFiles.filter(f => f.filename.startsWith(baseName));

            // Sort by timestamp (oldest first for deletion)
            autosaves.sort((a, b) => a.timestamp - b.timestamp);

            // Delete oldest if at limit
            while (autosaves.length >= maxCount) {
                const oldest = autosaves.shift();
                await this.deleteFile(oldest.filename);
                console.log('Deleted old autosave:', oldest.filename);
            }

            // Create new autosave filename
            const num = autosaves.length + 1;
            const filename = `${baseName}.${num}.exg`;

            console.log('Creating autosave:', filename, 'reason:', reason);
            return filename;
        } catch (err) {
            console.error('Autosave failed:', err);
            throw err;
        }
    }

    /**
     * Get total storage usage
     * @returns {Promise<number>} - Total bytes used
     */
    async getStorageUsage() {
        const files = await this.listFiles();
        return files.reduce((total, file) => total + (file.size || 0), 0);
    }

    /**
     * Clear all save files (with confirmation)
     * @returns {Promise<void>}
     */
    async clearAll() {
        if (!this.db) {
            throw new Error('Database not initialized');
        }

        const transaction = this.db.transaction([this.storeName], 'readwrite');
        const objectStore = transaction.objectStore(this.storeName);

        return new Promise((resolve, reject) => {
            const request = objectStore.clear();

            request.onsuccess = () => {
                console.log('Cleared all save files from IndexedDB');
                resolve();
            };

            request.onerror = () => {
                console.error('Failed to clear saves:', request.error);
                reject(request.error);
            };
        });
    }
}

// Initialize global instance
if (typeof Module !== 'undefined') {
    Module._saveDB = new SaveManager();

    // Auto-initialize when Module is ready
    if (Module.onRuntimeInitialized) {
        const originalOnInit = Module.onRuntimeInitialized;
        Module.onRuntimeInitialized = function() {
            originalOnInit();
            Module._saveDB.init().catch(err => {
                console.error('Failed to initialize SaveManager:', err);
            });
        };
    } else {
        Module.onRuntimeInitialized = function() {
            Module._saveDB.init().catch(err => {
                console.error('Failed to initialize SaveManager:', err);
            });
        };
    }
}
