# Create a command to copy assets to the build directory
add_custom_target(copy_example_assets ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/examples/assets
            ${CMAKE_BINARY_DIR}/examples/assets
    COMMENT "Copying assets directory to build folder"
)

# Function to reduce redundancy for each example
function(mvn_add_example exe_name source_file)
    add_executable(${exe_name} ${source_file})
    target_link_libraries(${exe_name} PRIVATE mvn SDL3::SDL3 SDL3_image::SDL3_image SDL3_ttf::SDL3_ttf flecs)
    target_compile_definitions(${exe_name} PRIVATE ASSET_DIR="${CMAKE_BINARY_DIR}/examples/assets")
    set_target_properties(${exe_name} PROPERTIES
        C_STANDARD 99
        C_STANDARD_REQUIRED ON
        C_EXTENSIONS OFF
    )
    if(EMSCRIPTEN)
        target_link_options(${exe_name} PRIVATE
            "-sEXPORT_NAME=mvnGame"
            "-sASSERTIONS=1"
            "-sEXPORTED_RUNTIME_METHODS=FS"
            "--preload-file ${EXAMPLE_ASSETS_DIR}@/assets"
        )
    endif()
endfunction()

##### Examples #####
mvn_add_example(mvn_example_empty empty.c)
mvn_add_example(mvn_example_hello_world hello-world.c)
mvn_add_example(mvn_example_simple_texture simple-texture.c)
mvn_add_example(mvn_example_simple_text simple-text.c)
