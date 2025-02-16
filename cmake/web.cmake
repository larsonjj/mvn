# Disable shared builds as we want a static binary
set(BUILD_SHARED_LIBS OFF CACHE INTERNAL "")

# Set the executable name
add_executable(${EXECUTABLE_NAME})

target_sources(${EXECUTABLE_NAME} PRIVATE ${PROJECT_SOURCES})

add_custom_command(TARGET ${EXECUTABLE_NAME} PRE_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_SOURCE_DIR}/assets"
    "$<TARGET_FILE_DIR:${EXECUTABLE_NAME}>/assets"
)

# Add std C and C++ libraries
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static -static-libgcc")

set(CMAKE_EXECUTABLE_SUFFIX ".html")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s ASSERTIONS=1 -gsource-map -s ALLOW_MEMORY_GROWTH=1 -s MAXIMUM_MEMORY=1gb --preload-file assets/ --shell-file ../../src/minshell.html")
set_target_properties(${EXECUTABLE_NAME} PROPERTIES SUFFIX ".html")

# Store source assets path
set(SRC_ASSETS_PATH "${CMAKE_SOURCE_DIR}/assets")

if (CMAKE_BUILD_TYPE MATCHES "Debug")
  # Set the asset path macro to the absolute path on the dev machine and ensure SDL uses callbacks for main
  target_compile_definitions(${EXECUTABLE_NAME} PUBLIC ASSETS_PATH=\"${SRC_ASSETS_PATH}\")
else()
  # Set the asset path macro in release mode to a relative path that assumes the assets folder is in the same directory as the game executable and ensure SDL uses callbacks for main
  target_compile_definitions(${EXECUTABLE_NAME} PUBLIC ASSETS_PATH="./assets/")
  SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Os")
endif()
