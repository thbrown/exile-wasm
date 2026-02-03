/*
 * filesystem.hpp
 * BoE - Platform Abstraction Layer for File System
 *
 * Interface for platform-independent file operations
 * Abstracts boost::filesystem/std::filesystem (desktop) and Emscripten VFS (web)
 */

#ifndef BOE_FILESYSTEM_HPP
#define BOE_FILESYSTEM_HPP

#include <string>
#include <vector>
#include <cstdint>
#include <memory>

/**
 * Platform-independent path representation
 */
class Path {
private:
    std::string pathStr;

public:
    Path() = default;
    Path(const std::string& p) : pathStr(p) {}
    Path(const char* p) : pathStr(p) {}

    // Path operations
    Path parent_path() const;
    Path filename() const;
    Path extension() const;
    std::string string() const { return pathStr; }
    const char* c_str() const { return pathStr.c_str(); }

    // Path manipulation
    Path& operator/=(const Path& other);
    Path operator/(const Path& other) const;

    bool empty() const { return pathStr.empty(); }
};

/**
 * Platform-independent file system interface
 */
class IFileSystem {
public:
    virtual ~IFileSystem() = default;

    // File existence and info
    virtual bool exists(const Path& path) const = 0;
    virtual bool is_regular_file(const Path& path) const = 0;
    virtual bool is_directory(const Path& path) const = 0;
    virtual uint64_t file_size(const Path& path) const = 0;

    // File operations
    virtual bool remove(const Path& path) = 0;
    virtual bool copy_file(const Path& from, const Path& to, bool overwrite = false) = 0;
    virtual bool rename(const Path& from, const Path& to) = 0;

    // Directory operations
    virtual bool create_directory(const Path& path) = 0;
    virtual bool create_directories(const Path& path) = 0;
    virtual std::vector<Path> list_directory(const Path& path) const = 0;

    // File I/O (convenience methods)
    virtual std::vector<uint8_t> read_file(const Path& path) const = 0;
    virtual bool write_file(const Path& path, const std::vector<uint8_t>& data) = 0;

    // Current directory
    virtual Path current_path() const = 0;
    virtual void current_path(const Path& path) = 0;

    // Special directories
    virtual Path temp_directory_path() const = 0;

    // Platform-specific operations
    // Show native file picker dialog (desktop) or HTML5 file input (web)
    virtual Path show_open_dialog(const std::string& title, const std::string& filter = "") = 0;
    virtual Path show_save_dialog(const std::string& title, const std::string& default_name = "", const std::string& filter = "") = 0;
};

// Factory function for creating platform-specific implementations
std::unique_ptr<IFileSystem> createFileSystem();

// Global filesystem instance (for convenience)
extern std::unique_ptr<IFileSystem> g_filesystem;

// Helper functions for common operations
namespace fs {
    inline bool exists(const Path& path) {
        return g_filesystem ? g_filesystem->exists(path) : false;
    }

    inline bool is_regular_file(const Path& path) {
        return g_filesystem ? g_filesystem->is_regular_file(path) : false;
    }

    inline bool is_directory(const Path& path) {
        return g_filesystem ? g_filesystem->is_directory(path) : false;
    }

    inline uint64_t file_size(const Path& path) {
        return g_filesystem ? g_filesystem->file_size(path) : 0;
    }

    inline std::vector<Path> list_directory(const Path& path) {
        return g_filesystem ? g_filesystem->list_directory(path) : std::vector<Path>();
    }

    inline std::vector<uint8_t> read_file(const Path& path) {
        return g_filesystem ? g_filesystem->read_file(path) : std::vector<uint8_t>();
    }

    inline bool write_file(const Path& path, const std::vector<uint8_t>& data) {
        return g_filesystem ? g_filesystem->write_file(path, data) : false;
    }
}

#endif // BOE_FILESYSTEM_HPP
