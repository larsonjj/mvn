##### ALL TESTS #####

# Create a command to copy assets to the build directory
add_custom_target(copy_test_assets ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/tests/assets
            ${CMAKE_BINARY_DIR}/tests/assets
    COMMENT "Copying assets directory to build folder"
)

# Test executable
add_executable(mvn_test_all
    source/mvn-test.c
    source/mvn-core-test.c
    source/mvn-texture-test.c
    source/mvn-hashmap-test.c
    source/mvn-list-test.c
    source/mvn-string-test.c
    source/mvn-logger-test.c
    source/mvn-file-test.c
    source/mvn-text-test.c
)

# Define asset directory for tests
target_compile_definitions(mvn_test_all PRIVATE
    ASSET_DIR="${CMAKE_BINARY_DIR}/tests/assets")

# Include directories for test
target_include_directories(mvn_test_all
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Link with MVN library
target_link_libraries(mvn_test_all PRIVATE mvn)

# Set C99 standard
set_target_properties(mvn_test_all PROPERTIES
    C_STANDARD 99
    C_STANDARD_REQUIRED ON
    C_EXTENSIONS OFF
)

##### TEST SUITE MACRO #####
function(mvn_add_test_executable target source main_macro)
    add_executable(${target} ${source})
    target_compile_definitions(${target} PRIVATE
        ASSET_DIR="${CMAKE_BINARY_DIR}/tests/assets"
        ${main_macro}
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
mvn_add_test_executable(mvn_test_core source/mvn-core-test.c MVN_CORE_TEST_MAIN)
mvn_add_test_executable(mvn_test_file source/mvn-file-test.c MVN_FILE_TEST_MAIN)
mvn_add_test_executable(mvn_test_string source/mvn-string-test.c MVN_STRING_TEST_MAIN)
mvn_add_test_executable(mvn_test_logger source/mvn-logger-test.c MVN_LOGGER_TEST_MAIN)
mvn_add_test_executable(mvn_test_list source/mvn-list-test.c MVN_LIST_TEST_MAIN)
mvn_add_test_executable(mvn_test_hashmap source/mvn-hashmap-test.c MVN_HASHMAP_TEST_MAIN)
mvn_add_test_executable(mvn_test_textures source/mvn-texture-test.c MVN_TEXTURES_TEST_MAIN)
mvn_add_test_executable(mvn_test_text source/mvn-text-test.c MVN_TEXT_TEST_MAIN)

##### TESTS #####
add_test(NAME mvn_test_all COMMAND mvn_test_all)
add_test(NAME mvn_test_core COMMAND mvn_test_core)
add_test(NAME mvn_test_file COMMAND mvn_test_file)
add_test(NAME mvn_test_string COMMAND mvn_test_string)
add_test(NAME mvn_test_logger COMMAND mvn_test_logger)
add_test(NAME mvn_test_list COMMAND mvn_test_list)
add_test(NAME mvn_test_hashmap COMMAND mvn_test_hashmap)
add_test(NAME mvn_test_textures COMMAND mvn_test_textures)
add_test(NAME mvn_test_text COMMAND mvn_test_text)
