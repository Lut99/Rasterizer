# Rasterizer

## Introduction
Welcome to the repository of the Rasterizer/Makma3D project, which aims to build a graphics engine in Vulkan. Eventually, it should be extensive enough to do things like cool lighting, load models, apply physics, animations, the whole junk - but given that it's a hobby project, we'll see how far we get.

## Features
As time progresses, the Rasterizer will grow in features and in capabilities. For a complete overview of our roadmap, check the [issues](/issues), but a short overview can be seen below as well.

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

Next, create an out-of-source build folder for CMake - for example, create a ```build/``` directory in the source root and cd into it.

Setup the cmake build files by running:
```
cmake ../
```
Where ```../``` is the (relative) path to the source root. Alternatively, ```-DCMAKE_BUILD_TYPE=Debug``` can be supplied _before_ the path to compile in Debug mode, which enables extra checks. 

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

Note that, to run, the rasterizer expects a couple of models to be present. These can be downloaded under any of the [pre-compiled zips for Windows](/releases), where the directory structure there must be present relative to the compiled executable.

## Dependencies
TBD
