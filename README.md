# simple-molecule-renderer

Simple molecule renderer implementing toon shading.

## Preview

![Preview image](asset/preview_with_outline.png "Preview image")

## Dependencies

- OpenGL
- GLFW
- GLEW
- GLM

For MacOS users

```Bash
brew install glfw3
brew install glew
brew install glm
```

For Ubuntu users

```Bash
sudo apt-get install libglfw3-dev libglew-dev libglm-dev
```

## Build

```Bash
mkdir build/
cd build
cmake ..
make
cd ..
ln -s build/ToonShading .
./ToonShading
```

## Basic usages

- W/A/S/D/Q/E for moving camera translationally
- dragging with left mouse key for rotating your model.
- dragging with right mouse key for rotating around z-direction (the direction of your camera)
- ctrl+s for exporting image(4x current resolution, enough for publishing)

You could simply replacing the `.xyz` file path in `main.cpp` with your own `xyz` file path.

## Acknowledgements

Thanks for the graphical library: [stb](https://github.com/nothings/stb).

- `zukxov02_P1_H.xyz` from [CSD MOF Collection](https://www.ccdc.cam.ac.uk/free-products/csd-mof-collection/)
- `C60-Ih.xyz` from [The nanotube site](https://nanotube.msu.edu/fullerene/fullerene.php?C=60)
- `benzene.xyz` from [Pressure-frozen benzene I revisited](https://doi.org/10.1107/S010876810503747X)
- `ps.xyz` from [Materials Project](https://next-gen.materialsproject.org/materials/mp-612)
