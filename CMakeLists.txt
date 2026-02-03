cmake_minimum_required(VERSION 3.10)
project(BladesOfExile)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Detect if we're building for Emscripten
if(EMSCRIPTEN)
    message(STATUS "Building for WebAssembly with Emscripten")

    # Emscripten-specific flags
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s WASM=1")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s USE_ZLIB=1")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s ALLOW_MEMORY_GROWTH=1")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s EXPORTED_FUNCTIONS=['_main','_malloc']")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s EXPORTED_RUNTIME_METHODS=['ccall','cwrap']")

    # Preload resource files
    # set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --preload-file ${CMAKE_SOURCE_DIR}/rsrc@/data")

    # Enable debugging for now
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")

    # For initial testing, create a simple main wrapper
    add_executable(boe web/main_web.cpp)

    # Set output to build/web directory
    set_target_properties(boe PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
    )

    # Generate HTML file
    set(CMAKE_EXECUTABLE_SUFFIX ".html")

else()
    message(STATUS "Building for desktop - use SConstruct instead")
    message(WARNING "This CMakeLists.txt is primarily for web builds. Use SConstruct for desktop builds.")
endif()

# Include directories (common for both builds)
include_directories(
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/game
    ${CMAKE_SOURCE_DIR}/src/tools
    ${CMAKE_SOURCE_DIR}/src/gfx
    ${CMAKE_SOURCE_DIR}/src/fileio
    ${CMAKE_SOURCE_DIR}/src/dialogxml
    ${CMAKE_SOURCE_DIR}/deps/fmtlib/include
)
