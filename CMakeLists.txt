cmake_minimum_required(VERSION 3.22.1)

if (CMAKE_SYSTEM_NAME MATCHES "Darwin")
  # Build universal binaries on Apple Silicon MacOS
  ## NOTE: This must be set before the project() call
  set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64" CACHE STRING "" FORCE)
endif()

# Project information
project(mvn
    VERSION 0.1.0
    DESCRIPTION "MVN game framework library"
    LANGUAGES C)

# Disable shared builds as we want a static binary
set(BUILD_SHARED_LIBS OFF CACHE INTERNAL "")

# Options
option(MVN_BUILD_EXAMPLES "Build MVN examples" ON)
option(MVN_BUILD_TESTS "Build MVN tests" ON)
option(MVN_WARNINGS_AS_ERRORS "Treat compiler warnings as errors" ON)

# Suppress developer warnings
set(CMAKE_SUPPRESS_DEVELOPER_WARNINGS 1 CACHE BOOL "Suppress developer warnings" FORCE)

# Disable installing to system directories.
set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}" CACHE INTERNAL "")

# Enable compilation database for tools like clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Define library sources
set(MVN_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/source/mvn.c
    ${CMAKE_CURRENT_SOURCE_DIR}/source/mvn-core.c
    ${CMAKE_CURRENT_SOURCE_DIR}/source/mvn-texture.c
    ${CMAKE_CURRENT_SOURCE_DIR}/source/mvn-logger.c
    ${CMAKE_CURRENT_SOURCE_DIR}/source/mvn-text.c
    ${CMAKE_CURRENT_SOURCE_DIR}/source/mvn-utils.c
    ${CMAKE_CURRENT_SOURCE_DIR}/source/mvn-file.c
    ${CMAKE_CURRENT_SOURCE_DIR}/source/mvn-list.c
    ${CMAKE_CURRENT_SOURCE_DIR}/source/mvn-string.c
    ${CMAKE_CURRENT_SOURCE_DIR}/source/mvn-hashmap.c
    ${CMAKE_CURRENT_SOURCE_DIR}/source/mvn-error.c
    # Add other source files here as they are created
)

# Define library headers
set(MVN_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/include/mvn/mvn.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/mvn/mvn-core.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/mvn/mvn-texture.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/mvn/mvn-utils.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/mvn/mvn-text.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/mvn/mvn-file.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/mvn/mvn-types.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/mvn/mvn-list.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/mvn/mvn-string.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/mvn/mvn-hashmap.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/mvn/mvn-error.h
    # Add other header files here as they are created
)

# Create library target
add_library(mvn ${MVN_SOURCES})
add_library(mvn::mvn ALIAS mvn)

# Set include directories
target_include_directories(mvn
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/source
)

# Set compiler warnings
if(MSVC)
    target_compile_options(mvn PRIVATE
        /W4
        /wd4312  # Disable warning C4312 (conversion to larger size)
        $<$<BOOL:${MVN_WARNINGS_AS_ERRORS}>:/WX>)

    if(NOT CMAKE_GENERATOR STREQUAL "Ninja")
        target_compile_options(mvn PRIVATE /MP)
    endif()
else()
    target_compile_options(mvn PRIVATE
        -Wall -Wextra -Wpedantic
        $<$<BOOL:${MVN_WARNINGS_AS_ERRORS}>:-Werror>)
endif()

# Fetch Dependencies

# SDL
set(SDL_TEST OFF CACHE BOOL "" FORCE)
set(SDL_INSTALL ON CACHE BOOL "" FORCE) # Make sure installation targets are generated
set(SDL_WINDOWS_GAMING_INPUT OFF CACHE BOOL "" FORCE) # Disable Windows Gaming Input API (requires Windows 10 SDK)

include(FetchContent)
FetchContent_Declare(
    SDL3
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG release-3.2.10
)
# Pull and build SDL3 completely first
FetchContent_MakeAvailable(SDL3)

# SDL_image
set(SDLIMAGE_INSTALL OFF CACHE BOOL "" FORCE)  # Don't install SDL3_image
set(SDLIMAGE_JPG ON CACHE BOOL "" FORCE)
set(SDLIMAGE_PNG ON CACHE BOOL "" FORCE)
set(SDLIMAGE_WEBP OFF CACHE BOOL "" FORCE) # Enable if needed
set(SDLIMAGE_JXL OFF CACHE BOOL "" FORCE)
set(SDLIMAGE_AVIF OFF CACHE BOOL "" FORCE)
set(SDLIMAGE_TIF OFF CACHE BOOL "" FORCE) # Enable if needed
FetchContent_Declare(
    SDL3_image
    GIT_REPOSITORY https://github.com/libsdl-org/SDL_image.git
    GIT_TAG release-3.2.4
)
# Now make SDL3_image available after SDL3 is available and verified
FetchContent_MakeAvailable(SDL3_image)

