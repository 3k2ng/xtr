# XToon Renderer

## Features

## Structure

## Building
Project is built using CMake. 
### VSCode
Having the [CMake Tools Extention](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cmake-tools) for VSCode would be cool. 
### Visual Studio
Project should work out of the box for Visual Studio.
### MacOS
Has support for system SDL2 and SDL2_image to reduce build time.
```
brew install sdl2
brew install sdl2_image
```
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

## Resources
- [SDL2 wiki](https://wiki.libsdl.org/SDL2/FrontPage)
- [docs.gl](https://docs.gl/)
