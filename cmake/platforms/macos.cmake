# Find compilers dynamically
find_program(CLANG_COMPILER clang)
find_program(CLANGPP_COMPILER clang++)

# Set C99 standard
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# Use the found compilers or fall back to defaults
if(CLANG_COMPILER)
    set(CMAKE_C_COMPILER "${CLANG_COMPILER}")
else()
    message(STATUS "Using default C compiler")
endif()

if(CLANGPP_COMPILER)
    set(CMAKE_CXX_COMPILER "${CLANGPP_COMPILER}")
else()
    message(STATUS "Using default C++ compiler")
endif()

# Add macOS-specific frameworks
find_library(CORE_FOUNDATION_FRAMEWORK CoreFoundation)
target_link_libraries(mvn PRIVATE ${CORE_FOUNDATION_FRAMEWORK})

# Set macOS deployment target if desired
set(CMAKE_OSX_DEPLOYMENT_TARGET "10.15" CACHE STRING "Minimum macOS version")

# Create link to compile_commands.json for clangd
if(CMAKE_EXPORT_COMPILE_COMMANDS)
    # Ensure the build directory exists at the root level
    file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/build)

    # Use symlink on Unix-like systems (Linux, macOS)
    add_custom_target(
        macos_create_compile_commands_link ALL
        COMMAND ${CMAKE_COMMAND} -E create_symlink
                ${CMAKE_BINARY_DIR}/compile_commands.json
                ${CMAKE_SOURCE_DIR}/build/compile_commands.json
        BYPRODUCTS ${CMAKE_SOURCE_DIR}/build/compile_commands.json
        COMMENT "Creating symlink to compile_commands.json in build directory"
    )
endif()
