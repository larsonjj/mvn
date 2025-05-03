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
rm -rf build/web

# Create a new build directory
mkdir -p build/web

# Method 1: Using CMake presets (requires EMSDK environment variable)
# Source Emscripten environment if needed
if [ -z "${EMSDK}" ]; then
    echo "EMSDK environment variable not set. Please source emsdk_env.sh first"
    echo "Example: source /path/to/emsdk/emsdk_env.sh"
    exit 1
fi

# Configure with emcmake - specify toolchain file explicitly
emcmake cmake -B build/web \
    -DCMAKE_BUILD_TYPE=Release \
    -DMVN_BUILD_EXAMPLES=ON \
    -DMVN_BUILD_TESTS=OFF \
    -DMVN_CI=ON

# Build the project
cmake --build build/web --config Release

# Run the file preparation script to check it works locally
mkdir -p build/web/dist
find build/web/examples -type f -name "mvn_example_*.html" -o -name "mvn_example_*.js" -o -name "mvn_example_*.wasm" -o -name "mvn_example_*.data" | xargs -I{} cp {} build/web/dist/ || echo "Some files may not exist"

# If there are any additional assets not preloaded, copy them
if [ -d "build/web/examples/assets" ]; then
mkdir -p build/web/dist/assets
cp -r build/web/examples/assets/* build/web/dist/assets/
fi

# Dynamically generate an index.html that lists all available examples based on HTML files
# Remove any existing index.html file
rm -f build/web/dist/index.html
# Create the index.html file
echo "<\!DOCTYPE html>" > build/web/dist/index.html
echo "<html><head><meta charset=\"utf-8\"><title>MVN Game Framework - Examples</title>" >> build/web/dist/index.html
echo "<style>body{font-family:Arial,sans-serif;max-width:800px;margin:0 auto;padding:20px;}h1{color:#333;}ul{list-style-type:none;padding:0;}li{margin:10px 0;}a{color:#0366d6;text-decoration:none;}a:hover{text-decoration:underline;}</style>" >> build/web/dist/index.html
echo "</head><body><h1>MVN Game Framework Examples</h1><ul>" >> build/web/dist/index.html

# Find all HTML example files and add them to the index
for file in $(find build/web/dist -name "mvn_example_*.html" | sort); do
  filename=$(basename "$file")
  name=$(echo "$filename" | sed 's/mvn_example_//g' | sed 's/\.html//g')
  display_name=$(echo "$name" | sed 's/_/ /g' | sed -e 's/\<./\u&/g')
  echo "<li><a href=\"$filename\">$display_name</a></li>" >> build/web/dist/index.html
done

echo "</ul></body></html>" >> build/web/dist/index.html

# Show success message and location of output files
echo "=========================="
echo "Build completed successfully!"
echo "Output files are in: $(pwd)/build/web/examples"
echo "HTML files: $(find build/web/examples -name '*.html')"
echo "=========================="
