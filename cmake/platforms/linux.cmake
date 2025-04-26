set(CMAKE_C_COMPILER gcc)
set(CMAKE_CXX_COMPILER g++)

# Create link to compile_commands.json for clangd
if(CMAKE_EXPORT_COMPILE_COMMANDS)
    # Ensure the build directory exists at the root level
    file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/build)

    # Use symlink on Unix-like systems (Linux, macOS)
    add_custom_target(
        linux_create_compile_commands_link ALL
        COMMAND ${CMAKE_COMMAND} -E create_symlink
                ${CMAKE_BINARY_DIR}/compile_commands.json
                ${CMAKE_SOURCE_DIR}/build/compile_commands.json
        BYPRODUCTS ${CMAKE_SOURCE_DIR}/build/compile_commands.json
        COMMENT "Creating symlink to compile_commands.json in build directory"
    )
endif()
