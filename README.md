# XToon Renderer

## Features
- Implemented X-Toon renderer following Barla et al
- Implemented outline using near silhouette and edge detection (Robert Cross and Sobel)
- Implemented halftone post-processing
- Combination of X-Toon and halftone dithering technique

## Cool screenshots!
![Suzanne](demo/suzanne.png?raw=true "Suzanne")
![Armadillo](demo/armadillo.png?raw=true "Armadillo")
![Augustus](demo/augustus.png?raw=true "Augustus")
![Venus](demo/venus.png?raw=true "Venus")

## Building
Project is built using CMake. 
### VSCode
Having the [CMake Tools Extention](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cmake-tools) for VSCode would be cool. 
### Visual Studio
Project should work out of the box for Visual Studio.
### Build in terminal
```
mkdir build
cd build
cmake ..
make
./xtr
```

## Dependencies
- SDL2
- SDL2_image
- GLAD
- Dear ImGui
- glm
- tinyobjloader
- miniply

## Resources
- [SDL2 wiki](https://wiki.libsdl.org/SDL2/FrontPage)
- [docs.gl](https://docs.gl/)
