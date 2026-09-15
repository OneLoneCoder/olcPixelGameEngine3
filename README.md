<p align="center">
  <a href="https://discord.gg/WhwHUMV"> <img src="https://img.shields.io/discord/380484403458998276?logo=discord"> </a>
</p>

# Pre-Launch BETA Testing
`olcPixelGameEngine v3.00 Beta B` is currently in a Beta testing phase, and has been made available to those that may want to experiment during olc::CodeJam2026. As such, this repo does not represent the final form of olcPixelGameEngine3, and presumably feedback from the Jam may change a few things prior to the official launch later this year.

# olcPixelGameEngine 3 (olcPGE3)
This is the official distribution of olcPixelGameEngine3, the successor to the popular [olcPixelGameEngine2](https://github.com/OneLoneCoder/olcPixelGameEngine/tree/master).

Purposes
 * Game Development
 * Tool Development
 * Visualisation
 * Real-Time Interactive Applications
 * Rapid Idea Prototyping
 * Kiosks and Browsers
 * Cross Platform Development
 * Cohesive User Experience
 * Algorithm Study
 * Classroom Exercises
 * Having Fun!

Easy To Use
 * Single Header File (as always)
 * CMAKE optional but provided
 * Entire project source for the curious

Capabilities
 * 2D Rendering
 * 3D Rendering
 * Batch Rendering
 * Custom GPU Shaders
 * Vector / Matrix Types
 * Polygon Construction
 * 2D Animation Package
 * 2D Camera Control
 * 2D Geometry Analysis & Interrogation
 * 2D Quad-Tree Data Structure
 * Flexible Game Creation

Tested On Platforms
 * Windows
 * Linux X11/Wayland
 * MacOS
 * Emscripten
 * Android

Input Features
 * Regional Keyboard
 * Mouse
 * Touch
 * Stylus / Pen

Extendable
 * Window Hooks
 * System Hooks


# Err.. I have questions!
* Is it compatible with olcPGE2? I've done loads of stuff in that.
  * No. PGE3 has built upon the years of development and community contributions and has outgrown the interfaces that olcPGE2 provided. They are however very similar, so if you have familiarity with PGE2, you should easily get going with PGE3.
* Are there ports to Rust, C# or Java? 
  * No. Over the past years PGE2 was ported to other environments only by fantatsic community contributors. The official release of PGE3 targets C++20.
* Have you used AI to make this?
  * No. Well, not knowingly or intentionally. This project has been developed for fun by enthusiasts who enjoy coding as a rewarding challenge. That said, no guarantees can be made that an IDE hasn't presented a useful auto-complete, or a contributer has used AI in pursuit of understanding a bug. The philosophy of OneLoneCoder is coding for fun, not prompt engineering.
* Can I use olcPixelGameEngine3 commercially?
  * Yes. The licence below simply requires that your end users know they are using a product that uses OneLoneCoder's olcPixelGameEngine.
* Can I contribute?
  * Yes. A primary goal of PGE3 was to be more open as a project. However, anonymous and/or AI created PRs will be ignored until you have introduced yourself and your ideas to the development team on our Discord server. 


# Licence (OLC-3)
Copyright 2018 - 2026 OneLoneCoder.com

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions 
are met:

1. Redistributions or derivations of source code must retain the above 
   copyright notice, this list of conditions and the following disclaimer.

2. Redistributions or derivative works in binary form must reproduce 
   the above copyright notice. This list of conditions and the following 
   disclaimer must be reproduced in the documentation and/or other 
   materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its 
   contributors may be used to endorse or promote products derived 
   from this software without specific prior written permission.
    
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR 
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, 
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY 
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

# Examples
As part of the development and testing process we have many examples that illustrate individual features. You can find interactive examples hosted here.

# Building

By far the easiest way to use olcPixelGameEngine3 is to simply include the header file provided in the root of this repository - `olcPixelGameEngine3.h`. As much as possible it will self configure for your chosen environment. On Microsoft platforms, using Visual Studio is our recommended way forward. For all other platforms, a CMake instruction is available.

## CMake

Configure with:
```bash
cmake -S dev -B build
```

**Note**: If using emscripten prepend the configuration command with ``emcmake``

Build with:
```bash
cmake --build build
```
Header file should now be located at `build/olcPixelGameEngine3.h`.

### Available CMake Configuration Flags

* ``-DBUILD_EXAMPLES=ON`` - builds examples, will be located at `build/examples`
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
Image loading is complex. For all the supported platforms we have ensured that as a minimum you can load a ".png" file as an asset. For a wider variety of formats its much better to use existing libraries, and we have decided to support "stb_image.h" which is pretty much the go-to image loader in C++. olcPGE3 can be instructed to use "stb_image" by declaring `OLC_USE_STB_IMAGE=1` before including the header file, or better yet, as a compiler predefined directive.

## Get the headers
Aquire [stb_image.h](https://github.com/nothings/stb/blob/master/stb_image.h) and [stb_image_write.h](https://github.com/nothings/stb/blob/master/stb_image_write.h). Place them in the root directory of the repo.

### CMake
Use the same configuration command you've chosen from above, and simply add ``-DUSE_STB=ON`` to the end of it. 

### GCC or Clang or EM++

Use the build commands you've chosen from above, and simply add ``-DOLC_USE_STB_IMAGE=1`` to the end of it.

