#!/bin/bash
# filepath: /Users/validmac/Projects/c_cpp/game/build_web_examples.sh

# Exit on error
set -e

# Display commands being executed
set -x

# Source Emscripten environment if needed
# Uncomment and modify the path to your emsdk installation
# source /path/to/emsdk/emsdk_env.sh

# Clean any previous builds
rm -rf build/emscripten

# Create a new build directory
mkdir -p build/emscripten

# Method 1: Using CMake presets (requires EMSDK environment variable)
# Source Emscripten environment if needed
if [ -z "${EMSDK}" ]; then
    echo "EMSDK environment variable not set. Please source emsdk_env.sh first"
    echo "Example: source /path/to/emsdk/emsdk_env.sh"
    exit 1
fi

# Configure with emcmake - specify toolchain file explicitly
emcmake cmake -B build/emscripten -S .

# Build the project
emmake make -C build/emscripten

# Show success message and location of output files
echo "=========================="
echo "Build completed successfully!"
echo "Output files are in: $(pwd)/build/emscripten/examples"
echo "HTML files: $(find build/emscripten/examples -name '*.html')"
echo "=========================="
