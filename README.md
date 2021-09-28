# Rasterizer

## Introduction
Welcome to the repository of the Rasterizer/Makma3D project, which aims to build a graphics engine in Vulkan. Eventually, it should be extensive enough to do things like cool lighting, load models, apply physics, animations, the whole junk - but given that it's a hobby project, we'll see how far we get.

## Features
As time progresses, the Rasterizer will grow in features and in capabilities. For a complete overview of our roadmap, check the [issues](https://github.com/Lut99/Rasterizer/issues), but a short overview can be seen below as well.

Checked features are those already implemented, while unchecked features are planned.
- [x] Basic vertex-based rendering
- [x] Basic model loading
- [x] Moveable camera
- [x] Basic texture loading
- [x] Material support:
  - [x] Just taking vertex colours
  - [x] Giving it a static colour
  - [x] Loading textures (without lighting)
  - [ ] Basic diffuse lighting on vertex colours
  - [ ] Basic diffuse lighting on a static colour
  - [ ] Basic diffuse lighting on textures
- [ ] Asynchronous model/texture loading
- [ ] Development console for interactivity
- [ ] Physics for objects

## Compilation
Compilation of the project should be relatively straightforward. Download the repository source files somewhere, and, depending on your OS, create the directory structure for the binaries:
```
- source root/
 |- bin/
   |- shaders/
     |- materials/
```
On Windows with VSBuild, the folders should spawn on compilation - but on Ubuntu with Makefiles, it will complain if these aren't present.

Next, before we start, make sure that the two compile dependencies for the project are present: install [GLFW3](https://www.glfw.org/), setting its path in the main ```CMakeLists.txt``` file if you're on Windows (also see the 'Dependencies' section), and make sure that Google's [glslc](https://github.com/google/shaderc) is present in your terminal's path.

Create an out-of-source build folder for CMake where it will generate (most) of the resulting binaries. For example, create a ```build/``` directory in the source root and cd into it.

Setup the cmake build files by running:
```
cmake ../
```
Where ```../``` is the (relative) path to the source root. Alternatively, ```-DCMAKE_BUILD_TYPE=Debug``` can be supplied _before_ the path to compile in Debug mode, which enables extra checks in the rasterizer's code. 

When the generation is complete, build the project by running:
```
cmake --build . --target rasterizer
```
Optionally, append ```--parallel <no. of jobs>``` to build with multiple processes, which considerably speeds up the process and is recommended if you're building from a clean directory. Wait until compilation is finished, and then go back to the source root. The executable should be under:
```
bin/rasterizer
```
on unix or
```
bin/rasterizer.exe
```
on Windows.

Note that, to run, the rasterizer expects a couple of models and textures to be present. These can be downloaded under any of the [pre-compiled zips for Windows](https://github.com/Lut99/Rasterizer/releases), where the directory structure there must be present relative to the compiled executable (see the 'Running' section).

## Dependencies
Apart from standard dependencies such as CMake and a compiler, the Rasterizer only depends externally on the [GLFW3](https://www.glfw.org/) library. CMake should find your GLFW installation automatically if you installed it via your distribution's package manager (```libglfw3-dev``` on Ubuntu, and ```glfw``` in the Arch repositories). If you manually install GLFW on Windows, however, the CMake file has to manually contain the path where it should look for GLFW's CMake files; by default, this is set to ```C:/Program Files (x86)/GLFW/lib/cmake/glfw3```, but you should change this according to your setup.

For running the executable, no further dependencies are required as the GLFW3 library is compiled statically. The only thing it requires other than itself are the compiled ```.spv``` shader files and the models/textures. These can be found under any pre-compiled zip for Windows on [GitHub](https://github.com/Lut99/Rasterizer/releases), or are compiled with the rest of the source code if you build the executable yourself.

## Running
To run the compiled rasterizer executable, simply run it by double clicking or launching it from a terminal. There are some options available:
- ```-l <size>,--local <size>```: Determines the size of the GPU-local memory pool for the entire application. Can be given as a number followed by the unit. It supports the standard bytes (B) to terabytes (TB), and for each of those also the bit equivalent (```b``` for bits and ```Tb``` for terabits) and the 1024-equivalent (```KiB``` for kibibytes and ```TiB``` for tebibytes). Defaults to ```100MiB```.
- ```-v <size>,--visible <size>```: Determines the size of the CPU-visible but device-size memory pool for the entire application. Supports the same format for the size parameter as the ```--local``` argument. Defaults to ```100MiB```.

As mentioned before does the rasterizer expect certain files to be present in certain locations relative to its executable. The required structure is:
```
- rasterizer.exe
- data/
 |- materials/
   |- <required .mtl files>
 |- models/
   |- <required .obj files>
 |- textures/
   |- <required .png/.jpg files>
- shaders/
 |- materials/
   |- <required .spv shader files>
```
Again, a correctly structured instance of these folders can be found in the correct version's [pre-compiled zip file](https://github.com/Lut99/Rasterizer/releases).

## Contributing
Because this is a hobby project, we do not accept contributions to this fork of the source code. However, feel free to fork the code yourself and go nuts - it is a hobby project, after all. However, if you intend to use this code for commercial ends, please mention this repository somewhere.

## Contact
Do you have any questions about or issues with the project? Feel free to raise [an issue](https://github.com/Lut99/Rasterizer/issues)! Bug reports are always appreciated if you encounter them, and if you do, mark your issue with the ```bug```-tag.
