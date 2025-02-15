#!/bin/bash
cd "${0%/*}"
cd ..

mkdir -p build
mkdir -p build/web

cd build/web
emcmake cmake ../..
cmake --build ./ --config Release --parallel
