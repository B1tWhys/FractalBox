# Fractal Box
Fractal box is a 3D ray-marched fractal render engine (similar to [Marble Marcher](https://github.com/HackerPoet/MarbleMarcher)) which will (hopefully) support VR eventually. Currently though it just renderes the mandelbrot set in 2D (womp womp).

## Download
To clone the project, `cd` to a directory where you want this thing, and then run 

```git clone --recurse-submodules```

to pull down everything you need

## Compilation
Make sure you've got make and CMake installed, then run:
```
cd ./FractalBox
mkdir build
cd build
cmake ..
make
```
Once that finishes, you'll find youself in a build directory (located at `/.../FractalBox/build`), and if everything went well there will be a FractalBox executable that you can run.