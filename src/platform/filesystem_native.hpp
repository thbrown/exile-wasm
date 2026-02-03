/*
 * filesystem_native.hpp
 * BoE - Native Implementation of File System
 *
 * Desktop implementation using std::filesystem (C++17) or boost::filesystem
 */

#ifndef BOE_FILESYSTEM_NATIVE_HPP
#define BOE_FILESYSTEM_NATIVE_HPP

#include "filesystem.hpp"

// Use std::filesystem if available (C++17), otherwise boost::filesystem
#if __cplusplus >= 201703L && defined(__has_include)
    #if __has_include(<filesystem>)
        #define USE_STD_FILESYSTEM
        #include <filesystem>
        namespace fs_impl = std::filesystem;
    #else
        #include <boost/filesystem.hpp>
        namespace fs_impl = boost::filesystem;
    #endif
#else
    #include <boost/filesystem.hpp>
    namespace fs_impl = boost::filesystem;
#endif

/**
 * Native (desktop) implementation of IFileSystem
 * Uses std::filesystem or boost::filesystem
 */
class NativeFileSystem : public IFileSystem {
public:
    NativeFileSystem() = default;
    virtual ~NativeFileSystem() = default;

    // IFileSystem interface
    bool exists(const Path& path) const override;
    bool is_regular_file(const Path& path) const override;
    bool is_directory(const Path& path) const override;
    uint64_t file_size(const Path& path) const override;

    bool remove(const Path& path) override;
    bool copy_file(const Path& from, const Path& to, bool overwrite = false) override;
    bool rename(const Path& from, const Path& to) override;

    bool create_directory(const Path& path) override;
    bool create_directories(const Path& path) override;
    std::vector<Path> list_directory(const Path& path) const override;

    std::vector<uint8_t> read_file(const Path& path) const override;
    bool write_file(const Path& path, const std::vector<uint8_t>& data) override;

    Path current_path() const override;
    void current_path(const Path& path) override;

    Path temp_directory_path() const override;

    Path show_open_dialog(const std::string& title, const std::string& filter = "") override;
    Path show_save_dialog(const std::string& title, const std::string& default_name = "", const std::string& filter = "") override;

private:
    fs_impl::path toNativePath(const Path& path) const;
    Path fromNativePath(const fs_impl::path& path) const;
};

#endif // BOE_FILESYSTEM_NATIVE_HPP
