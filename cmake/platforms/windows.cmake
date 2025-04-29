set(CMAKE_C_COMPILER cl)
set(CMAKE_CXX_COMPILER cl)

# Set C99 standard
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# Create link to compile_commands.json for clangd
if(CMAKE_EXPORT_COMPILE_COMMANDS)
    # Ensure the build directory exists at the root level
    file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/build)

    # Only add the custom target if MVN_CI is NOT defined
    if(NOT DEFINED MVN_CI)
        add_custom_target(
            windows_create_compile_commands_link ALL
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    ${CMAKE_SOURCE_DIR}/windows/compile_commands.json
                    ${CMAKE_SOURCE_DIR}/build/compile_commands.json
            BYPRODUCTS ${CMAKE_SOURCE_DIR}/build/compile_commands.json
            COMMENT "Copying compile_commands.json to build directory"
        )
    endif()
endif()
