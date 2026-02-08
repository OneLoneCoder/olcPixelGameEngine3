# Building

## CMake

Configure with:
```bash
cmake -S . -B build
```

**Note**: If using emscripten prepend the configuration command with ``emcmake``

Build with:
```bash
cmake --build build
```
Header file should now be located at `build/olcPixelGameEngine3.h`.

### Available CMake Configuration Flags

* ``-DBUILD_EXAMPLES=ON`` - builds examples, will be located at `build/examples`
* ``-DBUILD_TEST=ON``     - builds the multi-header for rapid development and a better debugging experience.
* ``-DBUILD_WAYLAND=ON``  - if examples are built, build them using wayland. Linux ONLY!
* ``-DUSE_STB=ON``        - build using stb_image for the image loader (See section on STB Image)

## Manually on Linux

```bash
cd examples
```

### Using GCC

```bash
g++ -ggdb olcPGE3_ImageQuads.cpp -o olcPGE3_ImageQuads -std=c++20 -lpng -lGL -lX11 -lpthread
```

### Using Clang

```bash
clang++ -ggdb olcPGE3_ImageQuads.cpp -o olcPGE3_ImageQuads -std=c++20 -lpng -lGL -lX11 -lpthread
```

### Using Emscripten

```bash
emcc olcPGE3_ImageQuads.cpp -o olcPGE3_ImageQuads.html -sASYNCIFY -sALLOW_MEMORY_GROWTH=1 -sSTACK_SIZE=1048576 -sEXPORTED_RUNTIME_METHODS=HEAPF32 -sMAX_WEBGL_VERSION=2 -sMIN_WEBGL_VERSION=2 -sUSE_LIBPNG=1 -sLLD_REPORT_UNDEFINED --preload-file assets@assets
```

# Using STB Image

## Get the headers
Aquire [stb_image.h](https://github.com/nothings/stb/blob/master/stb_image.h) and [stb_image_write.h](https://github.com/nothings/stb/blob/master/stb_image_write.h). Place them in the root directory of the repo.

### CMake
Use the same configuration command you've chosen from above, and simply add ``-DUSE_STB=ON`` to the end of it. 

### GCC or Clang or EM++

Use the build commands you've chosen from above, and simply add ``-DOLC_USE_STB_IMAGE=1`` to the end of it.

