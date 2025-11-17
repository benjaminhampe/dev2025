# CMakeLists.txt for Spectrum 3D Terrain VST Plugin
# Author: MiniMax Agent
# Date: 2025-11-12

cmake_minimum_required(VERSION 3.12)
project(Spectrum3DTerrainVST VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Compiler-specific options
if(MSVC)
    set(CMAKE_CXX_FLAGS "/W4 /permissive- /Zc:__cplusplus")
    set(CMAKE_CXX_FLAGS_RELEASE "/O2 /DNDEBUG")
else()
    set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wpedantic")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")
endif()

# Platform detection
if(WIN32)
    set(VST_SUFFIX ".dll")
elseif(APPLE)
    set(VST_SUFFIX ".vst")
else()
    set(VST_SUFFIX ".so")
endif()

# =============================================================================
# Find Required Libraries
# =============================================================================

# Find PFFFT
find_path(PFFFT_INCLUDE_DIR
    NAMES pffft.h
    PATHS ${CMAKE_CURRENT_SOURCE_DIR}/external/pffft/include
          /usr/local/include
          /usr/include
    DOC "PFFFT include directory"
)

if(PFFFT_INCLUDE_DIR)
    message(STATUS "Found PFFFT include directory: ${PFFFT_INCLUDE_DIR}")
else()
    message(WARNING "PFFFT include directory not found. Please install PFFFT.")
endif()

# Find GLFW
find_package(PkgConfig REQUIRED)
pkg_check_modules(GLFW REQUIRED glfw3)

if(GLFW_FOUND)
    message(STATUS "Found GLFW: ${GLFW_VERSION}")
    include_directories(${GLFW_INCLUDE_DIRS})
    link_directories(${GLFW_LIBRARY_DIRS})
else()
    message(FATAL_ERROR "GLFW not found. Please install GLFW3.")
endif()

# Find GLEW
find_package(GLEW REQUIRED)
message(STATUS "Found GLEW: ${GLEW_VERSION}")

# Find GLM
find_package(glm REQUIRED)
message(STATUS "Found GLM: ${GLM_VERSION}")

# =============================================================================
# Source Files
# =============================================================================

set(VST_SOURCES
    Spectrum3DTerrainVST.cpp
    VSTMain.cpp
)

set(VST_HEADERS
    Spectrum3DTerrainVST.h
)

# =============================================================================
# Build VST Plugin
# =============================================================================

# Create VST plugin target
add_library(Spectrum3DTerrainVST SHARED ${VST_SOURCES} ${VST_HEADERS})

# Set VST-specific compiler definitions
target_compile_definitions(Spectrum3DTerrainVST PRIVATE
    WIN32_LEAN_AND_MEAN
    NOMINMAX
    _WINDOWS
    _USRDLL
    _CRT_SECURE_NO_WARNINGS
    VST_FORCE_DEPRECATED=0
)

# Platform-specific definitions
if(WIN32)
    target_compile_definitions(Spectrum3DTerrainVST PRIVATE WINDOWS_BUILD)
elseif(APPLE)
    target_compile_definitions(Spectrum3DTerrainVST PRIVATE MACOS_BUILD)
endif()

# Include directories
target_include_directories(Spectrum3DTerrainVST PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${PFFFT_INCLUDE_DIR}
    ${GLEW_INCLUDE_DIRS}
    ${GLFW_INCLUDE_DIRS}
    ${GLM_INCLUDE_DIRS}
)

# Link libraries
target_link_libraries(Spectrum3DTerrainVST PRIVATE
    ${GLFW_LIBRARIES}
    ${GLEW_LIBRARIES}
    ${GLM_LIBRARIES}
    opengl32
    glu32
)

# Platform-specific libraries
if(WIN32)
    target_link_libraries(Spectrum3DTerrainVST PRIVATE
        user32
        kernel32
        gdi32
        winspool
        shell32
        ole32
        oleaut32
        uuid
        comdlg32
        advapi32
    )
elseif(APPLE)
    target_link_libraries(Spectrum3DTerrainVST PRIVATE
        "-framework Cocoa"
        "-framework IOKit"
        "-framework CoreVideo"
        "-framework OpenGL"
    )
