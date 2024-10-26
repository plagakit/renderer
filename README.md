# renderer

## Building
Make sure you have [CMake](https://cmake.org/). First, clone the repo and make a build folder:
```
git clone https://github.com/plagakit/renderer.git
cd renderer
mkdir bin
cd bin
```
For desktop, run:
```
cmake ..
cmake --build build
```
For web (make sure you have [Emscripten](https://emscripten.org/docs/getting_started/Tutorial.html) and the [Ninja build system](https://ninja-build.org/)):
```
cmake -S .. -B web -G Ninja -DPLATFORM=Web "-DCMAKE_TOOLCHAIN_FILE=<YOUR EMSCRIPTEN PATH>/emsdk/upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake"
cmake --build web
```
Alternatively, clone the repo and open it with Visual Studio.