# SDL_ttf
set(SDLTTF_INSTALL OFF CACHE BOOL "" FORCE)  # Don't install SDL3_ttf
set(SDLTTF_VENDORED ON CACHE BOOL "" FORCE)
FetchContent_Declare(
    SDL3_ttf
    GIT_REPOSITORY https://github.com/libsdl-org/SDL_ttf.git
    GIT_TAG release-3.2.2
)
# Now make SDL3_ttf available after SDL3 is available and verified
FetchContent_MakeAvailable(SDL3_ttf)

# Export all symbols to help SDL3_image find them
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

# Platform-specific configuration
if(WIN32)
    # Windows-specific settings
    message(STATUS "Configuring for Windows")

    # Include cmake Windows platform cmake file
    include(cmake/platforms/windows.cmake)

    # Add Windows-specific libraries
    target_link_libraries(mvn PRIVATE winmm)

    # Windows-specific compile definitions
    target_compile_definitions(mvn PRIVATE WIN32_LEAN_AND_MEAN NOMINMAX)

elseif(APPLE)
    # macOS-specific settings
    message(STATUS "Configuring for macOS")

    # Include cmake MacOS platform cmake file
    include(cmake/platforms/macos.cmake)

elseif(EMSCRIPTEN)
    # Emscripten-specific settings
    message(STATUS "Configuring for Emscripten WebAssembly")

    # Include cmake Emscripten platform cmake file
    include(cmake/platforms/emscripten.cmake)

    # Define the function but don't apply it automatically
    function(configure_emscripten_executable target)
        set_target_properties(${target} PROPERTIES SUFFIX ".html")
        target_link_options(${target} PRIVATE
            "--shell-file=${SHELL_TEMPLATE}"
            "-sWASM=1"
            "-sALLOW_MEMORY_GROWTH=1"
            "-sUSE_WEBGL2=1"
            "-sSINGLE_FILE=0"
            "-sEXPORTED_RUNTIME_METHODS=[\"ccall\",\"cwrap\"]"
            "-sASSERTIONS=1"
        )
    endfunction()

elseif(UNIX)
    # Linux-specific settings
    message(STATUS "Configuring for Linux")

    # Include cmake Linux platform cmake file
    include(cmake/platforms/linux.cmake)

    # Add Linux-specific libraries
    target_link_libraries(mvn PRIVATE m dl)

    # Linux-specific compile options
    target_compile_options(mvn PRIVATE -fPIC)

endif()

# Link against SDL3 and SDL3_image for build-time usage
target_link_libraries(mvn
  PUBLIC
    $<BUILD_INTERFACE:SDL3::SDL3>
    $<BUILD_INTERFACE:SDL3_image::SDL3_image>
    $<BUILD_INTERFACE:SDL3_ttf::SDL3_ttf>
    # For install-time usage - handled via find_dependency in config file
    $<INSTALL_INTERFACE:>
)

# Save the original link libraries before export
get_target_property(ORIGINAL_LINK_LIBS mvn INTERFACE_LINK_LIBRARIES)

# When exporting the target, remove the SDL3 dependency
set_property(TARGET mvn PROPERTY INTERFACE_LINK_LIBRARIES "")

# Installation rules
install(TARGETS mvn
    EXPORT mvnTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

# Export targets
install(EXPORT mvnTargets
    FILE mvnTargets.cmake
    NAMESPACE mvn::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/mvn
)

# Restore the SDL3 dependency after exporting
set_property(TARGET mvn PROPERTY INTERFACE_LINK_LIBRARIES "${ORIGINAL_LINK_LIBS}")

# Create config file
include(CMakePackageConfigHelpers)
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/mvnConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/mvnConfig.cmake"
    @ONLY
)
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/mvnConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/mvnConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/mvnConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/mvn
)

# Process subdirectories if enabled
if(MVN_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)

    # Apply Emscripten settings to test executables
    if(EMSCRIPTEN AND TARGET mvn_test)
        configure_emscripten_executable(mvn_test)
    endif()
endif()

if(MVN_BUILD_EXAMPLES)
    add_subdirectory(examples)

    # Apply Emscripten settings to example executables
    if(EMSCRIPTEN)
        # Get all targets in the examples directory
        get_property(example_targets DIRECTORY examples PROPERTY BUILDSYSTEM_TARGETS)
        foreach(target ${example_targets})
            get_target_property(target_type ${target} TYPE)
            if(target_type STREQUAL "EXECUTABLE")
                configure_emscripten_executable(${target})
            endif()
        endforeach()
    endif()
endif()
