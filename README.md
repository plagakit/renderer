# renderer

## Building
First, clone the repo and make a build folder:
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
For web:
```
cmake -S .. -B web -G Ninja -DPLATFORM=Web "-DCMAKE_TOOLCHAIN_FILE=<YOUR EMSCRIPTEN PATH>/emsdk/upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake"
cmake --build web
```
Alternatively, clone the repo and open it with Visual Studio.
