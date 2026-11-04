# Technion Computer Graphic Skeleton

This project is what you'll use as a skeleton to build your homework assignments.
We'll use a **custom** modified version of [Raylib](www.raylib.com) as our platform abstraction layer.

## Building the program
Building for each platform should be as simple as running the provided `build-<platform>` script. Those scripts are made to be simple to read and are filled with comments explaining what flags are passed to the compiler.

### Linux - `build-linux.sh`
Dependencies: `gcc`, `GL`, `X11`

If you have a version of glibc under 2.38 (check with `ld --version`), you'd want to replace `lib/lib-linux/libraylib.a` with `lib/lib-linux/libraylib-glibc2.37.a`

### MacOS - `build-mac.sh`
Dependencies: `GL`

### Windows - `build-windows.bat`
Dependencies: [Visual Studio 2022 Build Tools](https://aka.ms/vs/17/release/vs_buildtools.exe) with clang-cl ("Individual components" -> "C++ Clang Compiler for Windows" in the installation menu)
Note that the Visual Studio installation location is hard-coded in `build-windows.bat`, so if you choose a non-default location for installation or want to use a different version of visual studio, make sure to change `VC_PATH` in the script, or run the script through "x64 Native Tools Command Prompt for VS".

## C-style and Single header approach
This skeleton is written in pure C, no interfaces, no classes, a very minimal approach. The idea is that you can understand it with basic Intro to C knowledge, and focus on the actual task - implementing and understanding Computer Graphics algorithms. You have all the tools you need to succeed provided by the given libraries and examples. However, as long as the project still compiles using the provided build scripts, you can do convert it to C++ and use other tools, although I'd strongly recoomend against it.

We use a single-header apporach for everything in the skeleton, it means `main.c` includes all of the other files directly, and there's only one compilation unit. This approach helps compilation speed and simplifies the whole build process to a single call to the compiler.

## Libraries
Some of the libraries contains modifications, make sure you use the versions provided in the project and not the original ones from the internet.

### raylib.h
Raylib is a very rich platform layer and graphics library, built on top of OpenGL. It will provide us access to cross-platform window creation, file IO, memory operations, and other utilities. Read the comments inside of the provided `raylib.h` to see how each function is defined.

### stb_ds.h
Sean Barrett's single-header dynamic array and hash table library, we'll use it for growing arrays. The interesting functions for us are `arrput` and `arrfree` - again, read the comments inside to understand how to call it.

### raygui.h and gui_window_file_dialog.h
The user interface uses `raygui` which is Raylib's GUI library, with a few additions to make your life a bit easier. It was chosen because it's very simple to use, so you can focus on the actual graphical algorithms. The initial example provided in `interface.h` should have all of the building blocks you need in order to make a sufficient GUI for our assignments.

### rlgl.h (for HW3)
A wrapper over opengl functions, mainly to add logging and simplify some reoccuring patterns.

## The Skeleton
 * `main.c` - The main program, opens a window, allocates resources like the frame buffer and calls to the gui, etc.
 * `math.h` - Linear algebra functionality, implements vectors and matrices.
 * `model.h` - defines `Model` struct and implements loading a model from an OBJ file.
 * `scene.h` - defines `Camera` and `Scene` structs, where `Scene` holds all of the information about the rendered scene.
 * `renderer.h` - defines `Renderer` and `RenderSettings` structs, and implements the rendering algorithms used the render the scene into a buffer.
 * `renderer-gl.h` (for HW3) - Implements hardware rendering.
 * `interface.h` - implements the GUI and the keyboard bindings for the program.
