/*
 * filesystem_native.cpp
 * BoE - Native Implementation of File System
 */

#include "filesystem_native.hpp"
#include <fstream>
#include <iterator>

// ========== Path Implementation ==========

Path Path::parent_path() const {
    size_t pos = pathStr.find_last_of("/\\");
    if (pos == std::string::npos) {
        return Path();
    }
    return Path(pathStr.substr(0, pos));
}

Path Path::filename() const {
    size_t pos = pathStr.find_last_of("/\\");
    if (pos == std::string::npos) {
        return *this;
    }
    return Path(pathStr.substr(pos + 1));
}

Path Path::extension() const {
    std::string fname = filename().string();
    size_t pos = fname.find_last_of('.');
    if (pos == std::string::npos || pos == 0) {
        return Path();
    }
    return Path(fname.substr(pos));
}

Path& Path::operator/=(const Path& other) {
    if (!pathStr.empty() && pathStr.back() != '/' && pathStr.back() != '\\') {
        pathStr += '/';
    }
    pathStr += other.pathStr;
    return *this;
}

Path Path::operator/(const Path& other) const {
    Path result = *this;
    result /= other;
    return result;
}

// ========== NativeFileSystem Implementation ==========

fs_impl::path NativeFileSystem::toNativePath(const Path& path) const {
    return fs_impl::path(path.string());
}

Path NativeFileSystem::fromNativePath(const fs_impl::path& path) const {
    return Path(path.string());
}

bool NativeFileSystem::exists(const Path& path) const {
    try {
        return fs_impl::exists(toNativePath(path));
    } catch (...) {
        return false;
    }
}

bool NativeFileSystem::is_regular_file(const Path& path) const {
    try {
        return fs_impl::is_regular_file(toNativePath(path));
    } catch (...) {
        return false;
    }
}

bool NativeFileSystem::is_directory(const Path& path) const {
    try {
        return fs_impl::is_directory(toNativePath(path));
    } catch (...) {
        return false;
    }
}

uint64_t NativeFileSystem::file_size(const Path& path) const {
    try {
        return static_cast<uint64_t>(fs_impl::file_size(toNativePath(path)));
    } catch (...) {
        return 0;
    }
}

bool NativeFileSystem::remove(const Path& path) {
    try {
        return fs_impl::remove(toNativePath(path));
    } catch (...) {
        return false;
    }
}

bool NativeFileSystem::copy_file(const Path& from, const Path& to, bool overwrite) {
    try {
        auto options = overwrite ?
            fs_impl::copy_options::overwrite_existing :
            fs_impl::copy_options::none;
        fs_impl::copy_file(toNativePath(from), toNativePath(to), options);
        return true;
    } catch (...) {
        return false;
    }
}

bool NativeFileSystem::rename(const Path& from, const Path& to) {
    try {
        fs_impl::rename(toNativePath(from), toNativePath(to));
        return true;
    } catch (...) {
        return false;
    }
}

bool NativeFileSystem::create_directory(const Path& path) {
    try {
        return fs_impl::create_directory(toNativePath(path));
    } catch (...) {
        return false;
    }
}

bool NativeFileSystem::create_directories(const Path& path) {
    try {
        return fs_impl::create_directories(toNativePath(path));
    } catch (...) {
        return false;
    }
}

std::vector<Path> NativeFileSystem::list_directory(const Path& path) const {
    std::vector<Path> result;
    try {
        for (const auto& entry : fs_impl::directory_iterator(toNativePath(path))) {
            result.push_back(fromNativePath(entry.path()));
        }
    } catch (...) {
        // Return empty vector on error
    }
    return result;
}

std::vector<uint8_t> NativeFileSystem::read_file(const Path& path) const {
    std::ifstream file(path.string(), std::ios::binary);
    if (!file) {
        return std::vector<uint8_t>();
    }

    file.seekg(0, std::ios::end);
    size_t size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<uint8_t> data(size);
    file.read(reinterpret_cast<char*>(data.data()), size);

    return data;
}

bool NativeFileSystem::write_file(const Path& path, const std::vector<uint8_t>& data) {
    std::ofstream file(path.string(), std::ios::binary);
    if (!file) {
        return false;
    }

    file.write(reinterpret_cast<const char*>(data.data()), data.size());
    return file.good();
}

Path NativeFileSystem::current_path() const {
    try {
        return fromNativePath(fs_impl::current_path());
    } catch (...) {
        return Path();
    }
}

void NativeFileSystem::current_path(const Path& path) {
    try {
        fs_impl::current_path(toNativePath(path));
    } catch (...) {
        // Ignore errors
    }
}

Path NativeFileSystem::temp_directory_path() const {
    try {
        return fromNativePath(fs_impl::temp_directory_path());
    } catch (...) {
        return Path("/tmp");
    }
}

Path NativeFileSystem::show_open_dialog(const std::string& title, const std::string& filter) {
    // TODO: Platform-specific file dialog implementation
    // For now, this is a placeholder that would need to be implemented
    // using platform-specific APIs (Windows: GetOpenFileName, Mac: NSOpenPanel, Linux: GTK)
    return Path();
}

Path NativeFileSystem::show_save_dialog(const std::string& title, const std::string& default_name, const std::string& filter) {
    // TODO: Platform-specific file dialog implementation
    return Path();
}

// ========== Global Filesystem Instance ==========

std::unique_ptr<IFileSystem> g_filesystem;

std::unique_ptr<IFileSystem> createFileSystem() {
#ifdef USE_SFML
    return std::make_unique<NativeFileSystem>();
#else
    // Web implementation will be created in Phase 4
    return nullptr;
#endif
}
