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

##### TEST CONFIGURATION #####
# List of all test modules
set(MVN_TEST_MODULES
    core
    file
    string
    logger
    list
    hashmap
    texture
    text
)

# Build all test executables
set(MVN_TEST_TARGETS)
foreach(MODULE ${MVN_TEST_MODULES})
    set(TARGET_NAME mvn_test_${MODULE})
    list(APPEND MVN_TEST_TARGETS ${TARGET_NAME})
    mvn_add_test_executable(${TARGET_NAME} source/mvn-${MODULE}-test.c)
endforeach()

# Create a meta-target that depends on all test executables
add_custom_target(mvn_test_build_all DEPENDS ${MVN_TEST_TARGETS})

# Make this meta-target part of the default build
set_target_properties(mvn_test_build_all PROPERTIES EXCLUDE_FROM_ALL FALSE)

# Make sure the meta-target depends on the main mvn target
add_dependencies(mvn_test_build_all mvn)

##### TESTS #####
# Register all tests with CTest
foreach(TARGET ${MVN_TEST_TARGETS})
    add_test(NAME ${TARGET} COMMAND ${TARGET})
endforeach()
