##### DEFINES #####
if(MVN_CI)
    add_compile_definitions(MVN_TEST_CI)
endif()

##### COPY ASSETS COMMAND #####
# Create a command to copy assets to the build directory
add_custom_target(copy_test_assets ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/tests/assets
            ${CMAKE_BINARY_DIR}/tests/assets
    COMMENT "Copying assets directory to build folder"
)

##### TEST SUITE MACRO #####
function(mvn_add_test_executable target source)
    add_executable(${target} ${source})
    add_dependencies(${target} copy_test_assets)
    target_compile_definitions(${target} PRIVATE
        ASSET_DIR="${CMAKE_BINARY_DIR}/tests/assets"
    )
    target_include_directories(${target} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    )
    target_link_libraries(${target} PRIVATE mvn)
    set_target_properties(${target} PROPERTIES
        C_STANDARD 99
        C_STANDARD_REQUIRED ON
        C_EXTENSIONS OFF
    )
endfunction()

##### INDIVIDUAL TESTS CONFIG #####
mvn_add_test_executable(mvn_test_core source/mvn-core-test.c)
mvn_add_test_executable(mvn_test_file source/mvn-file-test.c)
mvn_add_test_executable(mvn_test_string source/mvn-string-test.c)
mvn_add_test_executable(mvn_test_logger source/mvn-logger-test.c)
mvn_add_test_executable(mvn_test_list source/mvn-list-test.c)
mvn_add_test_executable(mvn_test_hashmap source/mvn-hashmap-test.c)
mvn_add_test_executable(mvn_test_textures source/mvn-texture-test.c)
mvn_add_test_executable(mvn_test_text source/mvn-text-test.c)

# Create a meta-target that depends on all test executables
add_custom_target(mvn_test_build_all DEPENDS
    mvn_test_core
    mvn_test_file
    mvn_test_string
    mvn_test_logger
    mvn_test_list
    mvn_test_hashmap
    mvn_test_textures
    mvn_test_text
)

# Make this meta-target part of the default build
set_target_properties(mvn_test_build_all PROPERTIES EXCLUDE_FROM_ALL FALSE)

# Make sure the meta-target depends on the main mvn target
add_dependencies(mvn_test_build_all mvn)

##### TESTS #####
# add_test(NAME mvn_test_all COMMAND mvn_test_all)
add_test(NAME mvn_test_core COMMAND mvn_test_core)
add_test(NAME mvn_test_file COMMAND mvn_test_file)
add_test(NAME mvn_test_string COMMAND mvn_test_string)
add_test(NAME mvn_test_logger COMMAND mvn_test_logger)
add_test(NAME mvn_test_list COMMAND mvn_test_list)
add_test(NAME mvn_test_hashmap COMMAND mvn_test_hashmap)
add_test(NAME mvn_test_textures COMMAND mvn_test_textures)
add_test(NAME mvn_test_text COMMAND mvn_test_text)
