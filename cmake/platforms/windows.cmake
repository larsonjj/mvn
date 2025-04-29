set(CMAKE_C_COMPILER cl)
set(CMAKE_CXX_COMPILER cl)

# Create link to compile_commands.json for clangd
if(CMAKE_EXPORT_COMPILE_COMMANDS)
    # Ensure the build directory exists at the root level
    file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/build)
endif()
