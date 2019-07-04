# Fractal Box
Fractal box is a 3D ray-marched fractal render engine (similar to [Marble Marcher](https://github.com/HackerPoet/MarbleMarcher) but without the game/physics bits) which will (hopefully) support VR eventually. 

I've currently implemented shaders which render the 2D Mandelbrot set, as well as a 3D sierpinski tetrahedron.

![Mandelbrot Set](/doc/screenshots/mandelbrot1.png)

![Sierpinski Trap](/doc/screenshots/sierpinski1.png)

## Download
To clone the project, `cd` to a directory where you want this thing, and then run 

```git clone --recurse-submodules https://github.com/Bouldersky/FractalBox.git```

to pull down everything you need

## Requirements
You need at least CMake version 3.10 and a compiler which is compatable with C++17 or better. CMake will let you know if either one needs to be updated.
## Compilation
After recursively cloning the project, run:
```
cd ./FractalBox
mkdir build
cd build
cmake ..
make
```
Once that finishes, you'll find youself in a build directory (located at `FractalBox/build`), and if everything went well there will be a `FractalBox` executable that you can run.