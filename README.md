# Tetris AI Environment
## Prerequisites
* CMake
* Pybind11 (preferably installed in a conda environment)
* SFML > 2.0

## Building Python libraries
```
mkdir build
cd build
cmake .. -DBUILD_PYTHON=ON
make
```
Afterwards, the Python library files will be present in the build folder. Make sure to point your import statements to this folder.
## Building normal executable
```
mkdir build
cd build
cmake .. -DBUILD_PYTHON=OFF
make
./tetris
```
