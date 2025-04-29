# This toolchain file is used when building for WebAssembly with Emscripten

# Set compilers
set(CMAKE_C_COMPILER emcc)
set(CMAKE_CXX_COMPILER em++)

# Set system name
set(CMAKE_SYSTEM_NAME Emscripten)
set(CMAKE_SYSTEM_VERSION 1)

# Force GNU C standard for EM_ASM support
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu99")
set(CMAKE_C_EXTENSIONS ON)

# Don't look for programs in the build host directories
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

# Define the shell template file path here so it's available globally
set(EMSCRIPTEN_SHELL_TEMPLATE "${CMAKE_SOURCE_DIR}/cmake/templates/minshell.html" CACHE STRING "Path to HTML shell template")

# Default Emscripten flags for better optimization and WebGL support
# Note: -s flags should be in the linker flags, not compiler flags
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s WASM=1 -s ALLOW_MEMORY_GROWTH=1 -s USE_WEBGL2=1 --shell-file ${EMSCRIPTEN_SHELL_TEMPLATE} -s SINGLE_FILE=0 -s EXPORTED_RUNTIME_METHODS=[\"ccall\",\"cwrap\"]")
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -s WASM=1 -s ALLOW_MEMORY_GROWTH=1 -s USE_WEBGL2=1")

# Set preferred suffix for HTML output
set(CMAKE_EXECUTABLE_SUFFIX ".html")

# Get the absolute path to the shell template
set(SHELL_TEMPLATE "${CMAKE_CURRENT_SOURCE_DIR}/cmake/templates/minshell.html")

# Set Emscripten output file extension for the library
set_target_properties(mvn PROPERTIES SUFFIX ".js")

# Add Emscripten-specific link flags for the library
target_link_options(mvn PRIVATE
    "-sEXPORT_NAME=mvnLib"
    "-sMODULARIZE=1"
)

# Create link to compile_commands.json for clangd
if(CMAKE_EXPORT_COMPILE_COMMANDS)
    # Ensure the build directory exists at the root level
    file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/build)

    # Check if target already exists before creating it
    if(NOT TARGET emscripten_create_compile_commands_link)
        # Use symlink on Unix-like systems, copy on Windows
        if(WIN32)
            add_custom_target(
                emscripten_create_compile_commands_link ALL
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                        ${CMAKE_BINARY_DIR}/compile_commands.json
                        ${CMAKE_SOURCE_DIR}/build/compile_commands.json
                BYPRODUCTS ${CMAKE_SOURCE_DIR}/build/compile_commands.json
                COMMENT "Copying compile_commands.json to build directory"
            )
        else()
            add_custom_target(
                emscripten_create_compile_commands_link ALL
                COMMAND ${CMAKE_COMMAND} -E create_symlink
                        ${CMAKE_BINARY_DIR}/compile_commands.json
                        ${CMAKE_SOURCE_DIR}/build/compile_commands.json
                BYPRODUCTS ${CMAKE_SOURCE_DIR}/build/compile_commands.json
                COMMENT "Creating symlink to compile_commands.json in build directory"
            )
        endif()
    endif()
endif()
