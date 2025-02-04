# shadertest
An interactive GLSL vertex/fragment shader playground in C++17.

## Usage
At the moment, all shaders and options must be passed in as command-line arguments. Consult the help string (e.g. `shadertest --help`) for more information.

## Building
This version of the software successfully builds with debug flags on Ubuntu Linux 24.10 (GNU Make + GCC) and Windows 11 (Visual Studio + MSVC).

### Third-party libraries

#### GLAD 2
This project utilizes the GLAD extension loading library for OpenGL. You'll first need to generate a GLAD header file for inclusion in the project using [GLAD 2](https://gen.glad.sh).

The GLAD settings are as follows:
- **Generator**: C/C++
- **APIs**:
  - **gl**: Version 4.6 Core
- **Extensions**:
  - GL_ARB_debug_output
  - GL_ARB_vertex_array_object
- **Options**:
  - Header only

You can also use [the corresponding permalink](https://gen.glad.sh/#generator=c&api=gl%3D4.6&profile=gl%3Dcore%2Cgles1%3Dcommon&extensions=GL_ARB_debug_output%2CGL_ARB_vertex_array_object&options=HEADER_ONLY).

#### GLFW
This project uses the GLFW window and context management library. It can be linked dynamically or statically, though static linking may require you to build GLFW from source.

### Ubuntu Linux
1. Install/generate the following:
   - GCC
   - GNU Make
   - GLAD 2 header
   - GLFW 3.4
1. Copy the GLAD header file into the following (new) location:
   - `include/glad/gl.h`
1. If you built GLFW from source (e.g. for static linking), copy the header and library files into the following (new) locations:
   - `include/GLFW/glfw3.h`
   - `lib/libglfw3.a`
1. Set/`export` the following environment variables, as needed:
   - For static linking, set `STATIC_BUILD=1`.
   - For debug builds, set `DEBUG_BUILD=1`.
1. Compile using `make`.
1. Run the app using either `bin/release/shadertest` (or `bin/debug/shadertest` if `DEBUG_BUILD=1` was set).
1. Bundle the app using `make dist`.

### Windows (Visual Studio)
1. Install/generate the following:
   - GLAD 2 header
   - GLFW 3.4
1. Copy the GLAD and GLFW files into the following (new) locations:
   - `include/glad/gl.h`
   - `include/GLFW/glfw3.h`
   - For static linking (default):
     - `lib/glfw3.lib`
   - For dynamic linking:
     - `lib/glfw3dll.lib`
     - `lib/glfw3.dll`
1. For dynamic linking:
   1. Navigate to **Project** > **ShaderTest Properties**.
   1. Set the configuration to **All Configurations**.
   1. Under **Linker** > **Input** > **Additional Dependencies**, change `glfw3.lib` to `glfw3dll.lib`.
   1. Under **Build Events** > **Post-Build Event**:
      1. Click the dropdown and then **<Edit...>**.
      1. Add the line: `xcopy /y "$(SolutionDir)lib\glfw3.dll" "$(OutDir)"`.
      1. Click **OK** to close the Command Line dialog.
   1. Click **OK** to close the Properties Pages dialog.
1. Set the target configuration to either **Debug x64** or **Release x64**.
1. Build using **Build** > **Build Solution**.
