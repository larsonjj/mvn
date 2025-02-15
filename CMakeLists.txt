####################################
# CMAKE Configuration
####################################

# Minimum version of CMake required to build this project
cmake_minimum_required(VERSION 3.22.1)

# Declare the project name, version, description, and languages used.
project(mvn
  VERSION 0.1.0
  DESCRIPTION "Maven Game Engine"
  HOMEPAGE_URL "https://github.com/larsonjj/mvn"
  LANGUAGES C
)

# Include CPack to package the project
if(NOT CPACK_INCLUDED)
    include(CPack)
endif()

# Add CPM.cmake to the project
# Add CPM dependency
include(cmake/CPM.cmake)

set(EXECUTABLE_NAME ${PROJECT_NAME})

# Set C standard to c99
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)


# Export compile commands for use with clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Ensure that the dynamic library goes into the build directory automatically.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")

# Disable installing to system directories.
set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}" CACHE INTERNAL "")

# Instructs CMake to organize targets and projects into folders when generating IDE project files (such as Visual Studio solutions or Xcode projects).
# This makes the project structure cleaner and easier to navigate in IDEs that support folder organization.
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Define PROJECT_SOURCES as a list of all source files
file(GLOB_RECURSE PROJECT_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_LIST_DIR}/src/*.c")

####################################
# CPack
####################################

# Setup CPack for packaging
set(CPACK_INSTALL_CMAKE_PROJECTS "${CMAKE_BINARY_DIR}" "${PROJECT_NAME}" "ALL" "/")

set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})
set(CPACK_PACKAGE_CONTACT ${AUTHOR_EMAIL})

####################################
# Platform Specific Configuration
####################################

### Android ###
if(CMAKE_SYSTEM_NAME MATCHES "Darwin")
  include(cmake/macos.cmake)
elseif(CMAKE_SYSTEM_NAME MATCHES "Emscripten")
  include(cmake/web.cmake)
elseif(CMAKE_SYSTEM_NAME MATCHES "Windows")
  include(cmake/windows.cmake)
else()
  message(FATAL_ERROR "Unsupported platform")
endif()

####################################
# Load Dependencies
####################################

# Add SDL
set(SDL_DIALOG OFF CACHE BOOL "" FORCE)
set(SDL_CAMERA OFF CACHE BOOL "" FORCE)
set(SDL_SENSOR OFF CACHE BOOL "" FORCE)
CPMAddPackage(
  NAME SDL
  GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
  GIT_TAG release-3.2.2
  EXCLUDE_FROM_ALL YES
)

# Add SDL Image
set(SDLIMAGE_INSTALL OFF) # disable install target for SDL Image
set(SDLIMAGE_AVIF OFF)
set(SDLIMAGE_BMP ON)
set(SDLIMAGE_GIF OFF)
set(SDLIMAGE_JPG OFF)
set(SDLIMAGE_JXL OFF)
set(SDLIMAGE_LBM OFF)
set(SDLIMAGE_PCX OFF)
set(SDLIMAGE_PNG ON)
set(SDLIMAGE_PNM OFF)
set(SDLIMAGE_QOI OFF)
set(SDLIMAGE_SVG ON)
set(SDLIMAGE_TGA OFF)
set(SDLIMAGE_TIF OFF)
set(SDLIMAGE_WEBP ON)
set(SDLIMAGE_XCF OFF)
set(SDLIMAGE_XPM OFF)
set(SDLIMAGE_XV OFF)
CPMAddPackage(
  NAME SDL_image
  GIT_REPOSITORY https://github.com/libsdl-org/SDL_image.git
  GIT_TAG release-3.2.0
  EXCLUDE_FROM_ALL YES
)

# Add SDL Mixer
set(SDLMIXER_FLAC OFF)
set(SDLMIXER_GME OFF)
set(SDLMIXER_MOD OFF)
set(SDLMIXER_MIDI OFF)
set(SDLMIXER_OPUS OFF)
CPMAddPackage(
  NAME SDL_mixer
  GIT_REPOSITORY https://github.com/libsdl-org/SDL_mixer.git
  GIT_TAG 4870186
  EXCLUDE_FROM_ALL YES
)

# Add SDL_TTF
set(SDLTTF_VENDORED ON CACHE BOOL "" FORCE)
CPMAddPackage(
  NAME SDL_ttf
  GIT_REPOSITORY https://github.com/libsdl-org/SDL_ttf.git
  GIT_TAG preview-3.1.0
  EXCLUDE_FROM_ALL YES
)

####################################
# Target Configuration
####################################

# Add error checking flags for C (after linking dependencies)
target_compile_options(${EXECUTABLE_NAME} PRIVATE
    $<$<COMPILE_LANGUAGE:C>:
        $<$<C_COMPILER_ID:MSVC>:/W4 /WX>
        $<$<C_COMPILER_ID:GNU>:-Wall -Wextra -Werror>
        $<$<C_COMPILER_ID:Clang>:-Wall -Wextra -Werror>
    >
)

# Ensure we use SDL3 main callbacks. (Must be done after linking dependencies)
target_compile_definitions(${EXECUTABLE_NAME} PUBLIC SDL_MAIN_USE_CALLBACKS=1)

# Link dependencies
target_link_libraries(${EXECUTABLE_NAME} PUBLIC SDL3::SDL3 SDL3_image::SDL3_image SDL3_mixer::SDL3_mixer SDL3_ttf::SDL3_ttf)

# Use only C flags for Debug builds
if (CMAKE_BUILD_TYPE MATCHES "Debug")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0 -DDEBUG")
endif()