else()
    target_link_libraries(Spectrum3DTerrainVST PRIVATE
        pthread
        dl
        X11
        Xrandr
        Xi
        Xxf86vm
        Xinerama
        Xcursor
    )
endif()

# Set output name and directory
set_target_properties(Spectrum3DTerrainVST PROPERTIES
    OUTPUT_NAME "Spectrum3DTerrainVST${VST_SUFFIX}"
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/VST
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/VST
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/VST
)

# =============================================================================
# Build GLFW Test Application
# =============================================================================

# Create test application
add_executable(Spectrum3DTerrainTest
    TestApp.cpp
    VSTMain.cpp
)

target_include_directories(Spectrum3DTerrainTest PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${PFFFT_INCLUDE_DIR}
    ${GLFW_INCLUDE_DIRS}
    ${GLEW_INCLUDE_DIRS}
    ${GLM_INCLUDE_DIRS}
)

target_link_libraries(Spectrum3DTerrainTest PRIVATE
    Spectrum3DTerrainVST
    ${GLFW_LIBRARIES}
    ${GLEW_LIBRARIES}
    ${GLM_LIBRARIES}
    opengl32
    glu32
)

if(WIN32)
    target_link_libraries(Spectrum3DTerrainTest PRIVATE
        user32
        kernel32
        gdi32
    )
elseif(APPLE)
    target_link_libraries(Spectrum3DTerrainTest PRIVATE
        "-framework Cocoa"
        "-framework IOKit"
        "-framework CoreVideo"
        "-framework OpenGL"
    )
else()
    target_link_libraries(Spectrum3DTerrainTest PRIVATE
        pthread
        dl
        X11
        Xrandr
        Xi
        Xxf86vm
        Xinerama
        Xcursor
    )
endif()

# =============================================================================
# Installation
# =============================================================================

install(TARGETS Spectrum3DTerrainVST
    RUNTIME DESTINATION bin/VST
    LIBRARY DESTINATION lib/VST
    ARCHIVE DESTINATION lib/VST
)

install(FILES ${VST_HEADERS}
    DESTINATION include/Spectrum3DTerrainVST
)

# =============================================================================
# Dependencies Download (Optional)
# =============================================================================

# Download PFFFT if not found
if(NOT PFFFT_INCLUDE_DIR)
    message(STATUS "PFFFT not found. You can download it from: https://bitbucket.org/mrobbins/pffft")
endif()

# =============================================================================
# Build Configuration Examples
# =============================================================================

# Build with ASAN (for debugging):
# cmake -DCMAKE_CXX_FLAGS="-fsanitize=address" -DCMAKE_C_FLAGS="-fsanitize=address" ..

# Build in Release mode:
# cmake -DCMAKE_BUILD_TYPE=Release ..

# Build with specific compiler:
# cmake -DCMAKE_CXX_COMPILER=clang++ ..

# =============================================================================
# Documentation
# =============================================================================

# Create a simple Doxyfile (requires Doxygen to be installed)
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
"DOXYFILE_ENCODING      = UTF-8
PROJECT_NAME           = \"Spectrum 3D Terrain VST Plugin\"
PROJECT_NUMBER         = 1.0.0
OUTPUT_DIRECTORY       = ${CMAKE_CURRENT_BINARY_DIR}/docs
INPUT                  = ${CMAKE_CURRENT_SOURCE_DIR}
RECURSIVE              = YES
USE_MDFILE_AS_MAINPAGE = README.md
GENERATE_HTML          = YES
GENERATE_MAN           = YES
GENERATE_XML           = YES
")

message(STATUS "Build configuration complete!")
message(STATUS "VST plugin will be built as: Spectrum3DTerrainVST${VST_SUFFIX}")
message(STATUS "Test application: Spectrum3DTerrainTest")
message(STATUS "")
message(STATUS "To build:")
message(STATUS "  mkdir build && cd build")
message(STATUS "  cmake ..")
message(STATUS "  make")
message(STATUS "")
message(STATUS "Dependencies:")
message(STATUS "  - PFFFT: https://bitbucket.org/mrobbins/pffft")
message(STATUS "  - GLFW: sudo apt install libglfw3-dev (Linux)")
message(STATUS "  - GLEW: sudo apt install libglew-dev (Linux)")
message(STATUS "  - GLM: sudo apt install libglm-dev (Linux)")