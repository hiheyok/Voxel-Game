# Voxel Game

A Minecraft-style voxel game engine written from scratch in **C++20** and **OpenGL 4.5+**, built around performance, multithreading, and data-driven design.

The game runs a client-server model even in single player: an integrated server runs on its own thread, so single-player and multiplayer share the same logic.

## Features

- **Client-server architecture.** Separate `Client` and `Server` layers that exchange packets, with an integrated server for single player.
- **Entity Component System.** The server and client each have an ECS (`ServerECSManager` / `ClientECSManager`) where entities are IDs, components are plain data, and systems hold the logic.
- **Multithreaded lighting.** Separate sky-light and block-light engines run on a threaded light engine (`ThreadedLightEngine`) with a bucket-queue propagation pass.
- **Chunk meshing and rendering.**
  - Chunk meshes sample neighbouring corner and edge blocks for ambient occlusion.
  - Terrain is drawn in batches backed by a GPU memory pool (`ChunkBatch`, `MemoryPool`) using multi-draw indirect to keep CPU overhead low.
  - Frustum culling.
- **Terrain generation.** Pluggable generators (Mountains, Superflat, and several debug and mesh-stress generators) built on noise maps, a Java-compatible RNG, and structure placement.
- **Data-driven assets.** Blocks, models, textures, entity shapes, GUI layouts, and shaders load from JSON and GLSL files under `build/assets`, in a resource-pack layout (`assets/minecraft/...`).
- **Rendering abstraction.** A render-resource layer (`RenderEngine/RenderResources`) sits on top of the OpenGL backend (`RenderEngine/OpenGL`).

## Repository layout

```
Voxel-Game/
├── Minecraft Clone.sln            Visual Studio solution
└── Minecraft Clone/
    ├── CMakeLists.txt             CMake build
    ├── Minecraft Clone.vcxproj    Visual Studio project
    ├── include/                   Bundled headers: GLFW, GLEW, GLM, nlohmann/json, stb
    ├── bin/lib/x64/               Prebuilt static libs: GLEW, GLFW
    ├── build/assets/              Game assets: textures, models, shaders, JSON
    └── src/
        ├── main.cpp
        ├── Assets/                Asset loading and management
        ├── Client/                Client: input, player, rendering glue, UI, networking
        ├── Server/                Integrated server and packet handling
        ├── Core/                  Data structures, registries, IO, networking, options
        ├── Level/                 World, chunks, blocks, entities/ECS, lighting, terrain gen
        ├── RenderEngine/          Camera, chunk/entity/item/UI rendering, OpenGL backend
        ├── FileManager/
        └── Utils/
```

## Building

The project currently targets **Windows x64**. Prebuilt GLEW and GLFW libraries are in `Minecraft Clone/bin/lib/x64`, and all other dependencies are header-only and bundled in `include/`.

**Requirements**

- A GPU and driver that support OpenGL 4.5 or newer
- A CPU with AVX2 support (Release builds use `-mavx2`)
- One of: Visual Studio 2022 (C++20), VS Code with the CMake Tools extension, or CMake 3.29+ on its own. The last two need a MinGW-w64 GCC toolchain.

### Option 1: Visual Studio

1. Open `Minecraft Clone.sln`.
2. Select the `x64` platform and the `Release` configuration.
3. Build and run (`Ctrl+F5`).

### Option 2: CMake + MinGW

```bash
git clone https://github.com/hiheyok/Voxel-Game.git
cd "Voxel-Game/Minecraft Clone"

cmake -S . -B out -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release
cmake --build out -j
```

### Option 3: VS Code + CMake Tools (GCC)

1. Install [VS Code](https://code.visualstudio.com/) and a MinGW-w64 GCC toolchain (for example from [MSYS2](https://www.msys2.org/)). Make sure `gcc`, `g++` and `cmake` are on your `PATH`.
2. Install the **C/C++** and **CMake Tools** extensions.
3. Open the `Minecraft Clone` folder (the one containing `CMakeLists.txt`) in VS Code.
4. When asked to pick a kit, choose your **GCC (MinGW-w64)** kit. To change it later, run **CMake: Select a Kit** from the Command Palette.
5. Run **CMake: Select Variant** and choose `Release`, then build with **CMake: Build** (`F7`).
6. To run or debug, set the working directory to `Minecraft Clone/build` so the game can find its assets. One way is to add this to `.vscode/settings.json`:

   ```json
   {
     "cmake.debugConfig": {
       "cwd": "${workspaceFolder}/build"
     }
   }
   ```

   Then use **CMake: Run Without Debugging** (`Shift+F5`) or **CMake: Debug** (`Ctrl+F5`).

### Running

The game loads assets from `assets/` relative to the working directory. Visual Studio builds put the executable in `Minecraft Clone/build` next to the assets. For CMake builds, start the game from that folder:

```bash
cd "Minecraft Clone/build"
../out/voxel_game.exe
```

## Roadmap

- [ ] Vulkan rendering backend
- [ ] More world-generation features and structures
- [ ] More complete entity AI and gameplay systems
- [ ] Linux and macOS build support

## Disclaimer

This is a personal learning project. It is not affiliated with or endorsed by Mojang Studios or Microsoft.
